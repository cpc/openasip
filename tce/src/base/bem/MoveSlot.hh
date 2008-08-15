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
 * @file MoveSlot.hh
 *
 * Declaration of MoveSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_SLOT_HH
#define TTA_MOVE_SLOT_HH

#include "InstructionField.hh"

class GuardField;
class SourceField;
class DestinationField;
class ShortImmediateControlField;
class BinaryEncoding;

/**
 * The MoveSlot class represents move slots, fields of the TTA instruction
 * dedicated to program data transports.
 *
 * Move slots are subdivided into 2 or 3 fields: the guard (optional),
 * source and destination fields. Each type of move slot field is
 * modelled by a different class. MoveSlot has handles to the
 * instances of these classes. A move slot is identified by the name
 * of the transport bus it controls.  MoveSlot instances cannot exist
 * alone. They belong to a binary encoding and are always registered
 * to a BinaryEncoding object.
 */
class MoveSlot : public InstructionField {
public:
    MoveSlot(const std::string& busName, BinaryEncoding& parent)
	throw (ObjectAlreadyExists);
    MoveSlot(const ObjectState* state, BinaryEncoding& parent)
	throw (ObjectStateLoadingException);
    virtual ~MoveSlot();

    BinaryEncoding* parent() const;

    std::string name() const;
    void setName(const std::string& name)
	throw (ObjectAlreadyExists);

    void setGuardField(GuardField& field)
	throw (ObjectAlreadyExists);
    void unsetGuardField();
    bool hasGuardField() const;
    GuardField& guardField() const;

    void setSourceField(SourceField& field)
	throw (ObjectAlreadyExists);
    void unsetSourceField();
    bool hasSourceField() const;
    SourceField& sourceField() const;

    void setDestinationField(DestinationField& field)
	throw (ObjectAlreadyExists);
    void unsetDestinationField();
    bool hasDestinationField() const;
    DestinationField& destinationField() const;

    // virtual methods derived from InstructionField
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const
	throw (OutOfRange);
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for move slot.
    static const std::string OSNAME_MOVE_SLOT;
    /// ObjectState attribute key for the name of the bus.
    static const std::string OSKEY_BUS_NAME;

private:
    void deleteGuardField();
    void deleteSourceField();
    void deleteDestinationField();

    /// The bus name.
    std::string name_;
    /// The guard field.
    GuardField* guardField_;
    /// The source field.
    SourceField* sourceField_;
    /// The destination field.
    DestinationField* destinationField_;
};

#endif


