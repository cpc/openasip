/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ParsetStructs.hh
 *
 * Structures that assembler parser uses for storing parsed information.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#ifndef TCEASM_PARSER_STRUCTS_HH
#define TCEASM_PARSER_STRUCTS_HH

#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "Conversion.hh"

typedef unsigned long           UValue;
typedef long                    SValue;

/// Buffer size.
const UValue MAX_VALUE_LENGTH_IN_BYTES = 16; // oversized buffer...

/**
 * Parsed data of one BridgeTerm.
 */
class BusTerm {
public:
    /// Previous or next bus register.
    bool prev;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        if (prev) {
            return "{prev}";
        } else {
            return "{next}";
        }
    }
};

/**
 * Parsed data of function unit operand port or
 * special register reference.
 *
 * Form: unit.port[.operation]
 */
class FUTerm {
public:
    /// Is operation part of the term used.
    bool part3Used;
    /// Unit name
    std::string part1;
    /// Port name.
    std::string part2;
    /// Operation name.
    std::string part3;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::string retVal = part1 + "." + part2;
        if (part3Used) {
            retVal += "." + part3;
        }
        return retVal;
    }
};

/**
 * Parsed data of registerfile index or
 * function unit operation operand reference.
 *
 * Form: rf[.port].index and fu.operation.index
 */
class IndexTerm {
public:
    /// Is port name used.
    bool part2Used;
    /// Unit name.
    std::string part1;
    /// Port or operation name.
    std::string part2;
    /// Register or operand index.
    UValue index;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::string retVal = part1;
        if (part2Used) {
            retVal += "." + part2;
        }
        retVal += "." + Conversion::toString(index);
        return retVal;
    }
};

/**
 * Parsed data of register term
 *
 * This is used for presenting any type of port or register reference.
 *
 * Register term can be BusTerm, FUTerm or IndexTerm.
 */
class RegisterTerm {
public:

    /**
     * Register term types.
     */
    enum TermType {
        BUS,           ///< Bus term.
        FUNCTION_UNIT, ///< FU term.
        INDEX          ///< Index term.
    };

    /// Type of terminal that is represented by this object.
    TermType  type;

    /// The bus term, if type field is BUS. Otherwise not used.
    BusTerm   busTerm;
    /// The fu term, if type field is FUNCTION_UNIT. Otherwise not used.
    FUTerm    fuTerm;
    /// The index term, if type field is INDEX. Otherwise not used.
    IndexTerm indexTerm;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {

        switch (type) {
        case BUS:
            return busTerm.toString();
            break;

        case FUNCTION_UNIT:
            return fuTerm.toString();
            break;

        case INDEX:
            return indexTerm.toString();
            break;

        default:
            return "Unknown register term type";
        }
    }
};

/**
 * Parsed expression.
 *
 * Expression is label with possibly some additional information.
 *
 * Form: name[(+|-)offset][=literal]
 * e.g. myDataStructLabel+8=16, myCodeLabel=3, myDataStructLabel+8 or
 *      myDataStructLabel
 *
 * Part after '=' sign is just value of label, if value is always
 * resolved by compiler and it is also given by user values will
 * be compared to match.
 *
 * Offset can be used for example with structures,
 * if one want's to refer different datafields.
 */
class Expression {
public:
    /// Name of the label.
    std::string label;

    /// Is resolved value defined in struct.
    bool hasValue;
    /// Resolved value.
    UValue value;

    /// Is offset defined.
    bool hasOffset;
    /// Is offset minus.
    bool isMinus;
    /// Value of offset.
    UValue offset;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {

        std::stringstream retVal;

        retVal << label;

        if (hasOffset) {
            if (isMinus) {
                retVal << '-';
            } else {
                retVal << '+';
            }

            retVal << std::dec << offset;
        }

        if (hasValue) {
            retVal << "=" << std::hex << value;
        }
        
        return retVal.str();
    }
};

/**
 * Parsed literal or expression.
 */
class LiteralOrExpression {
public:
    
    LiteralOrExpression() : value(0) {}
    
    /// Does object contain expression or literal.
    bool isExpression;
    /// If expression the expression, Otherwise not used.
    Expression expression;

    /// If literal, the literal. Otherwise not used.
    UValue value;
    /// Sign of the value.
    bool isSigned;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::stringstream retVal;
        if (isExpression) {
            retVal << expression.toString();
        } else {
            retVal << std::dec << static_cast<int>(value);
        }
        return retVal.str();
    }        
};

/**
 * Source field of parsed move.
 *
 * Source can be either RegisterTerm or Immediate.
 */
class ParserSource {
public:
    /// Is source register or immediate reference.
    bool isRegister;
    /// If register, the register. Otherwise not used. 
    RegisterTerm        regTerm;
    /// If immediate value, the literal or expression. Otherwise not used. 
    LiteralOrExpression immTerm;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::stringstream retVal;
        if (isRegister) {
            retVal << regTerm.toString();
        } else {
            retVal << immTerm.toString();
        }
        return retVal.str();
    }
};

