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

#include "Application.hh"

class ObjectState;

namespace TTAMachine {

class ImmediateUnit;
class InstructionTemplate;
class ImmediateSlot;
class Bus;

/**
 * Repsesents a bit field of the TTA instruction that is used to encode a
 * piece of a long immediate for a given instruction template.
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
    virtual ~TemplateSlot();

    int width() const;
    std::string slot() const;
    ImmediateUnit* destination() const;

    ObjectState* saveState() const;

    /// ObjectState name for template slot.
    static const std::string OSNAME_TEMPLATE_SLOT;
    /// ObjectState attribute key for slot name.
    static const std::string OSKEY_SLOT;
    /// ObjectState attribute key for bit width.
    static const std::string OSKEY_WIDTH;
    /// ObjectState attribute key for destination.
    static const std::string OSKEY_DESTINATION;

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
