/**
 * @file Operand.hh
 *
 * Declaration of Operand class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#ifndef TTA_OPERAND_HH
#define TTA_OPERAND_HH

#include <set>
#include <string>

#include "Serializable.hh"
#include "Exception.hh"
#include "ObjectState.hh"

/**
 * Class that models Operation Operand.
 *
 * Contains the static properties of Operation inputs and outputs.
 */
class Operand : public Serializable {
public:

    /**
     * Type of operand.
     */
    enum OperandType {
        SINT_WORD,
        UINT_WORD,
        FLOAT_WORD,
        DOUBLE_WORD
    };

    static const std::string SINT_WORD_STRING;
    static const std::string UINT_WORD_STRING;
    static const std::string FLOAT_WORD_STRING;
    static const std::string DOUBLE_WORD_STRING;
    static const std::string UNKNOWN_TYPE_STRING;
    
    /// Object state name for operand id.
    static const std::string OPRND_ID;
    /// Object state name for operand type.
    static const std::string OPRND_TYPE;
    /// Object state name for memory address.
    static const std::string OPRND_MEM_ADDRESS;
    /// Object state name for memory data.
    static const std::string OPRND_MEM_DATA;
    /// Object state name for can swap.
    static const std::string OPRND_CAN_SWAP;
    /// Object state name for input operand.
    static const std::string OPRND_IN;
    /// Object state name for output operand.
    static const std::string OPRND_OUT;

    explicit Operand(bool isInput);
    explicit Operand(bool isInput, int index, OperandType type);
    Operand(const Operand& op);
    virtual ~Operand();

    virtual int index() const;
    virtual bool isInput() const;
    virtual bool isOutput() const;
    virtual OperandType type() const;
    virtual const std::string& typeString() const;
    virtual bool isAddress() const;
    virtual bool isMemoryData() const;
    virtual bool canSwap(const Operand& op) const;
    virtual const std::set<int>& swap() const;

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

private:
    void clear();

    /// Assignment not allowed.
    Operand& operator=(const Operand&);

    /// Index of the Operand.
    int index_;
    /// Direction of operand.
    bool isInput_;
    /// Type of the Operand.
    OperandType type_;
    /// Flag indicating whether Operand is address or not.
    bool isAddress_;
    /// Flag indicating whether Operand is memory data or not.
    bool isMemoryData_;
    /// Indexes of Operands which can be swapped with this Operand.
    std::set<int> swap_;
};

//////////////////////////////////////////////////////////////////////////////
// NullOperand
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null operand.
 *
 * All methods cause program abort with an error log message.
 *
 */
class NullOperand : public Operand {
public:
    virtual ~NullOperand();

    static NullOperand& instance();

    virtual int index() const;
    virtual bool isInput() const;
    virtual bool isOutput() const;
    virtual bool isAddress() const;
    virtual bool isMemoryData() const;
    virtual bool canSwap(const Operand& op) const;
    virtual const std::set<int>& swap() const;

private:
    NullOperand();
    /// Assignment not allowed.
    NullOperand& operator=(const NullOperand&);

    /// Unique instance.
    static NullOperand* instance_;

    /// Needed for one method, always empty.
    std::set<int> swap_;
};

#include "Operand.icc"

#endif
