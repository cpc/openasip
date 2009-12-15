/**
 * @file OperationDAGLanguageParser.hh
 * 
 * OsalDAGLanguage parser based on Ansi C grammar checker.
 *
 * Modified to do other things that just validating.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2009
 * @rating red 
 */

#ifndef TTA_OPERATION_DAG_LANGUAGE_PARSER
#define TTA_OPERATION_DAG_LANGUAGE_PARSER

/*===========================================================================
    C Grammar checker
    Requires Spirit v1.6.0 or later

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Adapted from:
        http://www.lysator.liu.se/c/ANSI-C-grammar-y.html
        http://www.lysator.liu.se/c/ANSI-C-grammar-l.html

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
============================================================================*/

//////////////////////////////////////////////////////////////////////////////
// version history
//
// TODO:    Handling typedef's, without that we can't correctly parse
//            auxilliary valid C sources. But hey it's a sample.
//
// Modified: 20.09.2002 21:58:45
//  Ported to Spirit V1.5
//
// Modified: 18.10.2001 16:36:07
//  Corrected: String literals may be splitted in several strings: "abc" "def"
//  Corrected: String literals may have length 0 ("")
//
// Modified: 17.10.2001 10:56:57
//  Rewritten:  *_CONSTANT rules
//  Introduced: operator '||' where possible
//  Corrected:  IDENTIFIER can start with '_' and can contain '$'
//  Added:      Skipping of '#line' and '#pragma' directives, which contained
//                in preprocessed files of the Intel V5.0.1 compiler
//
// Modified: 16.10.2001 21:12:05
//  Corrected: missing lexeme_d[] statements for *_CONSTANT
//  Corrected: missing longest_d[] for CONSTANT

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#if defined(_DEBUG)
#define BOOST_SPIRIT_DEBUG
#endif // defined(_DEBUG)

#include <boost/version.hpp>

#if BOOST_VERSION >= 103800

#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_utility.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_assign_key_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/classic_decrement_actor.hpp>
#include <boost/spirit/include/classic_erase_actor.hpp> 
#include <boost/spirit/include/classic_increment_actor.hpp>
#include <boost/spirit/include/classic_insert_key_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_push_front_actor.hpp>
#include <boost/spirit/include/classic_swap_actor.hpp>

using namespace boost::spirit::classic;

#else

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/symbols.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/assign_key_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
#include <boost/spirit/actor/decrement_actor.hpp>
#include <boost/spirit/actor/erase_actor.hpp> 
#include <boost/spirit/actor/increment_actor.hpp>
#include <boost/spirit/actor/insert_key_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/push_front_actor.hpp>
#include <boost/spirit/actor/swap_actor.hpp>

using namespace boost::spirit;

#endif

#include "OperationDAG.hh"
#include "Conversion.hh"

// There is a g++ (3.1 and 3.2) bug on MINGW that prohibits
// the use of the identifiers below. The following is a workaround.

#if ((__GNUC__ == 3) && (__GNUC_MINOR__ == 1 || __GNUC_MINOR__ == 2) \
    && defined(_WINNT_H))
#  ifdef CONST
#    undef CONST
#  endif
#  ifdef VOID
#    undef VOID
#  endif
#  define CHAR CHAR_RULE
#  define CONST CONST_RULE
#  define FLOAT FLOAT_RULE
#  define INT INT_RULE
#  define VOID VOID_RULE
#endif

/////////////////////////////////////////////////////////////////////////////
// used namespaces
using namespace boost::spirit;
using namespace std;


/**
 * Class which is used by Spirit parser to create tokenized
 * tree form of the source code.
 */

class TokenizerData {
public:

    /**
     * List of different type tokens.
     */
    enum OperationID {        
        IDENTIFIER,
        STRING_LITERAL,
        INTEGER_LITERAL,
        FLOAT_LITERAL,
        PRIMARY_EXPRESSION,
        UNARY_EXPRESSION,
        UNARY_ADDROF,
        UNARY_STAR,
        UNARY_PLUS,
        UNARY_MINUS,
        UNARY_TILDE,
        UNARY_BANG,
        POSTFIX_EXPRESSION,
        CAST_EXPRESSION,
        MULTIPLICATIVE_EXPRESSION,
        ADDITIVE_EXPRESSION,
        SHIFT_EXPRESSION,
        RELATIONAL_EXPRESSION,
        EQUALITY_EXPRESSION,
        AND_EXPRESSION,
        EXCLUSIVE_EXPRESSION,
        INCLUSIVE_EXPRESSION,
        LOGICAL_AND_EXPRESSION,
        LOGICAL_OR_EXPRESSION,
        CONDITIONAL_EXPRESSION,
        ASSIGNMENT_EXPRESSION,
        ASSIGNMENT_OPERATOR,
        EXPRESSION,
        CONSTANT_EXPRESSION,
        DECLARATION,
        DECLARATION_SPECIFIERS,
        INIT_DECLARATOR,
        STORAGE_CLASS_SPECIFIER,
        TYPE_SPECIFIER,
        STRUCT_OR_UNION_SPECIFIER,
        STRUCT_OR_UNION,
        STRUCT_DECLARATION,
        STRUCT_DECLARATOR,
        ENUM_SPECIFIER,
        ENUMERATOR,
        CONST_QUALIFER,
        VOLATILE_QUALIFER,
        TYPE_QUALIFER,
        DECLARATOR,
        INITIALIZER,
        DIRECT_DECLARATOR,
        POINTER,
        PARAMETER_TYPE_LIST,
        PARAMETER_LIST,
        PARAMETER_DECLARATION,
        IDENTIFIER_LIST,
        TYPE_NAME,
        ABSTRACT_DECLARATOR,
        DIRECT_ABSTRACT_DECLARATOR,
        INITIALIZER_LIST,
        STATEMENT,
        LABELED_STATEMENT,
        COMPOUND_STATEMENT,
        STATEMENT_LIST,
        EXPRESSION_STATEMENT,
        SELECTION_STATEMENT,
        ITERATION_STATEMENT,
        JUMP_STATEMENT,
        FUNCTION_DEFINITION,
        EXTERNAL_DECLARATION,
        INVALID_TOKEN
    };

