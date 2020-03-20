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
        const TTAMachine::Machine& mach) const;
    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results) const;

    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& mach) const;

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
