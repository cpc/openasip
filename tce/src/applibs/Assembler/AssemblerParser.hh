/*
    Copyright (c) 2002-2017 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AssemblerParser.hh
 *
 * Syntax declarations and callbacks of assembler language.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2009
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TCEASM_ASSEMBLER_PARSER_HH
#define TCEASM_ASSEMBLER_PARSER_HH

#include <boost/version.hpp>

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")
#if BOOST_VERSION >= 103800

#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_increment_actor.hpp>
#include <boost/spirit/include/classic_assign_key_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>

using namespace boost::spirit::classic;

#else

#include <boost/spirit.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/assign_key_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>

using namespace boost::spirit;

#endif
POP_COMPILER_DIAGS

#include "Exception.hh"

#include "ParserStructs.hh"
#include "MachineResourceManager.hh"
#include "DataSectionCreator.hh"
#include "CodeSectionCreator.hh"
#include "LabelManager.hh"

#include "AssemblyParserDiagnostic.hh"

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

/**
 * Temporary structure, where all data is parsed before copying
 * it to right place.
 */
struct ParserTemp {
    ParserTemp() : lastNumber(0), lineNumber(1), codeLineCount(0) {}

    // Last parsed string literal.
    std::string sLiteral;

    /// Last parsed numeric value.
    UValue lastNumber;

    /// Should lastNumber be interpret as signed or unsigned value.
    bool isLastSigned;

    /// Last parsed index.
    UValue index;

    /// Last parsed bus reference.
    BusTerm busTerm;

    /// Last parsed unit.port[.operation] reference.
    FUTerm fuTerm;

    /// Last parsed rf[.port].index or fu.operation.index reference.
    IndexTerm indexTerm;

    /// Last parsed register term
    /// (copy of most recent parsed bus, fu or index term).
    RegisterTerm regTerm;

    /// Last parsed label reference form: name[(+|-)offset][=literal]
    Expression expression;

    /// Last parsed literal or expression.
    LiteralOrExpression litOrExpr;

    /// Last parsed source, destination and guard.
    ParserMove move;

    /// Last label name.
    std::string label;

    /// Last directive parameter string.
    std::string directive;

    /// Last init data field
    InitDataField initDataField;

    /// Last data line.
    DataLine dataLine;

    /// Line number of currently parsed line.
    UValue lineNumber;

    /// Line number of currently parsed code line.
    UValue codeLineCount;

    /// Last annotation
    Annotation annotation;
};

/**
 * Something about functors from Sprit manual
 * (those little classes below that can be used as like callback functions
 * with Spirit).
 *
 * With functors, take note that the operator() should be const. This
 * implies that functors are immutable. One may wish to have some member
 * variables that are modified when the action gets called. This is not a
 * good idea. First of all, functors are preferably lightweight. Functors
 * are passed around a lot and it would incur a lot of overhead if the
 * functors are heavily laden. Second, functors are passed by value. Thus,
 * the actual functor object that finally attaches to the parser, will
 * surely not be the original instance supplied by the client. What this
 * means is that changes to a functor's state will not affect the original
 * functor that the client passed in since they are distinct copies. If a
 * functor needs to update some state variables, which is often the case, it
 * is better to use references to external data.
 *
 * Personal notes:
 *
 * I preferred to use predefined functors, which can do some stl
 * container operations and assignments...
 *
 * I came across with few problems with spirit, I tried to do parser that
 * would have used pointer assignments and dynamic binding. Parser seems
 * to hard code some addresses, because address pointers and copy
 * parameters didn't seen to work too well.
 *
 * Here is some examples that didn't work.
 *
 * [assign_a(pointer, &object)] in this case I tried to set destination for
 * parsing to be object. I manage do this kind of assignment when I wrote
 * special actor for that.
 *
 * [assign_a(pointer->structField)] This doesn't work either. If you
 * want to set field through pointer, you have to create actor for that.
 *
 * Copying by passing pointers and references and and dynamic
 * binding is seems to be possible with Spirit, but you have to
 * write a lot more code. So I always copied parsed values that I used.
 */