    /**
     * Prints string value for each token.
     *
     * @param id Token type.
     * @return String for token type.
     */
    static std::string idString(OperationID id) {
        switch (id) {
        case IDENTIFIER: return "IDENTIFIER:";
        case STRING_LITERAL: return "STRING_LITERAL:";
        case INTEGER_LITERAL: return "INTEGER_LITERAL:";
        case FLOAT_LITERAL: return "FLOAT_LITERAL:";
        case PRIMARY_EXPRESSION: return "PRIMARY_EXPRESSION:";
        case UNARY_EXPRESSION: return "UNARY_EXPRESSION:";
        case UNARY_ADDROF: return "UNARY_ADDROF:";
        case UNARY_STAR: return "UNARY_STAR:";
        case UNARY_PLUS: return "UNARY_PLUS:";
        case UNARY_MINUS: return "UNARY_MINUS:";
        case UNARY_TILDE: return "UNARY_TILDE:";
        case UNARY_BANG: return "UNARY_BANG:";
        case POSTFIX_EXPRESSION: return "POSTFIX_EXPRESSION:";
        case CAST_EXPRESSION: return "CAST_EXPRESSION:";
        case MULTIPLICATIVE_EXPRESSION: return "MULTIPLICATIVE_EXPRESSION:";
        case ADDITIVE_EXPRESSION: return "ADDITIVE_EXPRESSION:";
        case SHIFT_EXPRESSION: return "SHIFT_EXPRESSION:";
        case RELATIONAL_EXPRESSION: return "RELATIONAL_EXPRESSION:";
        case EQUALITY_EXPRESSION: return "EQUALITY_EXPRESSION:";
        case AND_EXPRESSION: return "AND_EXPRESSION:";
        case EXCLUSIVE_EXPRESSION: return "EXCLUSIVE_EXPRESSION:";
        case INCLUSIVE_EXPRESSION: return "INCLUSIVE_EXPRESSION:";
        case LOGICAL_AND_EXPRESSION: return "LOGICAL_AND_EXPRESSION:";
        case LOGICAL_OR_EXPRESSION: return "LOGICAL_OR_EXPRESSION:";
        case CONDITIONAL_EXPRESSION: return "CONDITIONAL_EXPRESSION:";
        case ASSIGNMENT_EXPRESSION: return "ASSIGNMENT_EXPRESSION:";
        case ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR:";
        case EXPRESSION: return "EXPRESSION:";
        case CONSTANT_EXPRESSION: return "CONSTANT_EXPRESSION:";
        case DECLARATION: return "DECLARATION:";
        case DECLARATION_SPECIFIERS: return "DECLARATION_SPECIFIERS:";
        case INIT_DECLARATOR: return "INIT_DECLARATOR:";
        case STORAGE_CLASS_SPECIFIER: return "STORAGE_CLASS_SPECIFIER:";
        case TYPE_SPECIFIER: return "TYPE_SPECIFIER:";
        case STRUCT_OR_UNION_SPECIFIER: return "STRUCT_OR_UNION_SPECIFIER:";
        case STRUCT_OR_UNION: return "STRUCT_OR_UNION:";
        case STRUCT_DECLARATION: return "STRUCT_DECLARATION:";
        case STRUCT_DECLARATOR: return "STRUCT_DECLARATOR:";
        case ENUM_SPECIFIER: return "ENUM_SPECIFIER:";
        case ENUMERATOR: return "ENUMERATOR:";
        case CONST_QUALIFER: return "CONST_QUALIFER:";
        case VOLATILE_QUALIFER: return "VOLATILE_QUALIFER:";
        case TYPE_QUALIFER: return "TYPE_QUALIFER:";
        case DECLARATOR: return "DECLARATOR:";
        case INITIALIZER: return "INITIALIZER:";
        case DIRECT_DECLARATOR: return "DIRECT_DECLARATOR:";
        case POINTER: return "POINTER:";
        case PARAMETER_TYPE_LIST: return "PARAMETER_TYPE_LIST:";
        case PARAMETER_LIST: return "PARAMETER_LIST:";
        case PARAMETER_DECLARATION: return "PARAMETER_DECLARATION:";
        case IDENTIFIER_LIST: return "IDENTIFIER_LIST:";
        case TYPE_NAME: return "TYPE_NAME:";
        case ABSTRACT_DECLARATOR: return "ABSTRACT_DECLARATOR:";
        case DIRECT_ABSTRACT_DECLARATOR: return "DIRECT_ABSTRACT_DECLARATOR:";
        case INITIALIZER_LIST: return "INITIALIZER_LIST:";
        case STATEMENT: return "STATEMENT:";
        case LABELED_STATEMENT: return "LABELED_STATEMENT:";
        case COMPOUND_STATEMENT: return "COMPOUND_STATEMENT:";
        case STATEMENT_LIST: return "STATEMENT_LIST:";
        case EXPRESSION_STATEMENT: return "EXPRESSION_STATEMENT:";
        case SELECTION_STATEMENT: return "SELECTION_STATEMENT:";
        case ITERATION_STATEMENT: return "ITERATION_STATEMENT:";
        case JUMP_STATEMENT: return "JUMP_STATEMENT:";
        case FUNCTION_DEFINITION: return "FUNCTION_DEFINITION:";
        case EXTERNAL_DECLARATION: return "EXTERNAL_DECLARATION:";
        case INVALID_TOKEN: return "INVALID_TOKEN:";            
        default: return "UNKNOWN_ID:";
        }        
    }
    
    /**
     * Represents one token of source code.   
     */ 
    class Token {
    public:
        Token(unsigned long start, unsigned long end, std::string& strValue) : 
            start_(start), end_(end), strValue_(strValue),
            type_(INVALID_TOKEN),
            isIdentifier_(false),
            isStringLiteral_(false),
            isIntegerLiteral_(false),
            isFloatLiteral_(false),
            isUnaryExpression_(false),
            isAddrofOperator_(false),
            isAsteriskOperator_(false),
            isPlusOperator_(false),
            isMinusOperator_(false),
            isTildeOperator_(false),
            isBangOperator_(false) {}
        
        bool isIdentifier() const {return isIdentifier_;}
        bool isStringLiteral() const {return isStringLiteral_;}
        bool isIntegerLiteral() const {return isIntegerLiteral_;}
        bool isFloatLiteral() const {return isFloatLiteral_;}
        bool isUnaryExpression() const {return isUnaryExpression_;}
        bool isAddrofOperator() const {return isAddrofOperator_;}
        bool isAsteriskOperator() const {return isAsteriskOperator_;}
        bool isPlusOperator() const {return isPlusOperator_;}
        bool isMinusOperator() const {return isMinusOperator_;}
        bool isTildeOperator() const {return isTildeOperator_;}
        bool isBangOperator() const {return isBangOperator_;}
        
        int intValue() const {
            if (!isIntegerLiteral()) {
                // TODO: throw exception
                assert(false && "Node is not integer literal.");
            }
            return Conversion::toInt(strValue_);
        }
        
        std::string stringValue() const {
            return strValue_;
        }
        
        bool isPostfixExpression() const {return false;}
        
        void setProperty(OperationID id) {
            switch (id) {
            case IDENTIFIER: isIdentifier_ = true; break;
            case STRING_LITERAL: isStringLiteral_ = true; break;
            case INTEGER_LITERAL: isIntegerLiteral_ = true; break;
            case FLOAT_LITERAL: isFloatLiteral_ = true; break;
            case UNARY_EXPRESSION: isUnaryExpression_ = true; break;
            case UNARY_ADDROF: isAddrofOperator_ = true; break;
            case UNARY_STAR: isAsteriskOperator_ = true; break;
            case UNARY_PLUS: isPlusOperator_ = true; break;
            case UNARY_MINUS: isMinusOperator_ = true; break;
            case UNARY_TILDE: isTildeOperator_ = true; break;
            case UNARY_BANG: isBangOperator_ = true; break;
            default: ;
            }
            
            type_ = id;
        }
        
        unsigned long start_;
        unsigned long end_;
        std::string strValue_;
        OperationID type_;
        
        bool isIdentifier_;
        bool isStringLiteral_;
        bool isIntegerLiteral_;
        bool isFloatLiteral_;
        bool isUnaryExpression_;
        bool isAddrofOperator_;
        bool isAsteriskOperator_;
        bool isPlusOperator_;
        bool isMinusOperator_;
        bool isTildeOperator_;
        bool isBangOperator_;
    };
    
    /**
     * One node of tree of tokens. 
     *
     * Each node contains reference to it's token and it's sub-token-nodes..
     * 
     * root
     *    \
     *   function
     *     /   \
     *   name postfix
     *         /  |
     *      name  name
     */
    class TokenTreeNode {
    public:
        TokenTreeNode() : data_(NULL) {}
        TokenTreeNode(Token& token) : data_(&token) {}
        
        /**
         * Returns true if node contain tokens of function call.
         *
         * leafs must be:
         * PRIMARY_EXPRESSION, POSTFIX_EXPRESSION
         *
         * @return true if node is function call.
         */ 
        bool isFunctionCall() const {
            if (leafCount() == 2 && 
                leaf(0).token().type_ == PRIMARY_EXPRESSION &&
                leaf(1).token().type_ == POSTFIX_EXPRESSION) {
                return true;
            }
            return false;
        }
        
