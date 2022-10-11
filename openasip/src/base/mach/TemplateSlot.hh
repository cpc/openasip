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
 * @file TemplateSlot.hh
 *
 * Declaration of class TemplateSlot.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_HH
#define TTA_TEMPLATE_SLOT_HH

#include <string>
#include <vector>
#include <map>

#include "Application.hh"

class ObjectState;

namespace TTAMachine {

class ImmediateUnit;
class InstructionTemplate;
class ImmediateSlot;
class Bus;

/**
 * Repsesents a bit field of the TTA instruction that is used to encode a
 * piece of a long immediate for a given instruction template or a NOP move.
 */
class TemplateSlot {
public:

    TemplateSlot(
        const Bus& slot,
        int width,
        ImmediateUnit& destination);
    TemplateSlot(
        const ImmediateSlot& slot,
        int width,
        ImmediateUnit& destination);

    TemplateSlot(const Bus& slot);

    virtual ~TemplateSlot();

    int width() const;
    std::string slot() const;
    ImmediateUnit* destination() const;

    ObjectState* saveState() const;

    const Bus* bus() const;
    /// ObjectState name for template slot.
    static const std::string OSNAME_TEMPLATE_SLOT;
    /// ObjectState attribute key for slot name.
    static const std::string OSKEY_SLOT;
    /// ObjectState attribute key for bit width.
    static const std::string OSKEY_WIDTH;
    /// ObjectState attribute key for destination.
    static const std::string OSKEY_DESTINATION;
    // ObjectState attribute key for RF read
    static const std::string OSKEY_RF_READ;
    // ObjectState attribute key for RF write
    static const std::string OSKEY_RF_WRITE;
    // ObjectState attribute key for FU read
    static const std::string OSKEY_FU_READ;
    // ObjectState attribute key for FU write
    static const std::string OSKEY_FU_WRITE;


private:

    /// The bus which is programmed by the instruction bit
    /// field of this template slot.
    const Bus* bus_;

    /// The immediate slot which is programmed by the instruction bit
    /// field of this template slot.
    const ImmediateSlot* immSlot_;

    /// The number of bits that can be encoded in this instruction field.
    int width_;

    /// Destination unit.
    ImmediateUnit* destination_;

};
}

#include "TemplateSlot.icc"

#endif
