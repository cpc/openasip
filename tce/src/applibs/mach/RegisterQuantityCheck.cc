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
#include "FullyConnectedCheck.hh"
#include "MachineCheckResults.hh"

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
    if (missingIntRegs(intRegs, NULL, fullyConCheck_.check(mach))) {
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
    if (missingIntRegs(intRegs, NULL, fullyConCheck_.check(mach))) {
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
    missingIntRegs(intRegs, &results, fullyConCheck_.check(mach));

    return results.errorCount() == 0;
}


/**
 * Check only if enough integer registers. 
 *
 * @param mach Machine to be checked for int registers.
 * @return True if enough integer registers found.
 */
bool 
RegisterQuantityCheck::checkIntRegs(const TTAMachine::Machine& mach) const {
    // find all registers that can be used for guards
    std::set<Register> guardRegs;
    const std::set<std::string> ignoreRFs; //empty, no ignore
    findGuardRegisters(mach, guardRegs, ignoreRFs);

    // count all integer registers
    unsigned int intRegs = countIntRegisters(mach, guardRegs, ignoreRFs);

    // check if enough integer registers
    if (missingIntRegs(intRegs, NULL, fullyConCheck_.check(mach))) {
        return false;
    }
    return true;
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
 * @param guardRegs A sorted list of register file, index pairs.
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
 * @return number of missing registers, 0 if none missing.
 */
unsigned int
RegisterQuantityCheck::missingIntRegs(
    const unsigned int& regCount,
    MachineCheckResults* results,
    bool isFullyConnected) const {

    unsigned int neededIntRegs = isFullyConnected ? 5 : 6;
    unsigned int missingRegisters = 0;
     
    if (regCount < neededIntRegs) {
        missingRegisters = neededIntRegs - regCount;
        if (results != NULL) {
            results->addError(*this, "too few integer registers");
        }
    }
   return missingRegisters;
}


/**
 * Adds integer registers to an int rf so that requirements are met.
 *
 * @param machine Machine where integer registers are to be added if needed.
 * @return True if something was done to the machine, false otherwise.
 */
bool
RegisterQuantityCheck::fixIntRegs(TTAMachine::Machine& mach) const {
    // find all guard registers, which are ignored
    std::set<Register> guardRegs;
    const std::set<std::string> ignoreRFs; //empty, no ignore
    findGuardRegisters(mach, guardRegs, ignoreRFs);

    // count all integer registers
    unsigned int intRegs = countIntRegisters(mach, guardRegs, ignoreRFs);
   
    unsigned int missingRegs = 
        missingIntRegs(intRegs, NULL, fullyConCheck_.check(mach));

    if (!missingRegs) {
        return true;
    }

    TTAMachine::Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    // find an int rf to add registers
    for (int i = 0; i < regNav.count(); i++) {
        TTAMachine::RegisterFile* rf = regNav.item(i);
        if (rf->width() == 32) {
            rf->setNumberOfRegisters(rf->size() + missingRegs);
            return true;
        }
    }

    // no int rf found
    return false;
}


/**
 * Returns true if an int register file found, meaning int registers can be
 * added.
 *
 * @return True, if int rf found.
 */
bool
RegisterQuantityCheck::canFixIntRegs(const TTAMachine::Machine& mach) const {
    TTAMachine::Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    // return true if an int register file was found
    for (int i = 0; i < regNav.count(); i++) {
        TTAMachine::RegisterFile* rf = regNav.item(i);
        if (rf->width() == 32) {
            return true;
        }
    }
    return false;
}
