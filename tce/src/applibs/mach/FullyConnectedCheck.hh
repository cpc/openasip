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
 * @file FullyConnectedCheck.hh
 *
 * Declaration of FullyConnectedCheck class that checks and fixes machines
 * fully connectivity.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FULLY_CONNECTED_CHECK_HH
#define FULLY_CONNECTED_CHECK_HH

#include "MachineCheck.hh"
#include "Socket.hh"

namespace TTAMachine {
    class FUPort;
    class RFPort;
    class Unit;
    class SpecialRegisterPort;
}

/**
 * MachineCheck to check is the machine fully connected.
 */
class FullyConnectedCheck : public MachineCheck {
public:
    FullyConnectedCheck();
    virtual ~FullyConnectedCheck();

    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results) const;
    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& mach) const
        throw (InvalidData);
    void connectFUPort(TTAMachine::FUPort& port) const;
protected:
    FullyConnectedCheck(const std::string& shortDesc);
private:
    void connectIUPort(TTAMachine::RFPort& port) const;
    void connectSpecialRegisterPort(
        TTAMachine::SpecialRegisterPort& port) const;
    void connectRFPort(TTAMachine::RFPort& port, TTAMachine::Socket::Direction direction) const;
    TTAMachine::Socket* createSocket(
        const TTAMachine::Unit& unit, TTAMachine::Socket::Direction direction)
        const;
    void attachSocketToAllBusses(TTAMachine::Socket& socket) const;
    bool socketAttachedToAllBusses(const TTAMachine::Socket& socket) const;
};

#endif
