/**
 * @file RegisterMap.cc
 *
 * Implementation of RegisterMap class
 *
 * Stores references to all registers on machine
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include "RegisterMap.hh"

#include "Machine.hh"
#include "RegisterFile.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "Guard.hh"
#include "MachineConnectivityCheck.hh"

#include "InterPassDatum.hh"
#include "InterPassData.hh"

#include <vector>

//TODO: put these into some one good place
static const int FPR_OFFSET = 1024;

using namespace TTAMachine;
using namespace TTAProgram;

RegisterMap::RegisterMap(
    const TTAMachine::Machine& machine, InterPassData& interPassData)
    : machine_(machine) {
    Machine::RegisterFileNavigator regNav =
        machine.registerFileNavigator();

    RegisterFile* connectedRF = NULL;

    std::list<TTAMachine::RegisterFile*> boolRFs;
    std::list<TTAMachine::RegisterFile*> intRFs;
    std::list<TTAMachine::RegisterFile*> fpRFs;

    // find all registers that can be used for guards
    Machine::BusNavigator busNav = machine.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int j = 0; j < bus->guardCount(); j++) {
            RegisterGuard* regGuard = 
                dynamic_cast<RegisterGuard*>(bus->guard(j));
            if (regGuard != NULL) {
                guards_.push_back(regGuard);
            }
        }
    }

    for (int i = 0; i < regNav.count(); i++) {

        // only GPR's for now
        // ehhh... hardcoded bitwidth 32 for int RFs?? --Pekka
        // what about smaller int RF and float RFs?
        if (regNav.item(i)->width() == 32) {
            intRFs.push_back(regNav.item(i));
        } else {
            // ehhh... part II: all int regs with less than 32 bits
            // considered boolean RFs?? WTF? --Pekka
            if (regNav.item(i)->width() < 32) {
                boolRFs.push_back(regNav.item(i));
            } else {
                fpRFs.push_back(regNav.item(i));
            }
        }   
    }

    bool allConnected = true;
    std::vector<RegisterFile*> tempRegRFs = 
        MachineConnectivityCheck::tempRegisterFiles(machine);
    if (tempRegRFs.size() == 1) {
        connectedRF = tempRegRFs.at(0);
    }
    if (tempRegRFs.size() > 0) {
        allConnected = false;
    }
    
    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*,int> > > TempRegData;
    TempRegData* tempRegData = new TempRegData;
    for (unsigned int i = 0; i < tempRegRFs.size(); i++) {
        tempRegData->push_back(
            std::pair<TTAMachine::RegisterFile*,int>(
                tempRegRFs.at(i), tempRegRFs.at(i)->numberOfRegisters()-1));
    }

    interPassData.setDatum("SCRATCH_REGISTERS", tempRegData);
    
    addRegFiles(intRFs, gprs_, allConnected, connectedRF);
    addRegFiles(fpRFs, fprs_, allConnected, connectedRF);
    addRegFiles(boolRFs, bools_, allConnected, connectedRF);

}


void
RegisterMap::addRegFiles(
    std::list<RegisterFile*>& rfs,
    std::vector<TTAProgram::TerminalRegister*>& 
    regClass, bool allRfsConnected,
    RegisterFile* connectedRF) {

    typedef std::list<RegisterFile*>::iterator rfli;
    for (rfli iter = rfs.begin(); iter != rfs.end(); iter++) {
        int count = (*iter)->numberOfRegisters();
        bool needTemp = false;
        if (!allRfsConnected) {
            if ((connectedRF == NULL && ((*iter)->width() != 1))
                || connectedRF == *iter) {
                needTemp = true; // reserve last for connectivity temp
            }
        }
        for (int j = count-1; j >= 0; j--) {
            if (hasGuard(**iter, j)) {
                bools_.push_back(new TerminalRegister(*((*iter)->port(0)), j));
            } else {
                if (needTemp) {
                    // use this as temp reg
                    needTemp = false;
                } else {
                    regClass.push_back(
                        new TerminalRegister(*((*iter)->port(0)), j));
                }
            }
        }
    }
}

const TerminalRegister& 
RegisterMap::terminal(int index) const {
    if( index >= FPR_OFFSET) {
        return *fprs_.at(index-FPR_OFFSET);
    } else {
        return *gprs_.at(index);
    }
}

const TerminalRegister& 
RegisterMap::brTerminal(int index) const {
    return *bools_.at(index);
}

unsigned int 
RegisterMap::gprCount() const {
    return gprs_.size();
}

unsigned int 
RegisterMap::brCount() const {
    return bools_.size();
}

unsigned int 
RegisterMap::fprCount() const {
    return fprs_.size();
}

RegisterMap::~RegisterMap() {

    for (unsigned int i = 0; i < gprs_.size(); i++) {
        delete gprs_.at(i);
    }
    gprs_.clear();
    
    for (unsigned int i = 0; i < fprs_.size(); i++) {
        delete fprs_.at(i);
    }
    fprs_.clear();

    for (unsigned int i = 0; i <  bools_.size(); i++) {
        delete bools_.at(i);
    }
    bools_.clear();
}

/**
 * Tells whether a register has a guard associated to it.
 *
 * @param rf Register File of the register
 * @param index index Index of the register.
 *
 * @return wheter there is a guard that uses the register.
 */
bool 
RegisterMap::hasGuard(TTAMachine::RegisterFile& rf, int index) const {
    for (unsigned int i = 0; i < guards_.size(); i++) {
        RegisterGuard* rg = guards_.at(i);
        if (rg->registerFile() == &rf && rg->registerIndex() == index) {
            return true;
        } 
    }
    return false;
}
