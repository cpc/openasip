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
 * @file SlotField.hh
 *
 * Declaration of SlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SLOT_FIELD_HH
#define TTA_SLOT_FIELD_HH

#include <vector>
#include "InstructionField.hh"
#include "BinaryEncoding.hh"

class SocketEncoding;
class MoveSlot;
class NOPEncoding;

/**
 * SlotField is an abstract base class that models the properties common to
 * SourceField and DestinationField classes.
 *
 * SlotField contains and manages the encodings for sockets. Encodings
 * for different sources and destinations consists of two parts:
 * socket ID and optional socket code. Socket ID determines the
 * socket. Socket code determines the port (and opcode) the socket is
 * connected to. Socket ID may be on either left or right side of the
 * socket code in the source or destination field. If socket ID is on
 * the left side of socket code, socket ID is aligned to the left end
 * of the whole source/destination field. In the other case, socket ID
 * is aligned to the right end of the source/destination field.
 */
class SlotField : public InstructionField {
public:
    virtual ~SlotField();

    MoveSlot* parent() const;

    void addSocketEncoding(SocketEncoding& encoding)
	throw (ObjectAlreadyExists);
    void removeSocketEncoding(SocketEncoding& encoding);

    int socketEncodingCount() const;
    SocketEncoding& socketEncoding(int index) const
	throw (OutOfRange);
    bool hasSocketEncoding(const std::string& socket) const;
    SocketEncoding& socketEncoding(const std::string& socket) const;

    void setNoOperationEncoding(NOPEncoding& encoding)
        throw (ObjectAlreadyExists);
    void unsetNoOperationEncoding();
    bool hasNoOperationEncoding() const;
    NOPEncoding& noOperationEncoding() const;

    BinaryEncoding::Position componentIDPosition() const;

    // methods inherited from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const
	throw (OutOfRange);

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for slot field.
    static const std::string OSNAME_SLOT_FIELD;
    /// ObjectState attribute key for component ID position.
    static const std::string OSKEY_COMPONENT_ID_POSITION;

protected:
    SlotField(BinaryEncoding::Position componentIDPos, MoveSlot& parent);
    SlotField(const ObjectState* state, MoveSlot& parent)
	throw (ObjectStateLoadingException);

private:
    /// A container type for socket encodings.
    typedef std::vector<SocketEncoding*> SocketEncodingTable;

    void clearSocketEncodings();
    void clearNoOperationEncoding();

    /// The NOP encoding.
    NOPEncoding* nopEncoding_;
    /// The container for socket encodings.
    SocketEncodingTable encodings_;
    /// Position of the socket and bridge IDs within the field.
    BinaryEncoding::Position componentIDPos_;
};

#endif


