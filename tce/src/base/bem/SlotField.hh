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

    void addSocketEncoding(SocketEncoding& encoding);
    void removeSocketEncoding(SocketEncoding& encoding);

    int socketEncodingCount() const;
    SocketEncoding& socketEncoding(int index) const;
    bool hasSocketEncoding(const std::string& socket) const;
    SocketEncoding& socketEncoding(const std::string& socket) const;

    void setNoOperationEncoding(NOPEncoding& encoding);
    void unsetNoOperationEncoding();
    bool hasNoOperationEncoding() const;
    NOPEncoding& noOperationEncoding() const;

    BinaryEncoding::Position componentIDPosition() const;

    // methods inherited from InstructionField
    virtual int width() const;
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    /// ObjectState name for slot field.
    static const std::string OSNAME_SLOT_FIELD;
    /// ObjectState attribute key for component ID position.
    static const std::string OSKEY_COMPONENT_ID_POSITION;

protected:
    SlotField(BinaryEncoding::Position componentIDPos, MoveSlot& parent);
    SlotField(const ObjectState* state, MoveSlot& parent);

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


