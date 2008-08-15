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