        /**
         * Returns true if node contain tokens of assignment
         *
         * leafs must be:
         * ANY, ASSIGNMENT_OPERATOR, ANY
         *
         * @return true if node is assginment.
         */ 
        bool isAssignment() const {
            if (leafCount() == 3 && 
                leaf(1).token().type_ == ASSIGNMENT_OPERATOR) {
                return true;
            }            
            return false;
        }
        
        /**
         * Returns true if node contain integer value.
         *
         * leafs must be: none, or (+|-, isIntegerLiteral)
         *
         * @return true if node is integer value.
         */ 
        bool isInteger() const {
            if (
                // if two part integer e.g. -1 or +1 
                (leafCount() == 2 && 
                 leaf(1).token().isIntegerLiteral() &&                  
                 (leaf(0).token().isPlusOperator() || 
                  leaf(0).token().isMinusOperator())) ||
                
                // or if just number without prefix
                (leafCount() == 0 && token().isIntegerLiteral()) 
                ) {
                
                return true;
            }

            return false;
        }
        
        int intValue() const {
            if (!isInteger()) {
                // TODO: throw exception
                assert(false && "Node is not integer.");
            }                        

            int retVal = 0;

            if (leafCount() == 2) {
                retVal = leaf(1).token().intValue();
                if (leaf(0).token().isMinusOperator()) {
                    retVal = -retVal;
                }
            } else {
                retVal = token().intValue();
            }

            return retVal;
        }

        /**
         * Adds newly parsed token to tree under this token tree node.
         *
         * @param token Token to add token tree.         
         */
        void addToTokenTree(Token& token) {
            addToTokenTree(this, new TokenTreeNode(token));
        }
        
        /**
         * Returns referred token of token tree node.
         *
         * @returns Referred token of token tree node.
         */
        const Token& token() const {
            return *data_;
        }
        
        /**
         * Returns number of leafs that this token tree node has.
         *
         * @return Number of leafs that this token tree node has.
         */
        int leafCount() const {
            return leafs_.size();
        }
        
        /**
         * Returns leaf of requested index.
         *
         * @param index Index of leaf to return.
         * @return leaf of requested index.
         */
        TokenTreeNode& leaf(int index) const {
            assert(index < leafCount());
            return *leafs_[index];
        }
        
        
        /**
         * Returns node and its sub nodes as a string.
         *
         * @return Node and its sub nodes as a string.
         */
        std::string toStr() const {
            static int recursioLevel = 0;
            
            recursioLevel++;
            std::stringstream retVal;            
            
            if (data_ == NULL) {
                retVal << "Root node" << std::endl;
            } else {
                retVal << setw(5) << data_->start_ << ":" 
                       << setw(5) << std::left << data_->end_ << std::right 
                       << setw(30) << TokenizerData::idString(data_->type_) 
                       << " ";
                
                for (int fill = 0; fill < (recursioLevel-2)*2; fill++) {
                    retVal << "*";
                } 
                
                retVal << " " << data_->strValue_ << std::endl;
            }
            
            for (int i = 0; i < leafCount(); i++) { 
                retVal << " " << leaf(i).toStr();
            }
            
            recursioLevel--;
            return retVal.str();
        }
        
    private:
        
        /**
         * Adds token tree node to under other token tree node.
         *
         * @param currNode Node where to add new node.
         * @param newToken Token tree node to add to currNode.
         */        
        static void addToTokenTree(
            TokenTreeNode* currNode, TokenTreeNode* newToken) {
            
            // check if token should be part of one of the leafs
            for (std::vector<TokenTreeNode*>::iterator i = 
                     currNode->leafs_.begin() ;    
                 i != currNode->leafs_.end();i++) {
                TokenTreeNode *currLeaf = *i;
                
                // if this token should be part of current leaf
                if (newToken->data_->start_ >= currLeaf->data_->start_ &&
                    newToken->data_->end_ <= currLeaf->data_->end_) {
                    
                    addToTokenTree(currLeaf, newToken);
                    return;
                }
                
                // move current leafs to inside this token
                // if necessary
                if (newToken->data_->start_ <= currLeaf->data_->start_ && 
                    newToken->data_->end_ >= currLeaf->data_->end_) {
                    
                    // remove from original 
                    std::vector<TokenTreeNode*>::iterator temp = i;
                    i--;
                    currNode->leafs_.erase(temp);
                    
                    // add to new...
                    newToken->leafs_.push_back(currLeaf);
                }
                
                // if start of current leaf is after end of this.. 
                // add token to this point.
                if (newToken->data_->end_ <= currLeaf->data_->start_) {
                    currNode->leafs_.insert(i, newToken);
                    return;
                }                
            }       
            
            // if there wasn't any sub nodes where value could go, add it to
            // end of current node 
            currNode->leafs_.push_back(newToken);
            return;        
        }
        
        /// Token of token tree node.
        Token* data_;
        
        /// Leafs of this node.
        std::vector<TokenTreeNode*> leafs_;
    };
    
    TokenizerData() {}
    
    /**
     * Deletes all the parsed data.
     */
    ~TokenizerData() {
        while (!parsedTokens_.empty()) {
            delete (parsedTokens_.begin()->second);
            parsedTokens_.erase(parsedTokens_.begin());
        }
        // TODO: delete TokenTreeNodes
    }

    
    /**
     * Creates new token based on parsed data or updates new type info
     * for old token.
     *
     * @param start Start position of token.
     * @param end End position of token.
     * @param id Type of token.
     * @param strVal String of token.
     */
    void addToken(const char* start, const char* end, 
                  OperationID id, std::string strVal) {
        
        unsigned long startPos, endPos;
        
        if (parsedTokens_.empty()) {
            startPos = 0;
            endPos = end - start;
        } else {
            // in parsedTokens_.begin()->first.second is actually stored value
            // (-start) of first parsed token. So we can add it to current 
            // start address for getting relative end and start positions.
            startPos = 
                reinterpret_cast<unsigned long>(start) + 
                reinterpret_cast<unsigned long>(
                    parsedTokens_.begin()->first.second);
            
            endPos = 
                reinterpret_cast<unsigned long>(end) + 
                reinterpret_cast<unsigned long>(
                    parsedTokens_.begin()->first.second);
        }
               
        // key for map is selected (end, -start) for automatically organizase
        // tokens
        std::pair<const char*, const char*> 
            tokenKey(end, (const char*)(-reinterpret_cast<unsigned long>(start)));
        
        if (parsedTokens_.find(tokenKey) == parsedTokens_.end()) {
            parsedTokens_[tokenKey] = new Token(startPos, endPos, strVal);
            root.addToTokenTree(*parsedTokens_[tokenKey]);
        }
        
        parsedTokens_[tokenKey]->setProperty(id);
    }
    
    /**
     * Shows source code parsed to tokens.
     */
    void showTokenizedSource() {
        
        for (TokenContainerType::iterator i = parsedTokens_.begin();
             i != parsedTokens_.end(); i++) {
            
            std::cerr << std::hex 
                      << std::setw(4) << i->second->start_
                      << " - " 
                      << std::setw(4) << i->second->end_ 
                
                      << " : "                                 
                      << std::setw(30) << idString(i->second->type_) 
                
                      << " : " 
                      << i->second->strValue_
                      << std::endl;
        }
    }
    
    /**
     * Returns root node of token tree.
     *
     * @return Root node of token tree.
     */
    const TokenTreeNode* tokenTree() const {
        return &root;
    }
    
private:
    typedef std::map<
        std::pair<const char*, const char*>, Token*> TokenContainerType;
    