/**
 * Prints given string to std::cerr. Used for syntax error.
 */
class PrintString {
public:
    PrintString(const char *aStr);
    PrintString(std::string &string);
    void operator() (const char*, const char*) const;

private:
    const char *str_;
};

/**
 * Actor for adding new code section.
 */
class NewCodeSectionActor {
public:
    NewCodeSectionActor(CodeSectionCreator& creator,
                        UValue& startAddress);

    void operator() (const char*, const char*) const;

private:
    CodeSectionCreator& creator_;
    UValue& startAddress_;
};

/**
 * Actor for adding moves to creator.
 */
class AddMoveActor {
public:
    AddMoveActor(CodeSectionCreator& creator,
                 const ParserMove& move);

    void operator() (const char*, const char*) const;

private:
    CodeSectionCreator& creator_;
    const ParserMove& move_;
};

/**
 * Actor for setting next start address.
 */
class SetStartAddressActor {
public:
	SetStartAddressActor(DataSectionCreator& creator,
                         UValue& startAddress);

	void operator() (const char*, const char*) const;

private:
	DataSectionCreator& creator_;
	UValue& startAddress_;
};

/**
 * Actor for adding data lines.
 */
class AddDataLineActor {
public:
	AddDataLineActor(DataSectionCreator& creator,
                     const DataLine& dataLine);

	void operator() (const char*, const char*) const;

private:
	DataSectionCreator& creator_;
	const DataLine& dataLine_;
};

/**
 * Actor for adding labels.
 */
class AddLabelActor {
public:
    AddLabelActor(LabelManager& manager,
                  TPEF::ASpaceElement& aSpace,
                  std::string& name, UValue& value);

    void operator() (const char*, const char*) const;

private:
    LabelManager& manager_;
    TPEF::ASpaceElement& aSpace_;
    std::string& name_;
    UValue& value_;
};

/**
 * Actor for adding procedures.
 */
class AddProcedureActor {
public:
    AddProcedureActor(
        LabelManager& manager,
        std::string& name, UValue& value);

    void operator() (const char*, const char*) const;

private:
    LabelManager& manager_;
    std::string& name_;
    UValue& value_;
};

/**
 * Actor for setting label global.
 */
class SetGlobalActor {
public:
    SetGlobalActor(LabelManager &manager);

    void operator() (const char* start, const char* end) const;

private:
    LabelManager& manager_;
};

// Spirit parser didn't work with literals like ParserInstruction::EMPTY,
// true, false, 0 etc. So I defined variables for literal assignments.

const bool MY_TRUE = true;
const bool MY_FALSE = false;
const UValue MY_ZERO = 0;

const ParserMove::MoveType EMPTY_MOVE = ParserMove::EMPTY;
const ParserMove::MoveType TRANSPORT_MOVE = ParserMove::TRANSPORT;
const ParserMove::MoveType LONG_IMM = ParserMove::LONG_IMMEDIATE;

const RegisterTerm::TermType BUS_TERM = RegisterTerm::BUS;
const RegisterTerm::TermType FUNCTION_UNIT_TERM = RegisterTerm::FUNCTION_UNIT;
const RegisterTerm::TermType INDEX_TERM = RegisterTerm::INDEX;

/// Empty parser move with isBegin flag set.
const ParserMove EMPTY_BEGIN_MOVE(ParserMove::EMPTY, true);

struct AssemblerParser : public grammar<AssemblerParser>
{

    /**
     * Grammar constructor.
     *
     * @param aBin TPEF where to program is compiled.
     * @param aMach Machine which for program is compiled.
     * @param parent Error message manager of the compiler.
     * @param codeLinesOnly If true start symbol for the grammar is
     *                      'codeLines'. The option is suitable for inline
     *                      assembly parsing. Default is false.
     */
    AssemblerParser(
        TPEF::Binary &aBin, const TTAMachine::Machine &aMach,
        AssemblyParserDiagnostic* parserDiagnostic,
        bool codeLinesOnly = false);

