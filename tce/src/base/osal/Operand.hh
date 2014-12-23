/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file Operand.hh
 *
 * Declaration of Operand class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#ifndef TTA_OPERAND_HH
#define TTA_OPERAND_HH

#include <set>
#include <string>

#include "Serializable.hh"
#include "Exception.hh"

class ObjectState;

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
        DOUBLE_WORD,
        HALF_FLOAT_WORD,
        BOOL,
        RAW_DATA
    };

    static const std::string SINT_WORD_STRING;
    static const std::string UINT_WORD_STRING;
    static const std::string HALF_FLOAT_WORD_STRING;
    static const std::string FLOAT_WORD_STRING;
    static const std::string DOUBLE_WORD_STRING;
    static const std::string BOOL_STRING;
    static const std::string RAW_DATA_STRING;
    static const std::string UNKNOWN_TYPE_STRING;
    
    /// Object state name for operand id.
    static const std::string OPRND_ID;
    /// Object state name for operand type.
    static const std::string OPRND_TYPE;
    /// Object state name for memory address.
    static const std::string OPRND_MEM_ADDRESS;
    /// Object state name for memory unit count.
    static const std::string OPRND_MEM_UNITS;
    /// Object state name for memory data.
    static const std::string OPRND_MEM_DATA;
    /// Object state name for can swap.
    static const std::string OPRND_CAN_SWAP;
    /// Object state name for input operand.
    static const std::string OPRND_IN;
    /// Object state name for output operand.
    static const std::string OPRND_OUT;
    /// Object state name for element width.
    static const std::string OPRND_ELEM_WIDTH;
    /// Object state name for element count.
    static const std::string OPRND_ELEM_COUNT;

    explicit Operand(bool isInput);
    explicit Operand(bool isInput, int index, OperandType type);
    Operand(const Operand& op);
    virtual ~Operand();

    virtual int index() const;
    virtual bool isInput() const;
    virtual bool isOutput() const;
    virtual OperandType type() const;
    virtual void setType(OperandType type);
    virtual const std::string& typeString() const;
    virtual bool isVector() const;
    virtual int elementWidth() const;
    virtual void setElementWidth(int elementWidth);
    virtual int elementCount() const;
    virtual void setElementCount(int elementCount);
    virtual int width() const;
    virtual bool isAddress() const;
    virtual int memoryUnits() const;
    virtual bool isMemoryData() const;
    virtual bool canSwap(const Operand& op) const;
    virtual const std::set<int>& swap() const;

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    virtual bool isNull() const { return false; }

    static int defaultElementWidth(OperandType type);

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
    /// Width of an element.
    int elementWidth_;
    /// Number of total elements.
    int elementCount_;
    /// Flag indicating whether Operand is address or not.
    bool isAddress_;
    /// size of the data this operation addresses. 0 if unknown.
    int addressUnits_;
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
    virtual bool isNull() const { return true; }

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
