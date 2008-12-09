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
