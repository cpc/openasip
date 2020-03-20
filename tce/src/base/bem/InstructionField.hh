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
 * @file InstructionField.hh
 *
 * Declaration of InstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_FIELD_HH
#define TTA_INSTRUCTION_FIELD_HH

#include "Exception.hh"
#include "Serializable.hh"

/**
 * InstructionField is an abstract base class that represents the properties
 * common to all types of bit fields of the TTA instruction word.
 */
class InstructionField : public Serializable {
public:
    virtual ~InstructionField();

    InstructionField* parent() const;

    /**
     * Returns the number of (immediate) child fields within the instruction
     * field.
     */
    virtual int childFieldCount() const = 0;

    virtual InstructionField& childField(int position) const;

    /**
     * Returns the bit width of the field.
     */
    virtual int width() const = 0;

    int bitPosition() const;
    int relativePosition() const;
    virtual void setRelativePosition(int position);
    void setExtraBits(int bits);
    int extraBits() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    /// ObjectState name for instruction field.
    static const std::string OSNAME_INSTRUCTION_FIELD;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;
    /// ObjectState attribute key for the relative position of the field.
    static const std::string OSKEY_POSITION;

protected:
    InstructionField(InstructionField* parent);
    InstructionField(const ObjectState* state, InstructionField* parent);

    void setParent(InstructionField* parent);
    static void reorderSubfields(ObjectState* state);

private:
    /// Indicates the relative position of the field.
    int relativePos_;
    /// The number of extra bits.
    int extraBits_;
    /// The parent instruction field.
    InstructionField* parent_;
};

#endif
