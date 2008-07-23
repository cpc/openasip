/**
 * @file TempRegisterFinderPass.cc
 *
 * Class which finds free regs for temp reg copier.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include <set>

#include "TempRegisterFinderPass.hh"

#include "MachineConnectivityCheck.hh"
#include "RegisterFile.hh"
#include "InterPassDatum.hh"
#include "InterPassData.hh"
#include "Machine.hh"
#include "Guard.hh"
#include "AssocTools.hh"

void TempRegisterFinderPass::start() throw (Exception) {

    std::vector<TTAMachine::RegisterFile*> tempRegRFs =
        MachineConnectivityCheck::tempRegisterFiles(*target_);

    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*,int> > > TempRegData;

    typedef std::pair<TTAMachine::RegisterFile*, int> Register;

    std::set<Register> guardRegs;
    TempRegData* tempRegData = new TempRegData;

    // find all registers that can be used for guards
    TTAMachine::Machine::BusNavigator busNav = target_->busNavigator();
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

    // then mark last (non-guard) register of all gotten reg files
    // as tempregcopy reg. 
    for (unsigned int i = 0; i < tempRegRFs.size(); i++) {
        TTAMachine::RegisterFile* rf = tempRegRFs.at(i);
        for (int j = rf->size()-1; j >= 0; j--) {
            // if does not have a guard, only then used.
            // if has guard, try next.
            if (!AssocTools::containsKey(guardRegs, Register(rf,j))) {
                tempRegData->push_back(
                    std::pair<TTAMachine::RegisterFile*,int>(rf, j));
                break; // goto next rf
            }
        }       
    }

    interPassData().setDatum("SCRATCH_REGISTERS", tempRegData);
}

/**
 * A short description of the module, usually the module name,
 * in this case "TempRegisterFinderPass".
 *
 * @return The description as a string.
 */
std::string
TempRegisterFinderPass::shortDescription() const {
    return "Startable: TempRegisterFinderPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
TempRegisterFinderPass::longDescription() const {
    std::string answer ="Startable: TempRegisterFinderPass";
    answer += " Finds free regs for temporary register copier.";
    return answer;
}
SCHEDULER_PASS(TempRegisterFinderPass)
