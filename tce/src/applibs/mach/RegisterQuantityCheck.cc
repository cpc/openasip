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
 * @file RegisterQuantityCheck.cc
 * 
 * Implementation of RegisterQuantityCheck class.
 * 
 * Checks that given machine has enough registers.
 *
 * @author Heikki Kultala (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include "RegisterFile.hh"
#include "Machine.hh"
#include "Guard.hh" 
#include "AssocTools.hh"

#include "Conversion.hh"

#include "RegisterQuantityCheck.hh"

RegisterQuantityCheck::RegisterQuantityCheck() : 
    MachineCheck("Checks that machine has enough registers") {}

RegisterQuantityCheck::~RegisterQuantityCheck() {}


bool 
RegisterQuantityCheck::check(
    const TTAMachine::Machine& mach, MachineCheckResults& results) const {

    typedef std::pair<TTAMachine::RegisterFile*, int> Register;
    std::set<Register> guardRegs;

    // find all registers that can be used for guards
    TTAMachine::Machine::BusNavigator busNav = mach.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        TTAMachine::Bus* bus = busNav.item(i);
        for (int j = 0; j < bus->guardCount(); j++) {
            TTAMachine::RegisterGuard* regGuard = 
                dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(j));
            if (regGuard != NULL) {
                guardRegs.insert(
                    Register(
                        regGuard->registerFile(), regGuard->registerIndex()));
            }
        }
    }

    if (guardRegs.size() < 2) {
        std::string msg = "too few predicate registers, 2 needed, ";
        msg += Conversion::toString(guardRegs.size());
        msg += " found";
        results.addError(*this, msg);
    }

    TTAMachine::Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    int intRegs = 0;
    for (int i = 0; i < regNav.count(); i++) {
        TTAMachine::RegisterFile* rf = regNav.item(i);
        if (rf->width() == 32) {
            for (int j = 0; j < rf->size(); j++) {
                if (!AssocTools::containsKey(guardRegs, Register(rf,j))) {
                    intRegs++;
                }
            }
        }
    }

    if (intRegs < 5) {
        results.addError(*this, "too few integer registers");
    }

    return results.errorCount() == 0;
}