    TokenContainerType parsedTokens_;
    TokenTreeNode root;    
};


/**
 * Spirit actor for adding data to TokenizerData during parsing.
 */
struct TokenizerActor {
    
    TokenizerActor(TokenizerData& data, TokenizerData::OperationID id) :
        data_(data), id_(id) { }    
    
    TokenizerData& data_;
    TokenizerData::OperationID id_;
    
    void operator()(const char* start, const char* end) const { 
        std::string temp(start,end);
        data_.addToken(start, end, id_, temp);

        //	std::cerr << "found: " << setw(10) << TokenizerData::idString(id_)
        //		  << " " << temp << std::endl;
    }
};

/**
 * Spirit actor for book keeping stripped strings.
 */
struct SetStripPairActor {
  
  SetStripPairActor(std::pair<const char*, const char*> &aPair) :
    thePair_(aPair) {}
  
  std::pair<const char*, const char*> &thePair_;
  
  void operator()(const char* start, const char* end) const {    
    thePair_ = std::pair<const char*, const char*>(start, end);
  }
};

/////////////////////////////////////////////////////////////////////////////
// parsing helper function

//  Here's our comment rule
struct skip_grammar : public grammar<skip_grammar>
{    
   template <typename ScannerT>
    struct definition
   {
     definition(skip_grammar const& self)
     {
       skip =   
 	 (str_p("//") >> *(anychar_p - '\n') >> ch_p('\n'))
	 [SetStripPairActor(self.lastStrip)]
 	 [push_back_a(self.strippedParts, self.lastStrip)]
	 
 	 |   
 	 ("/*" >> *(anychar_p - "*/") >> "*/")
	 [SetStripPairActor(self.lastStrip)]
 	 [push_back_a(self.strippedParts, self.lastStrip)]
 	 
	 | 
	 (+space_p)
	 [SetStripPairActor(self.lastStrip)]
 	 [push_back_a(self.strippedParts, self.lastStrip)]
	 ;
       
     }
     
     rule<ScannerT> skip;
     
     rule<ScannerT> const&
     start() const { return skip; }          
   };

public:
  mutable std::vector<std::pair<const char*, const char*> > strippedParts;
  mutable std::pair<const char*, const char*> lastStrip;
};

