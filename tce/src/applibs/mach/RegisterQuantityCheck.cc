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
 * @author Heikki Kultala (hkultala-no.spam-cs.tut.fi)
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
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


/**
 * Checks register quantities, just returns false if problems where found.
 *
 * @param mach Machine to be checked for registers resources.
 * @param results MachineCheckResults where possible errors are added.
 * @return True if no problems were found during testing, false otherwise.
 */
bool 
RegisterQuantityCheck::check(const TTAMachine::Machine& mach) const {
    std::set<Register> guardRegs;
    const std::set<std::string> ignoreRFs; //empty, no ignore

    // find all registers that can be used for guards
    findGuardRegisters(mach, guardRegs, ignoreRFs);

    // check if enough predicate registers
    if (!checkPredRegs(guardRegs.size(), NULL)) {
        return false;
    }

    // count all integer registers
    unsigned int intRegs = countIntRegisters(mach, guardRegs, ignoreRFs);

    // check if enough integer registers
    if (!checkIntRegs(intRegs, NULL)) {
        return false;
    }

    return true;
}


/**
 * Checks register quantities.
 *
 * Stores errors during checking to a MachineCheckResults object given as a
 * parameter.
 *
 * @param mach Machine to be checked for registers resources.
 * @param results MachineCheckResults where possible errors are added.
 * @return True if no problems were found during testing, false otherwise.
 */
bool 
RegisterQuantityCheck::check(
    const TTAMachine::Machine& mach, 
    MachineCheckResults& results) const {

    const std::set<std::string> empty;
    return RegisterQuantityCheck::checkWithIgnore(mach, results, empty);
}


/**
 * Checks register quantities with an option to ignore some RFs.
 *
 * Can be passed a list of RFs names that are ignored regarding the test.
 *
 * @param mach Machine to be checked for registers resources.
 * @param ignoreRFs A sorted list of RFs to be ignored while checking the
 *        needed registers.
 * @return True if no problems were found during testing, false otherwise.
 */
bool 
RegisterQuantityCheck::checkWithIgnore(
    const TTAMachine::Machine& mach, 
    const std::set<std::string>& ignoreRFs) const {

    std::set<Register> guardRegs;

    // find all registers that can be used for guards
    findGuardRegisters(mach, guardRegs, ignoreRFs);

    // check if enough predicate registers
    if (!checkPredRegs(guardRegs.size(), NULL)) {
        return false;
    }

    // count all integer registers
    unsigned int intRegs = countIntRegisters(mach, guardRegs, ignoreRFs);

    // check if enough integer registers
    if (!checkIntRegs(intRegs, NULL)) {
        return false;
    }

    return true;
}


/**
 * Checks register quantities with an option to ignore some RFs.
 *
 * Can be passed a list of RFs names that are ignored regarding the test.
 * Stores errors during checking to a MachineCheckResults object given as a
 * parameter.
 *
 * @param mach Machine to be checked for registers resources.
 * @param results MachineCheckResults where possible errors are added.
 * @param ignoreRFs A sorted list of RFs to be ignored while checking the
 *        needed registers.
 * @return True if no problems were found during testing, false otherwise.
 */
bool 
RegisterQuantityCheck::checkWithIgnore(
    const TTAMachine::Machine& mach, 
    MachineCheckResults& results, 
    const std::set<std::string>& ignoreRFs) const {

    std::set<Register> guardRegs;

    // find all registers that can be used for guards
    findGuardRegisters(mach, guardRegs, ignoreRFs);

    // check if enough predicate registers
    checkPredRegs(guardRegs.size(), &results);

    // count all integer registers
    unsigned int intRegs = countIntRegisters(mach, guardRegs, ignoreRFs);

    // check if enough integer registers
    checkIntRegs(intRegs, &results);

    return results.errorCount() == 0;
}


/**
 * Checks register quantities with an option to ignore some RFs.
 *
 * Can be passed a list of RFs names that are ignored regarding the test.
 *
 * @param mach Machine to be checked for registers resources.
 * @param guardRegs Counts registers that can be used as guards in the
 *        given machine.
 * @param ignoreRFs A sorted list of RFs to be ignored while checking the
 *        needed registers.
 * @return True if no problems were found during testing, false otherwise.
 */
void
RegisterQuantityCheck::findGuardRegisters(
    const TTAMachine::Machine& mach, 
    std::set<Register>& guardRegs,
    const std::set<std::string>& ignoreRFs) const {
    
    // find all registers that can be used for guards
    TTAMachine::Machine::BusNavigator busNav = mach.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        TTAMachine::Bus* bus = busNav.item(i);
        for (int j = 0; j < bus->guardCount(); j++) {
            TTAMachine::RegisterGuard* regGuard = 
                dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(j));
            if (regGuard != NULL) {
                if(ignoreRFs.find(regGuard->registerFile()->name()) ==
                        ignoreRFs.end()) {
                    guardRegs.insert(
                        Register(
                            regGuard->registerFile(), regGuard->registerIndex()));
                }
            }
        }
    }
}


/**
 * Counts integer registers int the given machine.
 *
 * Doesn't count guard registers given as sorted list of pairs.
 * Can be passed a list of RFs names that are ignored regarding the test.
 *
 * @param mach Machine where integer registers are counted.
 * @param registers A sorted list of register file, index pairs.
 * @param ignoreRFs A sorted list of RFs to be ignored while counting the
 *        registers.
 * @return The number of counted integer registers in them machine.
 */
unsigned int
RegisterQuantityCheck::countIntRegisters(
    const TTAMachine::Machine& mach, 
    const std::set<Register>& guardRegs,
    const std::set<std::string>& ignoreRFs) const {

    TTAMachine::Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    unsigned int intRegs = 0;
    for (int i = 0; i < regNav.count(); i++) {
        TTAMachine::RegisterFile* rf = regNav.item(i);
        if (rf->width() == 32 && 
                ignoreRFs.find(rf->name()) == ignoreRFs.end()) {
            for (int j = 0; j < rf->size(); j++) {
                if (!AssocTools::containsKey(guardRegs, Register(rf,j))) {
                    intRegs++;
                }
            }
        }
    }
    return intRegs;
}


/**
 * Checks that there are enough predicate registers.
 *
 * Outputs errors in MachineCheckResults object if given as a pointer.
 *
 * @param regCount The number of predicate registers.
 * @param results MachineCheckResults where possible errors are added.
 * @return True if the number of predicate registers given was high enough.
 */
bool
RegisterQuantityCheck::checkPredRegs(
    const unsigned int& regCount,
    MachineCheckResults* results) const {
    
    if (regCount < 2) {
        if (results != NULL) {
            std::string msg = "too few predicate registers, 2 needed, ";
            msg += Conversion::toString(regCount);
            msg += " found";
            results->addError(*this, msg);
        }
        return false;
    }
   return true;
}


/**
 * Checks that there are enough integer registers.
 *
 * Outputs errors in MachineCheckResults object if given as a pointer.
 *
 * @param regCount The number of integer registers.
 * @param results MachineCheckResults where possible errors are added.
 * @return True if the number of integer registers given was high enough.
 */
bool
RegisterQuantityCheck::checkIntRegs(
    const unsigned int& regCount,
    MachineCheckResults* results) const {
     
    if (regCount < 5) {
        if (results != NULL) {
            results->addError(*this, "too few integer registers");
        }
        return false;
    }
   return true;
}
