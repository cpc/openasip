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
 * @file SourceField.hh
 *
 * Declaration of SourceField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOURCE_FIELD_HH
#define TTA_SOURCE_FIELD_HH

#include "SlotField.hh"

class ImmediateEncoding;
class BridgeEncoding;

/**
 * SourceField class represents the source field of a move slot.
 *
 * It is a specialisation of InstructionField class. In addition to socket
 * encodings, a source field may encode an inline immediate or up to two
 * bridge sources.
 */
class SourceField : public SlotField {
public:
    SourceField(BinaryEncoding::Position componentIDPos, MoveSlot& parent)
	throw (ObjectAlreadyExists, IllegalParameters);
    SourceField(const ObjectState* state, MoveSlot& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~SourceField();

    void addBridgeEncoding(BridgeEncoding& encoding)
	throw (ObjectAlreadyExists);
    void removeBridgeEncoding(BridgeEncoding& encoding);
    bool hasBridgeEncoding(const std::string& bridge) const;
    BridgeEncoding& bridgeEncoding(const std::string& bridge) const;
    int bridgeEncodingCount() const;
    BridgeEncoding& bridgeEncoding(int index) const
	throw (OutOfRange);

    void setImmediateEncoding(ImmediateEncoding& encoding)
        throw (ObjectAlreadyExists);
    void unsetImmediateEncoding();
    bool hasImmediateEncoding() const;
    ImmediateEncoding& immediateEncoding() const;

    // methods inherited from InstructionField
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state)
	throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    /// ObjectState name for source field.
    static const std::string OSNAME_SOURCE_FIELD;

private:
    /// Container type for bridge encodings.
    typedef std::vector<BridgeEncoding*> BridgeEncodingTable;

    void clearBridgeEncodings();
    void clearImmediateEncoding();

    /// Container for bridge encodings.
    BridgeEncodingTable bridgeEncodings_;
    /// The immediate encoding.
    ImmediateEncoding* immEncoding_;
};

#endif
