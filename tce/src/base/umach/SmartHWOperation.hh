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
 * @file SmartHWOperation.hh
 *
 * Declaration of SmartHWOperation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_SMART_HW_OPERATION_HH
#define TTA_SMART_HW_OPERATION_HH

#include "HWOperation.hh"

class Operation;
class UniversalFunctionUnit;

/**
 * Represents an operation in a function unit. The smart operation
 * automatically creates operand bindings on demand. This kind of
 * operations are used in UniversalFunctionUnit. The last operand
 * requested is bound to the operation code setting port.
 */
class SmartHWOperation : public TTAMachine::HWOperation {
public:
    SmartHWOperation(
        const Operation& operation,
        UniversalFunctionUnit& parent)
        throw (ComponentAlreadyExists, InvalidName);
    virtual ~SmartHWOperation();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    TTAMachine::FUPort* port(int operand) const;

    UniversalFunctionUnit* parentUnit() const;

    virtual void bindPort(int operand, const TTAMachine::FUPort& port)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange);
    virtual void unbindPort(const TTAMachine::FUPort& port);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    bool otherMandatoryInputsBound(int operand) const;

    /// The operation represented by this SmartHWOperation instance.
    const Operation& operation_;
    /// Tells whether this operation has 32 bits wide operands.
    bool is32BitOperation_;
};

#endif
