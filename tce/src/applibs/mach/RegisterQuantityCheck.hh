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
 * @file RegisterQuantityCheck.hh
 * 
 * Declaration of RegisterQuantityCheck class.
 * 
 * Tests that given machine has enough registers.
 *
 * @author Heikki Kultala (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_QUANTITY_CHECK_HH
#define TTA_REGISTER_QUANTITY_CHECK_HH

#include "MachineCheck.hh"
#include "FullyConnectedCheck.hh"

#include <set>
#include <string>

namespace TTAMachine {
    class RegisterFile;
}

class RegisterQuantityCheck : public MachineCheck {
public:
    RegisterQuantityCheck();
    virtual ~RegisterQuantityCheck();

    virtual bool check(
        const TTAMachine::Machine& mach) const;
    virtual bool check(
        const TTAMachine::Machine& mach, 
        MachineCheckResults& results) const;

    bool checkWithIgnore(
        const TTAMachine::Machine& mach, 
        const std::set<std::string>& ignoreRFs) const;
    bool checkWithIgnore(
        const TTAMachine::Machine& mach, 
        MachineCheckResults& results, 
        const std::set<std::string>& ignoreRFs) const;
    
    bool checkIntRegs(const TTAMachine::Machine& mach) const;
    bool canFixIntRegs(const TTAMachine::Machine& mach) const;
    bool fixIntRegs(TTAMachine::Machine& mach) const;
private:
    typedef std::pair<const TTAMachine::RegisterFile*, int> Register;
    void findGuardRegisters(
        const TTAMachine::Machine& mach, 
        std::set<Register>& registers,
        const std::set<std::string>& ignoreRFs) const;
    unsigned int countIntRegisters(
        const TTAMachine::Machine& mach, 
        const std::set<Register>& guardRegs,
        const std::set<std::string>& ignoreRFs) const;
    bool checkPredRegs(
        const unsigned int& regCount,
        MachineCheckResults* results) const;
    unsigned int missingIntRegs(
        const unsigned int& regCount,
        MachineCheckResults* results,
        bool isFullyConnected) const;

    FullyConnectedCheck fullyConCheck_;
};

#endif
