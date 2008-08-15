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
 * @file SpecialRegisterPort.hh
 *
 * Declaration of SpecialRegisterPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SPECIAL_REGISTER_PORT_HH
#define TTA_SPECIAL_REGISTER_PORT_HH

#include "BaseFUPort.hh"

namespace TTAMachine {

class ControlUnit;

/**
 * SpecialRegisterPort class represent a port of a special register in
 * control unit.
 *
 * An example of such a port is the return address port.
 */
class SpecialRegisterPort : public BaseFUPort {
public:
    SpecialRegisterPort(
        const std::string& name,
        int width,
        ControlUnit& parent)
        throw (InvalidName, OutOfRange, ComponentAlreadyExists);
    SpecialRegisterPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~SpecialRegisterPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for special register port.
    static const std::string OSNAME_SPECIAL_REG_PORT;
};
}

#endif