    template <typename ScannerT>
    struct definition {

        const AssemblerParser& parent_;

        // assembly grammar
        // for spirit syntax, see:
        // http://www.boost.org/libs/spirit/doc/quickref.html

        // if you define new patterns and it doesn't seem to work, try
        // adding parenthesis... this works quite weird some times.

        // NOTE! Coding guidelines does not apply for this part... NOTE!
        // because code is more like BNF description than C

        definition(AssemblerParser const& self) : parent_(self) {

            // ------------------------------------------------------------
            // omits comments and update current line couter
            // ------------------------------------------------------------
            comment = str_p("#") >>
                (*(anychar_p - '\n') >> ch_p('\n'))
                [increment_a(self.parserTemp_.lineNumber)]
                ;

            // ------------------------------------------------------------
            // comment or any white empty space
            // ------------------------------------------------------------
            my_space =
                comment
                |

                str_p("\n")
                [increment_a(self.parserTemp_.lineNumber)]
                |

                space_p;

            // ------------------------------------------------------------
            // all literals are stored to parserTemp_.lastNumber field
            // ------------------------------------------------------------
            binNumber = str_p("0b") >> bin_p
                [assign_a(self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.isLastSigned, MY_FALSE)];

            hexNumber = str_p("0x") >> hex_p
                [assign_a(self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.isLastSigned, MY_FALSE)];

            uNumber = uint_p
                [assign_a(self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.isLastSigned, MY_FALSE)];

            sNumber = int_p
                [assign_a(self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.isLastSigned, MY_TRUE)];

            // ------------------------------------------------------------
            // unsigned literal
            // ------------------------------------------------------------
            uLiteral = binNumber|hexNumber|uNumber;

            // ------------------------------------------------------------
            // convert backslashed character " and also 
            // accept plain \-character 
            // ------------------------------------------------------------
            backslashedChars =
                str_p("\\\"")
                ;

            // ------------------------------------------------------------
            // string literal
            // ------------------------------------------------------------
            sLiteral =
                eps_p
                [assign_a(self.parserTemp_.sLiteral, "")] >>

                (ch_p('\"') >>

                 +(backslashedChars |
                   (anychar_p - ch_p('\"')) ) >>

                 ch_p('\"'))
                [PrintString(std::string(
                    "String literals are not supported yet!\n").c_str())]
                ;

            // ------------------------------------------------------------
            // any literal
            // ------------------------------------------------------------
            literal = uLiteral|sNumber|sLiteral;

            // ------------------------------------------------------------
            // any valid string
            // NOTE: we are not as liberal as ADF specification,
            // we don't allow ':'
            // ------------------------------------------------------------
            name = (alpha_p | chset_p("_")) >> *(alnum_p | chset_p("_"));

            // ------------------------------------------------------------
            // index is unsigned number and stored to parserTemp_.index field.
            // ------------------------------------------------------------
            index = uNumber[assign_a(self.parserTemp_.index,
                                     self.parserTemp_.lastNumber)];

            // ------------------------------------------------------------
            // {prev} and {next} reference which are not yet supported
            // ------------------------------------------------------------
            busTerm = '{' >>
                (str_p("prev")
                 [assign_a(self.parserTemp_.busTerm.prev, MY_TRUE)]|
                 str_p("next")
                 [assign_a(self.parserTemp_.busTerm.prev, MY_FALSE)]) >> '}';

            // ------------------------------------------------------------
            // unit.port[.operation] is stored to parserTemp_.fuTerm field
            // ------------------------------------------------------------
            fuTerm =
                eps_p
                [assign_a(self.parserTemp_.fuTerm.part3Used, MY_FALSE)] >>

                name
                [assign_a(self.parserTemp_.fuTerm.part1)] >> '.' >>

                name
                [assign_a(self.parserTemp_.fuTerm.part2)] >>

                !('.' >> name[assign_a(self.parserTemp_.fuTerm.part3)])
                [assign_a(self.parserTemp_.fuTerm.part3Used, MY_TRUE)];

            // ------------------------------------------------------------
            // rf[.port].index and fu.operation.index is stored to 
            // parserTemp_.indexTerm field
            // ------------------------------------------------------------
            indexTerm =
                eps_p
                [assign_a(self.parserTemp_.indexTerm.part2Used, MY_FALSE)] >>

                name
                [assign_a(self.parserTemp_.indexTerm.part1)] >> '.' >>

                !(name[assign_a(self.parserTemp_.indexTerm.part2)] >> '.')
                [assign_a(self.parserTemp_.indexTerm.part2Used, MY_TRUE)] >>

                index
                [assign_a(self.parserTemp_.indexTerm.index, 
                          self.parserTemp_.index)];

            // ------------------------------------------------------------
            // copy last parsed term and its type to regTerm
            // ------------------------------------------------------------
            regTerm =
                indexTerm
                [assign_a(self.parserTemp_.regTerm.indexTerm, 
                          self.parserTemp_.indexTerm)]
                [assign_a(self.parserTemp_.regTerm.type, INDEX_TERM)]|

                fuTerm
                [assign_a(self.parserTemp_.regTerm.fuTerm, 
                          self.parserTemp_.fuTerm)]
                [assign_a(self.parserTemp_.regTerm.type, FUNCTION_UNIT_TERM)]|

                busTerm
                [assign_a(self.parserTemp_.regTerm.busTerm, 
                          self.parserTemp_.busTerm)]
                [assign_a(self.parserTemp_.regTerm.type, BUS_TERM)];


            // ------------------------------------------------------------
            // offset part of expression stored to parserTemp_.expression
            // ------------------------------------------------------------
            offset =
                sign_p[assign_a(self.parserTemp_.expression.isMinus)] >> 
                
                *my_space >>
                
                uLiteral
                [assign_a(self.parserTemp_.expression.offset, 
                          self.parserTemp_.lastNumber)];

            // ------------------------------------------------------------
            //  name[(+|-)offset] stored to parserTemp_.expression
            // ------------------------------------------------------------
            symExpression =
                name
                [assign_a(self.parserTemp_.expression.label)] >>

                !(*my_space >> offset)
                [assign_a(self.parserTemp_.expression.hasOffset, MY_TRUE)];

            // ------------------------------------------------------------
            // name[(+|-)offset][=literal] stored to parserTemp_.expression
            // ------------------------------------------------------------
            expression =
                eps_p
                [assign_a(self.parserTemp_.expression.hasValue, MY_FALSE)]
                [assign_a(self.parserTemp_.expression.hasOffset, MY_FALSE)] >>

                symExpression >>

                !(*my_space >> '=' >> *my_space >> literal)
                [assign_a(self.parserTemp_.expression.value, 
                          self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.expression.hasValue, MY_TRUE)];

            // ------------------------------------------------------------
            // either literal or expression
            // ------------------------------------------------------------
            literalOrExpression =
                literal
                [assign_a(self.parserTemp_.litOrExpr.value, 
                          self.parserTemp_.lastNumber)]
                [assign_a(self.parserTemp_.litOrExpr.isExpression, MY_FALSE)]
                [assign_a(self.parserTemp_.litOrExpr.isSigned, 
                          self.parserTemp_.isLastSigned)]|

                expression
                [assign_a(self.parserTemp_.litOrExpr.expression, 
                          self.parserTemp_.expression)]
                [assign_a(self.parserTemp_.litOrExpr.isExpression, MY_TRUE)];

            // ------------------------------------------------------------
            // store immediate term of move to parserTemp_.move
            // ------------------------------------------------------------
            immTerm =
                literalOrExpression
                [assign_a(self.parserTemp_.move.source.immTerm, 
                          self.parserTemp_.litOrExpr)];

            // ------------------------------------------------------------
            // store destination
            // ------------------------------------------------------------
            destination =
                regTerm
                [assign_a(self.parserTemp_.move.destination, 
                          self.parserTemp_.regTerm)];

            // ------------------------------------------------------------
            // store source
            // ------------------------------------------------------------
            source =
                regTerm
                [assign_a(self.parserTemp_.move.source.regTerm, 
                          self.parserTemp_.regTerm)]
                [assign_a(self.parserTemp_.move.source.isRegister, MY_TRUE)]|

                immTerm
                [assign_a(self.parserTemp_.move.source.isRegister, MY_FALSE)];

            // ------------------------------------------------------------
            // guards type
            // ------------------------------------------------------------
            invertFlag =
                ch_p('?')[assign_a(self.parserTemp_.move.guard.isInverted, 
                                   MY_FALSE)]|

                ch_p('!')[assign_a(self.parserTemp_.move.guard.isInverted, 
                                   MY_TRUE)];

            // ------------------------------------------------------------
            // port or register guard
            // ------------------------------------------------------------
            guard =
                invertFlag >> *my_space >> regTerm
                [assign_a(self.parserTemp_.move.guard.regTerm, 
                          self.parserTemp_.regTerm)];

            // ------------------------------------------------------------
            // [field-size:]init-value one init data field of data area 
            // definition
            // ------------------------------------------------------------
            initDataField =
                eps_p
                [assign_a(self.parserTemp_.initDataField.width, MY_ZERO)] >>

                !(uNumber >> *my_space >> ':' >> *my_space)
                [assign_a(self.parserTemp_.initDataField.width, 
                          self.parserTemp_.lastNumber)] >>

                literalOrExpression
                [assign_a(self.parserTemp_.initDataField.litOrExpr,
                          self.parserTemp_.litOrExpr)];

            // ------------------------------------------------------------
            // annotationes
            // 
            // Only supported syntax for annotation is 
            // "{hexId value value ...}" 
            // ------------------------------------------------------------
            annotation =
                eps_p
                [clear_a(self.parserTemp_.annotation.payload)] >>

                (ch_p('{') >> *my_space >> 
                 hexNumber[assign_a(self.parserTemp_.annotation.id,
                                    self.parserTemp_.lastNumber)] >>

                 *(+my_space >> 
                   initDataField[push_back_a(self.parserTemp_.annotation.payload,
                                             self.parserTemp_.initDataField)]) >>

                 *my_space >> ch_p('}'));

            // ------------------------------------------------------------
            // one move that moves data through a bus
            // ------------------------------------------------------------
            transport =
                eps_p
                [assign_a(self.parserTemp_.move.guard.isGuarded, MY_FALSE)] 
                [clear_a(self.parserTemp_.move.annotationes)] >>

                !(guard >> *my_space)
                [assign_a(self.parserTemp_.move.guard.isGuarded, MY_TRUE)] >>

                source >>
                *my_space >> str_p("->") >> *my_space >>

                destination >> 
                
                *(*my_space >> annotation[push_back_a(
                                              self.parserTemp_.move.annotationes,
                                              self.parserTemp_.annotation)]);

            // ------------------------------------------------------------
            // long immediates
            // ------------------------------------------------------------
            immediateSpec =
                // moves destination stores immediate unit and register.
                eps_p
                [clear_a(self.parserTemp_.move.annotationes)] >>

                ch_p('[') >> *my_space >> 
                destination >>

                *my_space >> '=' >> *my_space >>

                // moves source is used as a value
                literalOrExpression
                [assign_a(self.parserTemp_.move.source.immTerm, 
                          self.parserTemp_.litOrExpr)]
                [assign_a(self.parserTemp_.move.source.isRegister, 
                          MY_FALSE)] >> 

                 *my_space >> ch_p(']') >> 

                 *(*my_space >> annotation[push_back_a(
                                               self.parserTemp_.move.annotationes,
                                               self.parserTemp_.annotation)]);
            // ------------------------------------------------------------
            // empty move
            // ------------------------------------------------------------
            emptyMove = str_p("...");

            // ------------------------------------------------------------
            // one immediate, transport or empty move
            // ------------------------------------------------------------
            move =
                emptyMove
                [assign_a(self.parserTemp_.move.type, EMPTY_MOVE)] |

                transport
                [assign_a(self.parserTemp_.move.type, TRANSPORT_MOVE)];

            // ------------------------------------------------------------
            // parses instruction and adds moves to CodeSectionCreator
            // ------------------------------------------------------------
            moves =
                move
                [assign_a(self.parserTemp_.move.isBegin, MY_TRUE)]
                [assign_a(self.parserTemp_.move.asmLineNumber,
                          self.parserTemp_.lineNumber)]
                [AddMoveActor(
                    self.codeSectionCreator_, self.parserTemp_.move)] >>

                *(*my_space >> ',' >> *my_space >>
                  move
                  [assign_a(self.parserTemp_.move.isBegin, MY_FALSE)]
                  [assign_a(self.parserTemp_.move.asmLineNumber, 
                            self.parserTemp_.lineNumber)]
                  [AddMoveActor(
                      self.codeSectionCreator_, self.parserTemp_.move)]);

            // ------------------------------------------------------------
            // empty instruction
            // ------------------------------------------------------------
            emptyInstruction =
                str_p(". . .");

            // ------------------------------------------------------------
            // empty or populated instruction
            // ------------------------------------------------------------
            instruction =
                emptyInstruction
                [AddMoveActor(
                    self.codeSectionCreator_, EMPTY_BEGIN_MOVE)] |
                moves;

            // ------------------------------------------------------------
            // label definitions
            // ------------------------------------------------------------
            label =
                (name[assign_a(self.parserTemp_.label)] >> *my_space >> ':') ;

            // ------------------------------------------------------------
            // code labels can be added straight to label manager, 
            // since we don't have to resolve address of label separately
            // ------------------------------------------------------------
            codeLabel =
                label
                [AddLabelActor(
                    self.labelManager_,
                    *self.resourceManager_.codeAddressSpace(),
                    self.parserTemp_.label,
                    self.parserTemp_.codeLineCount)];

            // ------------------------------------------------------------
            // directive definitions
            // ------------------------------------------------------------
            directive =
                ch_p(':') >> *my_space >>

                ((str_p("procedure") >> +my_space >>
                  (name[assign_a(self.parserTemp_.directive)])

                  [AddProcedureActor(
                      self.labelManager_,
                      self.parserTemp_.directive,
                      self.parserTemp_.codeLineCount)]) |

                 (str_p("global") >> +my_space >>
                  name[SetGlobalActor(self.labelManager_)]));

            // ------------------------------------------------------------
            // one code line terminated by semicolon
            // ------------------------------------------------------------
            codeLine =
                *(codeLabel >> *my_space) >>

                instruction
                [increment_a(self.parserTemp_.codeLineCount)] >>

                // immediate encoding from dedicated instruction slots
                *(*my_space >> immediateSpec)
                [assign_a(self.parserTemp_.move.type, LONG_IMM)]
                [AddMoveActor(
                    self.codeSectionCreator_, self.parserTemp_.move)];

            // ------------------------------------------------------------
            // body of code section (instructions and directives)
            // ------------------------------------------------------------
            codeLines =
                *((codeLine|directive) >>
                  *my_space >> ';' >> *my_space);

            // ------------------------------------------------------------
            // code header with start address
            // ------------------------------------------------------------
            codeHeader =
                eps_p
                [assign_a(self.parserTemp_.lastNumber, MY_ZERO)] >>

                str_p("CODE") >>
                !(+my_space >> literal) >> *my_space >> ';';

            // ------------------------------------------------------------
            // code section (header and body)
            // ------------------------------------------------------------
            codeArea =
                codeHeader
                [NewCodeSectionActor(
                    self.codeSectionCreator_, self.parserTemp_.lastNumber)] >>

                +my_space >> codeLines;

            // ------------------------------------------------------------
            // all init data fields of data line
            // ------------------------------------------------------------
            initData =
                *(+my_space >>
                  initDataField
                  [push_back_a(self.parserTemp_.dataLine.initData, 
                               self.parserTemp_.initDataField)]);

            // ------------------------------------------------------------
            // data label definitions
            // ------------------------------------------------------------
            dataLabel =
                label
                [push_back_a(self.parserTemp_.dataLine.labels, 
                             self.parserTemp_.label)];

            // ------------------------------------------------------------
            // one data line with data are definition and label
            // ------------------------------------------------------------
            dataLine =
                *(dataLabel >> *my_space) >>

                str_p("DA") >> +my_space >>

                uNumber[assign_a(self.parserTemp_.dataLine.width,
                                 self.parserTemp_.lastNumber)] >>

                initData;

            // ------------------------------------------------------------
            // body of data section
            // ------------------------------------------------------------
            dataLines =
                *((dataLine
                   [assign_a(self.parserTemp_.dataLine.asmLineNumber, 
                             self.parserTemp_.lineNumber)]
                   [AddDataLineActor(
                       self.dataSectionCreator_, self.parserTemp_.dataLine)]
                   [clear_a(self.parserTemp_.dataLine.initData)]
                   [clear_a(self.parserTemp_.dataLine.labels)] |

                   directive) >>

                  *my_space >> ';' >> *my_space);

            // ------------------------------------------------------------
            // data section header with address space name and start address
            // ------------------------------------------------------------
            dataHeader =
                str_p("DATA") >>
                +my_space >> 
                
                name[assign_a(self.parserTemp_.dataLine.dataSpace)] >>

                !(+my_space >> literal)
                [SetStartAddressActor(self.dataSectionCreator_, 
                                      self.parserTemp_.lastNumber)] >> 
                
                *my_space >> ';';

            // ------------------------------------------------------------
            // data section definition (header and boby)
            // ------------------------------------------------------------
            dataArea =
                dataHeader >>
                +my_space >> dataLines;

            // ------------------------------------------------------------
            // one data or code section
            // ------------------------------------------------------------
            area =
                dataArea | codeArea;

            // ------------------------------------------------------------
            // whole program (multiple sections)
            // ------------------------------------------------------------
            program =
                (*(*my_space >> area) >> *my_space);
        }

