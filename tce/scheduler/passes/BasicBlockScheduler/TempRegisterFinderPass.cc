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

SCHEDULER_PASS(TempRegisterFinderPass)
