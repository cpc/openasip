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
 * @file MoveSlot.hh
 *
 * Declaration of MoveSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
    MoveSlot(const std::string& busName, BinaryEncoding& parent);
    MoveSlot(const ObjectState* state, BinaryEncoding& parent);
    virtual ~MoveSlot();

    BinaryEncoding* parent() const;

    std::string name() const;
    void setName(const std::string& name);

    void setGuardField(GuardField& field);
    void unsetGuardField();
    bool hasGuardField() const;
    GuardField& guardField() const;

    void setSourceField(SourceField& field);
    void unsetSourceField();
    bool hasSourceField() const;
    SourceField& sourceField() const;

    void setDestinationField(DestinationField& field);
    void unsetDestinationField();
    bool hasDestinationField() const;
    DestinationField& destinationField() const;

    // virtual methods derived from InstructionField
    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const;
    virtual int width() const;

    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
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