/**
 * Guard field of parsed move.
 */
class ParserGuard {
public:
    /// Is guard used.
    bool         isGuarded;
    /// Is guard inverted.
    bool         isInverted;
    /// Guard port or register.
    RegisterTerm regTerm;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::stringstream retVal;
        if (isGuarded) {
            if (isInverted) {
                retVal << '!';
            } else {
                retVal << '?';
            }

            retVal << regTerm.toString();
        }
        return retVal.str();
    }
};

/**
 * One init data field of data line.
 *
 *  Form: [width:]literalOrExpression
 *
 *  e.g.these are valid  4:codeLabel, 0b1010010, 0x139da  4:0x1abcd -4
 */
class InitDataField {
public:
    /// Number of MAUs that are initialized by the init field.
    UValue              width;
    /// Initialisation value.
    LiteralOrExpression litOrExpr;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::stringstream retVal;
        retVal << std::dec << (int)width << ":";
        if (litOrExpr.isExpression) {
            retVal << std::dec << (litOrExpr.expression.label);
        } else {
            retVal << std::dec << (int)(litOrExpr.value);
        }
        return retVal.str();
    }
};

/**
 * One annotation.
 */
class Annotation {
public:
    // Id of annotation
    UValue id;
    // Payload
    std::vector<InitDataField> payload;

    std::string toString() const {
        std::stringstream retVal;
        retVal << "[0x" << std::hex << id;
        
        for (unsigned int i = 0; i < payload.size(); i++) {
            retVal << " " << payload[i].toString();
        }

        retVal << "]";

        return retVal.str();
    }
};

/**
 * All info of one parsed instruction slot.
 *
 * Instruction slot defines a move or long immediate. An instruction slot can
 * also be empty (unused).
 */
class ParserMove {
public:
    /// Types of instruction slots.
    enum MoveType {
        EMPTY,          ///< Empty move slot.
        LONG_IMMEDIATE, ///< Encoding of one long immediate slot.
        TRANSPORT       ///< Data transport (move).
    };

    /// Type of move.
    MoveType type;

    /// Tells whether the slot is the first of the instruction.
    bool                isBegin;

    /// Guard field.
    ParserGuard         guard;
    /// Source field.
    ParserSource        source;
    /// Destination field.
    RegisterTerm        destination;
    /// Line number of source code for errors.
    UValue              asmLineNumber;

    std::vector<Annotation> annotationes;

    /**
     * Empty constructor.
     *
     * All values are set by hand.
     */
    ParserMove() {
    }
    
    /**
     * Constructor.
     *
     * @param aType Type of created move.
     * @param begin Is first move of an instruction.
     */
    ParserMove(MoveType aType, bool begin) :
        type(aType), isBegin(begin) {
    }

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {
        std::stringstream retVal;
        switch (type) {
        case EMPTY:
            retVal << "...";
            break;

        case LONG_IMMEDIATE:
            retVal << "[" << destination.toString() 
                   << "=" <<  source.toString()
                   << "]";

            for (unsigned int i = 0;i < annotationes.size();i++) {
                retVal << annotationes[i].toString();
            }

            break;

        case TRANSPORT:
            retVal << guard.toString() << " " 
                   << source.toString() << " -> "
                   << destination.toString();

            for (unsigned int i = 0;i < annotationes.size();i++) {
                retVal << annotationes[i].toString();
            }
            
            break;

        default:
            retVal << "Unknown move type!\n";
        }
        return retVal.str();
    }
};

/**
 * Parsed data area definition.
 */
class DataLine {
public:
    /// Number of MAUs initialized by this data line.
    UValue width;
    /// Address space whose MAUs are initialized.
    std::string dataSpace;

    /// Init data fields of data line. Uninitilized data line, if empty.
    std::vector<InitDataField> initData;
    /// Labels of this data line.
    std::vector<std::string> labels;

    /// Line number where in source code this DA line is found.
    UValue              asmLineNumber;

    /**
     * String representation of term for error message generation.
     */
    std::string toString() const {

        std::stringstream retVal;

        retVal << "DataLine:\n"
               << "address space:\t" << dataSpace << std::endl
               << "data maus: \t" << width << std::endl;

        retVal << "Labels:" << std::endl;
        for (unsigned int i = 0; i < labels.size(); i++) {
            retVal << "\t" << labels[i] << std::endl;
        }

        retVal << "Init data:" << std::endl;
        for (unsigned int i = 0; i < initData.size(); i++) {
            retVal << (int)initData[i].width << ":";

            if (initData[i].litOrExpr.isExpression) {
                retVal << initData[i].litOrExpr.expression.label;
            } else {
                retVal << (int)initData[i].litOrExpr.value;
            }

            retVal << "\t";

            if ((i+1)%16 == 0) retVal << std::endl;
        }
        
        retVal << "\n";

        return retVal.str();
    }
};

#endif