        // all recoginition patterns
        rule<ScannerT>
     	    comment, my_space, port, name, program, area, dataArea, 
            dataHeader, dataLines, dataLine, initData, 
            uLiteral, literal, codeArea, codeHeader, codeLines, codeLine,
            literalOrExpression, label,codeLabel, instruction, 
            emptyInstruction, moves, move, emptyMove, directive, 
            immediateSpec, transport, annotation, initDataField, guard, 
            invertFlag, source, destination, dataLabel, uNumber, sNumber, 
            hexNumber, binNumber, index, fuTerm, 
            indexTerm, sLiteral, backslashedChars, regTerm, offset, 
            symExpression, expression, immTerm, busTerm;

        /**
         * Spirits grammar interface.
         */
        rule<ScannerT> const& start() const {
            if (parent_.codeLinesOnly()) {
                return codeLines;
            } else {
                return program;
            }
        }
    };

public:
    bool compile(const std::string& asmCode) const;

    void finalize(bool littleEndian) const;

    void cleanup();

    UValue errorLine();

    bool codeLinesOnly() const { return codeLinesOnly_; }

private:

    /// TPEF where to program is compiled.
    TPEF::Binary &bin_;

    /// Generates string, resource, null and address space sections.
    mutable MachineResourceManager resourceManager_;
    /// Creates data sections.
    mutable DataSectionCreator dataSectionCreator_;
    /// Creates code section.
    mutable CodeSectionCreator codeSectionCreator_;
    /// Creates symbol and relocation sections.
    mutable LabelManager labelManager_;

    /// Temp-structure containing most recent parsed tokens.
    mutable ParserTemp parserTemp_;

    /// Controls start symbol of the grammar. False: the default symbol
    /// (program) for parsing assembly files. True: 'codeLines' symbol
    /// suitable for inline assembly parsing.
    bool codeLinesOnly_ = false;
};

#endif
