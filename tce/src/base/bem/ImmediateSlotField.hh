/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ImmediateSlotField.hh
 *
 * Declaration of ImmediateSlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
