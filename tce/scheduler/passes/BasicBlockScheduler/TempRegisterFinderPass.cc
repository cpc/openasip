/**
 * @file TempRegisterFinderPass.hh
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

void TempRegisterFinderPass::start() throw (Exception) {
    
    bool allConnected = true;
    std::vector<TTAMachine::RegisterFile*> tempRegRFs = 
        MachineConnectivityCheck::tempRegisterFiles(*target_);

    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*,int> > > TempRegData;
    TempRegData* tempRegData = new TempRegData;
    
    for (unsigned int i = 0; i < tempRegRFs.size(); i++) {
        tempRegData->push_back(
            std::pair<TTAMachine::RegisterFile*,int>(
                tempRegRFs.at(i), tempRegRFs.at(i)->numberOfRegisters()-1));
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