struct OperationDAGLanguageGrammar : 
    public grammar<OperationDAGLanguageGrammar>
{
    template <typename ScannerT>
    struct definition {
        definition(OperationDAGLanguageGrammar const& self) :
            ELLIPSIS("..."), RIGHT_ASSIGN(">>="), LEFT_ASSIGN("<<="),
            ADD_ASSIGN("+="), SUB_ASSIGN("-="), MUL_ASSIGN("*="),
            DIV_ASSIGN("/="), MOD_ASSIGN("%="), AND_ASSIGN("&="),
            XOR_ASSIGN("^="), OR_ASSIGN("|="), RIGHT_OP(">>"), LEFT_OP("<<"),
            INC_OP("++"), DEC_OP("--"), PTR_OP("->"), AND_OP("&&"),
            OR_OP("||"), LE_OP("<="), GE_OP(">="), EQ_OP("=="), NE_OP("!="),
//             SEMICOLON(';'),
//             COMMA(','), COLON(':'), ASSIGN('='), LEFT_PAREN('('),
//             RIGHT_PAREN(')'), DOT('.'), ADDROF('&'), BANG('!'), TILDE('~'),
//             MINUS('-'), PLUS('+'), STAR('*'), SLASH('/'), PERCENT('%'),
//             LT_OP('<'), GT_OP('>'), XOR('^'), OR('|'), QUEST('?') {
            SEMICOLON(";"),
            COMMA(","), COLON(":"), ASSIGN("="), LEFT_PAREN("("),
            RIGHT_PAREN(")"), DOT("."), ADDROF("&"), BANG("!"), TILDE("~"),
            MINUS("-"), PLUS("+"), STAR("*"), SLASH("/"), PERCENT("%"),
            LT_OP("<"), GT_OP(">"), XOR("^"), OR("|"), QUEST("?") {

            // C keywords
            keywords =
                "auto", "break", "case", "char", "const", "continue", 
                "default", "do", "double", "else", "enum", "extern", 
                "float", "for", "goto", "if", "int", "long", "register", 
                "return", "short", "signed", "sizeof", "static", "struct", 
                "switch", "typedef", "union", "unsigned", "void", 
                "volatile", "while";

            // C operators
//             LEFT_BRACE = chlit<>('{') | strlit<>("<%");
//             RIGHT_BRACE = chlit<>('}') | strlit<>("%>");
//             LEFT_BRACKET = chlit<>('[') | strlit<>("<:");
//             RIGHT_BRACKET = chlit<>(']') | strlit<>(":>");
            LEFT_BRACE = strlit<>("{") | strlit<>("<%");
            RIGHT_BRACE = strlit<>("}") | strlit<>("%>");
            LEFT_BRACKET = strlit<>("[") | strlit<>("<:");
            RIGHT_BRACKET = strlit<>("]") | strlit<>(":>");
            
            // Tokens
            AUTO =       strlit<>("auto");
            BREAK =      strlit<>("break");
            CASE =       strlit<>("case");
            CHAR =       strlit<>("char");
            CONST =      strlit<>("const");
            CONTINUE =   strlit<>("continue");
            DEFAULT =    strlit<>("default");
            DO =         strlit<>("do");
            DOUBLE =     strlit<>("double");
            ELSE =       strlit<>("else");
            ENUM =       strlit<>("enum");
            EXTERN =     strlit<>("extern");
            FOR =        strlit<>("for");
            FLOAT =      strlit<>("float");
            GOTO =       strlit<>("goto");
            IF =         strlit<>("if");
            INT =        strlit<>("int");
            LONG =       strlit<>("long");
            REGISTER =   strlit<>("register");
            RETURN =     strlit<>("return");
            SHORT =      strlit<>("short");
            SIGNED =     strlit<>("signed");
            SIZEOF =     strlit<>("sizeof");
            STATIC =     strlit<>("static");
            STRUCT =     strlit<>("struct");
            SWITCH =     strlit<>("switch");
            TYPEDEF =    strlit<>("typedef");
            UNION =      strlit<>("union");
            UNSIGNED =   strlit<>("unsigned");
            VOID =       strlit<>("void");
            VOLATILE =   strlit<>("volatile");
            WHILE =      strlit<>("while");

	    // C identifiers
	    IDENTIFIER =
	      (lexeme_d[
                    ((alpha_p | '_' | '$') >> *(alnum_p | '_' | '$'))
                    - (keywords >> (anychar_p - (alnum_p | '_' | '$')))
                    ])
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::IDENTIFIER)]

                ;
            
            // string literals
            STRING_LITERAL_PART =
                lexeme_d[
                    !chlit<>('L') >> chlit<>('\"') >>
                    *( strlit<>("\\\"") | ( anychar_p - chlit<>('\"') )) >>
                    chlit<>('\"')
                    ]
                ;
            
            STRING_LITERAL = 
                (+STRING_LITERAL_PART)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STRING_LITERAL)]
                ;
            
            // integer constants
            INT_CONSTANT_HEX =
                lexeme_d[
                    chlit<>('0') >> 
                    as_lower_d[chlit<>('x')] >> 
                    +xdigit_p >> 
                    !as_lower_d[chlit<>('l') | chlit<>('u')]
                    ]
                ;
            
            INT_CONSTANT_OCT
                = lexeme_d[
                    chlit<>('0')
                    >> +range<>('0', '7')
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                    ]
                ;
            
            INT_CONSTANT_DEC
                = lexeme_d[
                    +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                    ]
                ;

            INT_CONSTANT_CHAR
                = lexeme_d[
                    !chlit<>('L') >> chlit<>('\'') >>
                    longest_d[
                        anychar_p
                        |   (   chlit<>('\\')
                                >> chlit<>('0')
                                >> repeat_p(0, 2)[range<>('0', '7')]
                            )
                        |   (chlit<>('\\') >> anychar_p)
                        ] >>
                    chlit<>('\'')
                    ]
                ;

            INT_CONSTANT =
                (INT_CONSTANT_HEX
                 | INT_CONSTANT_OCT
                 | INT_CONSTANT_DEC
                 | INT_CONSTANT_CHAR)
                [TokenizerActor(self.tokenData_, 
                                TokenizerData::INTEGER_LITERAL)]
                ;
            
            // float constants
            FLOAT_CONSTANT_1    // 12345[eE][+-]123[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> (chlit<>('e') | chlit<>('E'))
                    >> !(chlit<>('+') | chlit<>('-'))
                    >> +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                    ]
                ;
            
            FLOAT_CONSTANT_2    // .123([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    *digit_p
                    >> chlit<>('.')
                    >> +digit_p
                    >> !((chlit<>('e') | chlit<>('E'))
                         >> !(chlit<>('+') | chlit<>('-'))
                         >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                    ]
                ;
            
            FLOAT_CONSTANT_3    // 12345.([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> chlit<>('.')
                    >> *digit_p
                    >> !((chlit<>('e') | chlit<>('E'))
                         >> !(chlit<>('+') | chlit<>('-'))
                         >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                    ]
                ;
            
            FLOAT_CONSTANT =
                (FLOAT_CONSTANT_1
                 |   FLOAT_CONSTANT_2
                 |   FLOAT_CONSTANT_3)
                [TokenizerActor(self.tokenData_, 
                                TokenizerData::FLOAT_LITERAL)]
                ;
            
            CONSTANT = 
                (longest_d[FLOAT_CONSTANT | INT_CONSTANT])
                ;
            
            // debug support for terminals
            BOOST_SPIRIT_DEBUG_RULE(AUTO);
            BOOST_SPIRIT_DEBUG_RULE(BREAK);
            BOOST_SPIRIT_DEBUG_RULE(CASE);
            BOOST_SPIRIT_DEBUG_RULE(CHAR);
            BOOST_SPIRIT_DEBUG_RULE(CONST);
            BOOST_SPIRIT_DEBUG_RULE(CONTINUE);
            BOOST_SPIRIT_DEBUG_RULE(DEFAULT);
            BOOST_SPIRIT_DEBUG_RULE(DO);
            BOOST_SPIRIT_DEBUG_RULE(DOUBLE);
            BOOST_SPIRIT_DEBUG_RULE(ELSE);
            BOOST_SPIRIT_DEBUG_RULE(ENUM);
            BOOST_SPIRIT_DEBUG_RULE(EXTERN);
            BOOST_SPIRIT_DEBUG_RULE(FOR);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT);
            BOOST_SPIRIT_DEBUG_RULE(GOTO);
            BOOST_SPIRIT_DEBUG_RULE(IF);
            BOOST_SPIRIT_DEBUG_RULE(INT);
            BOOST_SPIRIT_DEBUG_RULE(LONG);
            BOOST_SPIRIT_DEBUG_RULE(REGISTER);
            BOOST_SPIRIT_DEBUG_RULE(RETURN);
            BOOST_SPIRIT_DEBUG_RULE(SHORT);
            BOOST_SPIRIT_DEBUG_RULE(SIGNED);
            BOOST_SPIRIT_DEBUG_RULE(SIZEOF);
            BOOST_SPIRIT_DEBUG_RULE(STATIC);
            BOOST_SPIRIT_DEBUG_RULE(STRUCT);
            BOOST_SPIRIT_DEBUG_RULE(SWITCH);
            BOOST_SPIRIT_DEBUG_RULE(TYPEDEF);
            BOOST_SPIRIT_DEBUG_RULE(UNION);
            BOOST_SPIRIT_DEBUG_RULE(UNSIGNED);
            BOOST_SPIRIT_DEBUG_RULE(VOID);
            BOOST_SPIRIT_DEBUG_RULE(VOLATILE);
            BOOST_SPIRIT_DEBUG_RULE(WHILE);
            BOOST_SPIRIT_DEBUG_RULE(IDENTIFIER);
            BOOST_SPIRIT_DEBUG_RULE(STRING_LITERAL);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_HEX);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_OCT);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_DEC);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_CHAR);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_1);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_2);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_3);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT);
            BOOST_SPIRIT_DEBUG_RULE(CONSTANT);

            // debug support for non terminals
            BOOST_SPIRIT_DEBUG_RULE(primary_expression);
            BOOST_SPIRIT_DEBUG_RULE(postfix_expression);
            BOOST_SPIRIT_DEBUG_RULE(postfix_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(argument_expression_list);
            BOOST_SPIRIT_DEBUG_RULE(unary_expression);
            BOOST_SPIRIT_DEBUG_RULE(unary_operator);
            BOOST_SPIRIT_DEBUG_RULE(cast_expression);
            BOOST_SPIRIT_DEBUG_RULE(multiplicative_expression);
            BOOST_SPIRIT_DEBUG_RULE(multiplicative_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(additive_expression);
            BOOST_SPIRIT_DEBUG_RULE(additive_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(shift_expression);
            BOOST_SPIRIT_DEBUG_RULE(shift_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(relational_expression);
            BOOST_SPIRIT_DEBUG_RULE(relational_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(equality_expression);
            BOOST_SPIRIT_DEBUG_RULE(equality_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(and_expression);
            BOOST_SPIRIT_DEBUG_RULE(and_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(exclusive_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(exclusive_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(inclusive_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(inclusive_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(logical_and_expression);
            BOOST_SPIRIT_DEBUG_RULE(logical_and_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(logical_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(logical_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(conditional_expression);
            BOOST_SPIRIT_DEBUG_RULE(conditional_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(assignment_expression);
            BOOST_SPIRIT_DEBUG_RULE(assignment_operator);
            BOOST_SPIRIT_DEBUG_RULE(expression);
            BOOST_SPIRIT_DEBUG_RULE(constant_expression);
            BOOST_SPIRIT_DEBUG_RULE(declaration);
            BOOST_SPIRIT_DEBUG_RULE(declaration_specifiers);
            BOOST_SPIRIT_DEBUG_RULE(init_declarator_list);
            BOOST_SPIRIT_DEBUG_RULE(init_declarator);
            BOOST_SPIRIT_DEBUG_RULE(storage_class_specifier);
            BOOST_SPIRIT_DEBUG_RULE(type_specifier);
            BOOST_SPIRIT_DEBUG_RULE(struct_or_union_specifier);
            BOOST_SPIRIT_DEBUG_RULE(struct_or_union);
            BOOST_SPIRIT_DEBUG_RULE(struct_declaration_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declaration);
            BOOST_SPIRIT_DEBUG_RULE(specifier_qualifier_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declarator_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declarator);
            BOOST_SPIRIT_DEBUG_RULE(enum_specifier);
            BOOST_SPIRIT_DEBUG_RULE(enumerator_list);
            BOOST_SPIRIT_DEBUG_RULE(enumerator);
            BOOST_SPIRIT_DEBUG_RULE(type_qualifier);
            BOOST_SPIRIT_DEBUG_RULE(declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_declarator_helper);
            BOOST_SPIRIT_DEBUG_RULE(pointer);
            BOOST_SPIRIT_DEBUG_RULE(type_qualifier_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_type_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_declaration);
            BOOST_SPIRIT_DEBUG_RULE(identifier_list);
            BOOST_SPIRIT_DEBUG_RULE(type_name);
            BOOST_SPIRIT_DEBUG_RULE(abstract_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_abstract_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_abstract_declarator_helper);
            BOOST_SPIRIT_DEBUG_RULE(initializer);
            BOOST_SPIRIT_DEBUG_RULE(initializer_list);
            BOOST_SPIRIT_DEBUG_RULE(statement);
            BOOST_SPIRIT_DEBUG_RULE(labeled_statement);
            BOOST_SPIRIT_DEBUG_RULE(compound_statement);
            BOOST_SPIRIT_DEBUG_RULE(declaration_list);
            BOOST_SPIRIT_DEBUG_RULE(statement_list);
            BOOST_SPIRIT_DEBUG_RULE(expression_statement);
            BOOST_SPIRIT_DEBUG_RULE(selection_statement);
            BOOST_SPIRIT_DEBUG_RULE(iteration_statement);
            BOOST_SPIRIT_DEBUG_RULE(jump_statement);
            BOOST_SPIRIT_DEBUG_RULE(translation_unit);
            BOOST_SPIRIT_DEBUG_RULE(external_declaration);
            BOOST_SPIRIT_DEBUG_RULE(function_definition);

            // Rules
            primary_expression = 
                (IDENTIFIER
                 | CONSTANT
                 | STRING_LITERAL
                 | LEFT_PAREN >> expression >> RIGHT_PAREN)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::PRIMARY_EXPRESSION)]
                ;
            
            // left recursion --> right recursion
            //    postfix_expression
            //        = primary_expression
            //        | postfix_expression >>
            //            (
            //                LEFT_BRACKET >> expression >> RIGHT_BRACKET
            //            |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
            //            |   DOT >> IDENTIFIER
            //            |   PTR_OP >> IDENTIFIER
            //            |   INC_OP
            //            |   DEC_OP
            //            )
            //        ;
            postfix_expression = 
                (primary_expression >> postfix_expression_helper)
                ;
            
            postfix_expression_helper =   
                ((LEFT_BRACKET >> expression >> RIGHT_BRACKET
                  |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
                  |   DOT >> IDENTIFIER
                  |   PTR_OP >> IDENTIFIER
                  |   INC_OP
                  |   DEC_OP) >>
                 postfix_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::POSTFIX_EXPRESSION)]
                | epsilon_p
                ;

            argument_expression_list = 
                (assignment_expression >> *(COMMA >> assignment_expression));

            unary_expression = 
                (postfix_expression
                 | INC_OP >> unary_expression
                 | DEC_OP >> unary_expression
                 | unary_operator >> cast_expression
                 | SIZEOF >>
                 
                 (unary_expression
                  | LEFT_PAREN >> type_name >> RIGHT_PAREN))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_EXPRESSION)]
                ;
            
            unary_operator = 
                ADDROF
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_ADDROF)]
                
                | STAR
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_STAR)]
                
                | PLUS
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_PLUS)]
                
                | MINUS
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_MINUS)]
                
                | TILDE
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_TILDE)]
                
                | BANG
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::UNARY_BANG)]
                
                ;

            cast_expression = 
                (LEFT_PAREN >> type_name >> RIGHT_PAREN >> 
                 cast_expression
                 | unary_expression)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::CAST_EXPRESSION)]
                
                ;

            // left recursion --> right recursion
            //    multiplicative_expression
            //        = cast_expression
            //        | multiplicative_expression >>
            //        (
            //                STAR >> cast_expression
            //            |   SLASH >> cast_expression
            //            |   PERCENT >> cast_expression
            //        )
            //        ;
            multiplicative_expression = 
                (cast_expression >> multiplicative_expression_helper)
                ;

            multiplicative_expression_helper =
                ((STAR >> cast_expression
                  |   SLASH >> cast_expression
                  |   PERCENT >> cast_expression) >>
                 multiplicative_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::MULTIPLICATIVE_EXPRESSION)]
                | epsilon_p 
                ;

            // left recursion --> right recursion
            //    additive_expression
            //        = multiplicative_expression
            //        | additive_expression >>
            //        (
            //                PLUS >> multiplicative_expression
            //            |   MINUS >> multiplicative_expression
            //        )
            //        ;
            additive_expression = 
                (multiplicative_expression >> additive_expression_helper)
                ;

            additive_expression_helper =
                ((PLUS >> multiplicative_expression
                  | MINUS >> multiplicative_expression) >>
                 additive_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ADDITIVE_EXPRESSION)]
                
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    shift_expression
            //        = additive_expression
            //        | shift_expression >>
            //        (
            //                LEFT_OP >> additive_expression
            //            |   RIGHT_OP >> additive_expression
            //        )
            //        ;
            shift_expression = 
                (additive_expression >> shift_expression_helper)
                ;

            shift_expression_helper =
                ((LEFT_OP >> additive_expression
                  |   RIGHT_OP >> additive_expression) >>
                 shift_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::SHIFT_EXPRESSION)]               
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    relational_expression
            //        = shift_expression
            //        | relational_expression >>
            //        (
            //                LT_OP >> shift_expression
            //            |   GT_OP >> shift_expression
            //            |   LE_OP >> shift_expression
            //            |   GE_OP >> shift_expression
            //        )
            //        ;
            relational_expression = 
                (shift_expression >> relational_expression_helper)
                ;

            relational_expression_helper =
                ((LT_OP >> shift_expression
                  |   GT_OP >> shift_expression
                  |   LE_OP >> shift_expression
                  |   GE_OP >> shift_expression) >>
                 relational_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::RELATIONAL_EXPRESSION)]                
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    equality_expression
            //        = relational_expression
            //        | equality_expression >>
            //        (
            //                EQ_OP >> relational_expression
            //            |   NE_OP >> relational_expression
            //        )
            //        ;
            equality_expression = 
                (relational_expression >> equality_expression_helper)
                ;

            equality_expression_helper =
                ((EQ_OP >> relational_expression
                  |   NE_OP >> relational_expression) >>
                 equality_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::EQUALITY_EXPRESSION)]                
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    and_expression
            //        = equality_expression
            //        | and_expression >> ADDROF >> equality_expression
            //        ;
            and_expression = 
                (equality_expression >> and_expression_helper)
                ;

            and_expression_helper = 
                (ADDROF >> equality_expression >> 
                 and_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::AND_EXPRESSION)]
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    exclusive_or_expression
            //        = and_expression
            //        | exclusive_or_expression >> XOR >> and_expression
            //        ;
            exclusive_or_expression = 
                (and_expression >> exclusive_or_expression_helper)
                ;

            exclusive_or_expression_helper = 
                (XOR >> and_expression >> 
                 exclusive_or_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::EXCLUSIVE_EXPRESSION)]
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    inclusive_or_expression
            //        = exclusive_or_expression
            //        | inclusive_or_expression >> OR >> exclusive_or_expression
            //        ;
            inclusive_or_expression = 
                (exclusive_or_expression >> inclusive_or_expression_helper)
                ;

            inclusive_or_expression_helper = 
                (OR >> exclusive_or_expression >> 
                 inclusive_or_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::INCLUSIVE_EXPRESSION)]
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    logical_and_expression
            //        = inclusive_or_expression
            //        | logical_and_expression >> AND_OP >> inclusive_or_expression
            //        ;
            logical_and_expression = 
                (inclusive_or_expression >> logical_and_expression_helper)
                ;

            logical_and_expression_helper = 
                (AND_OP >> inclusive_or_expression >> 
                 logical_and_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::LOGICAL_AND_EXPRESSION)]
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    logical_or_expression
            //        = logical_and_expression
            //        | logical_or_expression >> OR_OP >> logical_and_expression
            //        ;
            logical_or_expression = 
                (logical_and_expression >> logical_or_expression_helper)
                ;

            logical_or_expression_helper = 
                (OR_OP >> logical_and_expression >> 
                 logical_or_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::LOGICAL_OR_EXPRESSION)]                
                | epsilon_p
                ;

            // left recursion --> right recursion
            //    conditional_expression
            //        = logical_or_expression
            //        | logical_or_expression >> QUEST >> expression >> COLON >>
            //        conditional_expression
            //        ;
            conditional_expression = 
                (logical_or_expression >> conditional_expression_helper)
                ;

            conditional_expression_helper = 
                (QUEST >> expression >> COLON >> 
                 conditional_expression >> conditional_expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::CONDITIONAL_EXPRESSION)]                
                | epsilon_p
                ;

            //           assignment_expression = 
            //                 (unary_expression >>                 
            //                  assignment_operator >> 
            //                  assignment_expression
            //                  | conditional_expression)
            //                 [TokenizerActor(self.tokenData_, 
            //                           TokenizerData::ASSIGNMENT_EXPRESSION)]
            //                 ;

            assignment_expression = 
                (unary_expression >>                 
                 assignment_operator >> 
                 assignment_expression
                 | conditional_expression)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ASSIGNMENT_EXPRESSION)]
                ;
            
            assignment_operator = 
                (ASSIGN
                 | MUL_ASSIGN
                 | DIV_ASSIGN
                 | MOD_ASSIGN
                 | ADD_ASSIGN
                 | SUB_ASSIGN
                 | LEFT_ASSIGN
                 | RIGHT_ASSIGN
                 | AND_ASSIGN
                 | XOR_ASSIGN
                 | OR_ASSIGN)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ASSIGNMENT_OPERATOR)]
                ;
            
            // left recursion --> right recursion
            //    expression
            //        = assignment_expression
            //        | expression >> COMMA >> assignment_expression
            //        ;
            expression = 
                (assignment_expression >> expression_helper)
                ;

            expression_helper = 
                (COMMA >> assignment_expression >> expression_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::EXPRESSION)]
                | epsilon_p
                ;

            constant_expression = 
                conditional_expression
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::CONSTANT_EXPRESSION)]
                ;

            declaration = 
                (declaration_specifiers >> !init_declarator_list >> SEMICOLON)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::DECLARATION)]
                ;

            declaration_specifiers = 
                ((storage_class_specifier
                  |   type_specifier
                  |   type_qualifier) >>
                
                 !declaration_specifiers)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::DECLARATION_SPECIFIERS)]
                ;

            init_declarator_list = 
                init_declarator >> *(COMMA >> init_declarator);

            init_declarator = 
                (declarator >> !(ASSIGN >> initializer))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::INIT_DECLARATOR)]
                ;

            storage_class_specifier = 
                (TYPEDEF | EXTERN | STATIC | AUTO | REGISTER)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STORAGE_CLASS_SPECIFIER)]
                 ;

            //             type_specifier = 
            //                 (VOID
            //                  | CHAR
            //                  | SHORT
            //                  | INT
            //                  | LONG
            //                  | FLOAT
            //                  | DOUBLE
            //                  | SIGNED
            //                  | UNSIGNED
            //                  | struct_or_union_specifier
            //                  | enum_specifier
            //                  //        | TYPE_NAME
            //                     )
            //                 [TokenizerActor(self.tokenData_, 
            //                           TokenizerData::TYPE_SPECIFIER)]
            //                 ;

            type_specifier = 
                (str_p("IntWord")
                 | str_p("FloatWord")
                 | str_p("DoubleWord")
                 | str_p("SimValue")
                 | struct_or_union_specifier
                 | enum_specifier)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::TYPE_SPECIFIER)]
                ;

            struct_or_union_specifier = 
                (struct_or_union >>
                 (IDENTIFIER >> 
                  !(LEFT_BRACE >> struct_declaration_list >> 
                    RIGHT_BRACE)
                  |LEFT_BRACE >> struct_declaration_list >> RIGHT_BRACE))
                 [TokenizerActor(self.tokenData_, 
                           TokenizerData::STRUCT_OR_UNION_SPECIFIER)]
                 ;

            struct_or_union = 
                 (STRUCT | UNION)
                 [TokenizerActor(self.tokenData_, 
                           TokenizerData::STRUCT_OR_UNION)]
                 ;

            struct_declaration_list = 
                +struct_declaration;

            struct_declaration = 
                (specifier_qualifier_list >> struct_declarator_list >> 
                 SEMICOLON)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STRUCT_DECLARATION)];

            specifier_qualifier_list =
                ((type_specifier | type_qualifier) >>
                 !specifier_qualifier_list) ;

            struct_declarator_list = 
                struct_declarator >> *(COMMA >> struct_declarator)
                 ;

            struct_declarator = 
                (declarator || (COLON >> constant_expression))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STRUCT_DECLARATOR)]
                ;

            enum_specifier = 
                 (ENUM >> !IDENTIFIER >> LEFT_BRACE >> 
                  enumerator_list >> 
                  RIGHT_BRACE)
                 [TokenizerActor(self.tokenData_, 
                           TokenizerData::ENUM_SPECIFIER)]
                 ;

            enumerator_list = 
                enumerator >> *(COMMA >> enumerator)
                ;

            enumerator = 
                (IDENTIFIER >> !(ASSIGN >> constant_expression))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ENUMERATOR)]
                ;

            type_qualifier = 
                (CONST
                 [TokenizerActor(self.tokenData_, 
                           TokenizerData::CONST_QUALIFER)]
                 
                 | VOLATILE
                 [TokenizerActor(self.tokenData_, 
                           TokenizerData::VOLATILE_QUALIFER)])
                
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::TYPE_QUALIFER)]
                ;
            
            declarator = 
                (!pointer >> direct_declarator)
                [TokenizerActor(self.tokenData_, TokenizerData::DECLARATOR)]
                ;
            
            // left recursion --> right recursion
            //    direct_declarator
            //        = IDENTIFIER
            //        | LEFT_PAREN >> declarator >> RIGHT_PAREN
            //        | direct_declarator >>
            //            (
            //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
            //            |   LEFT_PAREN >>
            //                (
            //                    parameter_type_list >> RIGHT_PAREN
            //                |   identifier_list >> RIGHT_PAREN
            //                |   RIGHT_PAREN
            //                )
            //            )
            //        ;
            direct_declarator =
                ((IDENTIFIER
                  | LEFT_PAREN >> declarator >> RIGHT_PAREN) >>
                 direct_declarator_helper)
                ;
            
            direct_declarator_helper =
                ((LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                  |   LEFT_PAREN >>
                  !   (parameter_type_list
                       |   identifier_list) >> 
                  RIGHT_PAREN) >> 
                 direct_declarator_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::DIRECT_DECLARATOR)]                
                | epsilon_p
                ;
            
            pointer = 
                (STAR >> !(type_qualifier_list || pointer))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::POINTER)]
                ;
            
            type_qualifier_list =
                +type_qualifier
                ;
            
            parameter_type_list = 
                (parameter_list >> !(COMMA >> ELLIPSIS))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::PARAMETER_TYPE_LIST)]
                ;
            
            parameter_list = 
                (parameter_declaration >> *(COMMA >> parameter_declaration))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::PARAMETER_LIST)]
                ;
            
            parameter_declaration = 
                (declaration_specifiers >>                
                 !(declarator | abstract_declarator))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::PARAMETER_DECLARATION)]
                ;
            
            identifier_list = 
                (IDENTIFIER >> *(COMMA >> IDENTIFIER))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::IDENTIFIER_LIST)]
                ;
            
            type_name = 
                (specifier_qualifier_list >> !abstract_declarator)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::TYPE_NAME)]
                ;

            abstract_declarator = 
                (pointer || direct_abstract_declarator)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ABSTRACT_DECLARATOR)]
                ;

            // left recursion --> right recursion
            //    direct_abstract_declarator
            //        = LEFT_PAREN >>
            //            (
            //                abstract_declarator >> RIGHT_PAREN
            //            |   !parameter_type_list >> RIGHT_PAREN
            //            )
            //        | LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
            //        | direct_abstract_declarator >>
            //            (
            //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
            //            |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
            //            )
            //        ;
            direct_abstract_declarator =   
                ((LEFT_PAREN >>
                  
                  (abstract_declarator >> RIGHT_PAREN
                   | !parameter_type_list >> RIGHT_PAREN)
                  
                  | LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET) >>
                 
                 direct_abstract_declarator_helper)
                ;
            
            direct_abstract_declarator_helper =   
                ((LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                  |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN) >>
                 direct_abstract_declarator_helper)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::DIRECT_ABSTRACT_DECLARATOR)]
                | epsilon_p
                ;
            
            initializer = 
                (assignment_expression
                 | LEFT_BRACE >> initializer_list >> !COMMA >> RIGHT_BRACE)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::INITIALIZER)]
                ;
            
            initializer_list = 
                (initializer >> *(COMMA >> initializer))
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::INITIALIZER_LIST)]
                ;

            statement = 
                (labeled_statement
                 | compound_statement
                 | expression_statement
                 | selection_statement
                 | iteration_statement
                 | jump_statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STATEMENT)]

                ;

            labeled_statement = 
                (IDENTIFIER >> COLON >> statement
                 | CASE >> constant_expression >> COLON >> statement
                 | DEFAULT >> COLON >> statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::LABELED_STATEMENT)]
                ;

            compound_statement = 
                (LEFT_BRACE >> 
                 !(declaration_list || statement_list) >> 
                 RIGHT_BRACE)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::COMPOUND_STATEMENT)]
                ;

            declaration_list = (+declaration)
                ;

            statement_list = 
                (+statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::STATEMENT_LIST)]
                ;

            expression_statement = 
                (!expression >> SEMICOLON)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::EXPRESSION_STATEMENT)]
                ;

            selection_statement = 
                (IF >> LEFT_PAREN >> expression >> RIGHT_PAREN >> 
                 statement >> !(ELSE >> statement)
                 
                 | SWITCH >> LEFT_PAREN >> expression >> RIGHT_PAREN >> 
                 statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::SELECTION_STATEMENT)]                
                ;
            
            iteration_statement = 
                (WHILE >> 
                 LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                 
                 | DO >> statement >> WHILE >> 
                 LEFT_PAREN >> expression >> 
                 RIGHT_PAREN >> SEMICOLON
                 
                 | FOR >> LEFT_PAREN >> 
                 expression_statement >> 
                 expression_statement >> 
                 !expression >> RIGHT_PAREN >> statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::ITERATION_STATEMENT)]
                ;
            
            jump_statement = 
                (GOTO >> IDENTIFIER >> SEMICOLON
                 | CONTINUE >> SEMICOLON
                 | BREAK >> SEMICOLON
                 | RETURN >> !expression >> SEMICOLON)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::JUMP_STATEMENT)]

                ;

            function_definition =  
                (!declaration_specifiers >>  
                 declarator >> 
                 !declaration_list >>  
                 compound_statement)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::FUNCTION_DEFINITION)]
                ;

            external_declaration = 
                (function_definition | declaration)
                [TokenizerActor(self.tokenData_, 
                          TokenizerData::EXTERNAL_DECLARATION)]
                ;
	      
            // Original..
            // parser start symbol
            // translation_unit = *external_declaration ;
            //

            // We are only interested about expressions ended with semicolon
	       translation_unit = *(expression_statement|declaration_list)
                ;
        }

        // keywords
        symbols<> keywords;

        // operators
        strlit<>
        ELLIPSIS, RIGHT_ASSIGN, LEFT_ASSIGN, ADD_ASSIGN, SUB_ASSIGN,
                MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, XOR_ASSIGN,
                OR_ASSIGN, RIGHT_OP, LEFT_OP, INC_OP, DEC_OP, PTR_OP, AND_OP,
                OR_OP, LE_OP, GE_OP, EQ_OP, NE_OP;
