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
 * @file FUPort.hh
 *
 * Declaration of FUPort class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#ifndef TTA_FU_PORT_HH
#define TTA_FU_PORT_HH

#include <string>

#include "BaseFUPort.hh"

namespace TTAMachine {

/**
 * Represens an operand, trigger or result port of a function unit.
 */
class FUPort : public BaseFUPort {
public:
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);
    FUPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~FUPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    void setTriggering(bool triggers);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    std::string bindingString() const;
    void updateBindingString() const;

    bool isArchitectureEqual(FUPort* port);

    /// ObjectState name for FUPort.
    static const std::string OSNAME_FUPORT;
    /// ObjectState attribute key for triggering feature.
    static const std::string OSKEY_TRIGGERING;
    /// ObjectState attribute key for operand code setting feature.
    static const std::string OSKEY_OPCODE_SETTING;

protected:
    // this is for UniversalFUPort class
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode,
        bool dummy)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);

private:
    void cleanupGuards() const;
    void cleanupOperandBindings() const;
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Specifies whether this is a triggering port.
    bool triggers_;
    /// Specifies whether this is an operation selecting port.
    bool setsOpcode_;
    /// Binding string describes the operation bindings of 
    /// of the port to allow fast binding comparison.
    mutable std::string bindingString_;
};
}

#endif
