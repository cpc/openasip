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
 * @file DestinationField.hh
 *
 * Declaration of DestinationField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESTINATION_FIELD_HH
#define TTA_DESTINATION_FIELD_HH

#include "SlotField.hh"

/**
 * The DestinationField class represents the destination field of a move slot.
 *
 * The destination field contains only socket encodings and so it is just a
 * realization of SlotField class. It does not provide any additional methods.
 */
class DestinationField : public SlotField {
public:
    DestinationField(BinaryEncoding::Position socketIDPos, MoveSlot& parent);
    DestinationField(const ObjectState* state, MoveSlot& parent);
    virtual ~DestinationField();

    // methods inherited from Serializable interface
    virtual ObjectState* saveState() const;

    /// ObjectState name for destination field.
    static const std::string OSNAME_DESTINATION_FIELD;
};

#endif
