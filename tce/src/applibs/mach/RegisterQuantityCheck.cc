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