//        chit<>
        strlit<>
                SEMICOLON, COMMA, COLON, ASSIGN, LEFT_PAREN, RIGHT_PAREN,
                DOT, ADDROF, BANG, TILDE, MINUS, PLUS, STAR, SLASH, PERCENT,
                LT_OP, GT_OP, XOR, OR, QUEST;

        rule<ScannerT>
                LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET;

    // terminals
        rule<ScannerT>
                AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
                ELSE, ENUM, EXTERN, FOR, FLOAT, GOTO, IF, INT, LONG, REGISTER,
                RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH, TYPEDEF,
                UNION, UNSIGNED, VOID, VOLATILE, WHILE, IDENTIFIER,
                STRING_LITERAL_PART, STRING_LITERAL, INT_CONSTANT_HEX, INT_CONSTANT,
                INT_CONSTANT_OCT, INT_CONSTANT_DEC, INT_CONSTANT_CHAR,
                FLOAT_CONSTANT,FLOAT_CONSTANT_1, FLOAT_CONSTANT_2, FLOAT_CONSTANT_3,
                CONSTANT;

    // nonterminals
        rule<ScannerT>
                primary_expression, postfix_expression, postfix_expression_helper,
                argument_expression_list, unary_expression, unary_operator,
                cast_expression,
                multiplicative_expression, multiplicative_expression_helper,
                additive_expression, additive_expression_helper,
                shift_expression, shift_expression_helper,
                relational_expression, relational_expression_helper,
                equality_expression, equality_expression_helper,
                and_expression, and_expression_helper,
                exclusive_or_expression, exclusive_or_expression_helper,
                inclusive_or_expression, inclusive_or_expression_helper,
                logical_and_expression, logical_and_expression_helper,
                logical_or_expression, logical_or_expression_helper,
                conditional_expression, conditional_expression_helper,
                assignment_expression, assignment_operator,
                expression, expression_helper, constant_expression, declaration,
                declaration_specifiers, init_declarator_list, init_declarator,
                storage_class_specifier, type_specifier, struct_or_union_specifier,
                struct_or_union, struct_declaration_list, struct_declaration,
                specifier_qualifier_list, struct_declarator_list,
                struct_declarator, enum_specifier, enumerator_list, enumerator,
                type_qualifier, declarator,
                direct_declarator, direct_declarator_helper, pointer,
                type_qualifier_list, parameter_type_list, parameter_list,
                parameter_declaration, identifier_list, type_name,
                abstract_declarator,
                direct_abstract_declarator, direct_abstract_declarator_helper,
                initializer, initializer_list, statement, labeled_statement,
                compound_statement, declaration_list, statement_list,
                expression_statement, selection_statement, iteration_statement,
                jump_statement, translation_unit, external_declaration,
	//	       function_definition, last_comment_spirit_bugfix;
	  function_definition, last_comment_spirit_bugfix;

        rule<ScannerT> const&
        start() const { return translation_unit; }
    };

public:
    mutable TokenizerData tokenData_;        
};

#endif
