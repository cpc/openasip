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
 * @file ImmediateSlotField.hh
 *
 * Declaration of ImmediateSlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_FIELD_HH
#define TTA_IMMEDIATE_SLOT_FIELD_HH

#include "InstructionField.hh"

class BinaryEncoding;

/**
 * ImmediateSlotField represents a dedicated immediate slot in TTA 
 * instruction.
 */
class ImmediateSlotField : public InstructionField {
public:
    ImmediateSlotField(
        const std::string& name,
        int width,
        BinaryEncoding& parent)
        throw (OutOfRange, ObjectAlreadyExists);
    ImmediateSlotField(const ObjectState* state, BinaryEncoding& parent)
        throw (ObjectStateLoadingException);
    virtual ~ImmediateSlotField();

    BinaryEncoding* parent() const;
    std::string name() const;
    void setName(const std::string& name)
        throw (ObjectAlreadyExists);

    virtual int childFieldCount() const;
    virtual int width() const;
    void setWidth(int width)
        throw (OutOfRange);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for immediate slot field.
    static const std::string OSNAME_IMMEDIATE_SLOT_FIELD;
    /// ObjectState attribute key for the name of the immediate slot.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for the width of the field.
    static const std::string OSKEY_WIDTH;

private:
    /// Name of the immediate slot.
    std::string name_;
    /// The bit width of the field.
    int width_;
};

#endif
