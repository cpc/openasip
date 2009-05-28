/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MachineResourceModifier.cc
 *
 * Implementation of the MachineResourceModifier class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <cmath>

#include "MachineResourceModifier.hh"
#include "FullyConnectedCheck.hh"
#include "Conversion.hh"
#include "Guard.hh"
#include "TemplateSlot.hh"

using namespace TTAMachine;

/**
 * The constructor.
 */
MachineResourceModifier::MachineResourceModifier() {
}

/**
 * The destructor.
 */
MachineResourceModifier::~MachineResourceModifier() {
}

/**
 * Adds given number of buses to the given architecture.
 *
 * Determines the bus parameters in basis of the original architecture and
 * the most common bus types are added first.
 *
 * @param busesToAdd Number of buses to add.
 * @mach Architecture where the buses are added. 
 */
void
MachineResourceModifier::addBusesByAmount(
    int busesToAdd, TTAMachine::Machine& mach) {

    BusMap busMap;
    analyzeBuses(mach, busMap);

    int addedBuses = 0;
    std::multimap<double, TTAMachine::Bus*>::const_iterator iter = busMap.end();
    while (iter != busMap.begin()) {
        iter--;
        if (addedBuses == busesToAdd) {
            break;
        }
        double numberToAdd = ceil((*iter).first * busesToAdd);

        while (numberToAdd > 0) {
            if (addedBuses < busesToAdd) {
                TTAMachine::Bus* newBus = 
                    new TTAMachine::Bus((*iter).second->saveState());
                int busNum = 0;
                std::string busBaseName = "bus";
                TTAMachine::Machine::BusNavigator busNavigator = 
                    mach.busNavigator();
                while (busNavigator.hasItem(
                           busBaseName + Conversion::toString(busNum))) {
                    busNum++;
                }
                newBus->setName(busBaseName + Conversion::toString(busNum));
                mach.addBus(*newBus);

                // copy guards to the new bus
                for (int i = 0; i < (*iter).second->guardCount(); i++) {
                    ObjectState* guardState = 
                        (*iter).second->guard(i)->saveState();
                    if (guardState->name() == 
                        RegisterGuard::OSNAME_REGISTER_GUARD) {
                        
                        // RegisterGuard* guard =
                        new RegisterGuard(guardState, *newBus);
                    } else if (guardState->name() == 
                               PortGuard::OSNAME_PORT_GUARD) {
                        
                        //PortGuard* guard =
                        new PortGuard(guardState, *newBus);
                    } else if (guardState->name() ==
                               UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD) {
                        
                        //UnconditionalGuard* guard =
                        new UnconditionalGuard(guardState, *newBus);
                    }
                }
                // Fully connect the machine.
                FullyConnectedCheck check;
                check.fix(mach);
                addedBuses++;
            }
            numberToAdd--;
        }
    }
}

/**
 * Reduces the amount of buses with a given number using some strange
 * algorithm.
 *
 * Won't remove buses that are immediate template slots. Remove slots first.
 * Slots may cause that given number of buses cannot be removed.
 * (I, maattae, have really no idea why this function does what it does. And
 * what it does isn't actually very clear.)
 *
 * @param busesToRemove Number of buses to remove.
 * @param mach Machine where the buses are removed.
 * @param removedBusName Names of the removed buses are added at the end 
 *                       of this list.
 */
void
MachineResourceModifier::reduceBuses(
    const int& busesToRemove, TTAMachine::Machine& mach,
    std::list<std::string>& removedBusNames) {

    BusMap busMap;
    analyzeBuses(mach, busMap);

    int removedBuses = 0;
    std::multimap<double, TTAMachine::Bus*>::const_iterator iter =
        busMap.begin();

    while (iter != busMap.end() && removedBuses != busesToRemove) {

        double numberToRemove = ceil(iter->first * busesToRemove);

        TTAMachine::Machine::BusNavigator navigator = mach.busNavigator();

        for (int i = 0; numberToRemove > 0 && i < navigator.count(); i++) {
            if (removedBuses < busesToRemove) {
                if ((*iter).second->isArchitectureEqual(
                        *navigator.item(i))) {
                    Machine::InstructionTemplateNavigator itNav =
                        mach.instructionTemplateNavigator();
                    if (hasSlot(mach, navigator.item(i)->name())) {
                        // buses that contains template slot are not removed
                        continue;
                    }
                    // remove the bus
                    removedBusNames.push_back(
                        (*iter).second->name());
                    mach.removeBus(*(navigator.item(i)));
                    removedBuses++;
                    numberToRemove--;
                }
            } else {
                // continue with next bus type
                break;
            }
        }
        iter++;
    }
}


/**
 * Removes busses from machine.
 *
 * Doens't remove buses that have template slots.
 *
 * @param countToRemove Number of buses to remove in optimal case.
 * @param mach Machine where buses are going to be removed.
 * @param removedBusName Names of the removed buses are added at the end 
 *                       of this list.
 * @return True if the number of buses to be removed were actually removed,
 * false otherwise.
 */
bool
MachineResourceModifier::removeBuses(
    const int& countToRemove, TTAMachine::Machine& mach,
    std::list<std::string>& removedBusNames) {

    TTAMachine::Machine::BusNavigator navigator = mach.busNavigator();
    Machine::InstructionTemplateNavigator itNav = 
        mach.instructionTemplateNavigator();

    // TODO: make test that the last guard bus isn't removed either.
    for (int busesRemoved = 0, i = 0; 0 < navigator.count(); ++i) {
        if ( countToRemove == busesRemoved ) {
            return true;
        }
        // buses that contains template slot are not removed
        if (hasSlot(mach, navigator.item(i)->name())) {
            continue;
        }
        removedBusNames.push_back(navigator.item(i)->name());
        // remove the bus
        mach.removeBus(*(navigator.item(i)));
        ++busesRemoved;
        --i;
    }
    return false;
}


/**
 * Returns true if the machine contains an Instruction template slot for given
 * bus name.
 *
 * @param mach Machine where the slot is searched.
 * @param busName Name of the slot that is searched.
 * @return True if the machine contains template slot for the given bus name.
 */
bool 
MachineResourceModifier::hasSlot(
    const Machine& mach, const std::string& slotName) {
    
    Machine::InstructionTemplateNavigator itNav =
        mach.instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        for (int slot = 0;
             slot < itNav.item(i)->slotCount();
             slot++) {
            
            if (itNav.item(i)->slot(slot)->slot() == slotName) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Removes not sockets that have no connection to any port from the machine.
 *
 * @param mach Machine where the sockets are removed.
 * @param removedSocketNames Names of the removed sockets are inserted at the
 *                           end of this list.
 */
void
MachineResourceModifier::removeNotConnectedSockets(
    TTAMachine::Machine& mach, std::list<std::string>& removedSocketNames) {

    std::list<std::string> socketsToRemove;
 
    TTAMachine::Machine::SocketNavigator navigator = mach.socketNavigator();
    for (int i = 0; i < navigator.count(); i++) {
        if (navigator.item(i)->portCount() == 0 ||
            navigator.item(i)->segmentCount() == 0) {
            socketsToRemove.push_back(navigator.item(i)->name());
        }
    }

    std::list<std::string>::const_iterator iter = socketsToRemove.begin();
    while (iter != socketsToRemove.end()) {
        mach.removeSocket(*navigator.item(*iter));
        removedSocketNames.push_back(*iter);
        iter++;
    }
}

/**
 * Analyzes the bus types of the architecture.
 *
 * @param mach Machine to be analyzed.
 * @param busMap Map where bus counts are stored.
 */
void
MachineResourceModifier::analyzeBuses(
    const TTAMachine::Machine& mach, BusMap& busMap) const {

    TTAMachine::Machine::BusNavigator busNavigator = mach.busNavigator();

    std::set<int> checkedBuses;
    for (int i = 0; i < busNavigator.count(); i++) {

        // go trough only the buses that are not already counted
        if (checkedBuses.find(i) != checkedBuses.end()) {
            continue;
        }

        TTAMachine::Bus* bus = busNavigator.item(i);        
        int counter = 1;
        for (int j = i + 1; j < busNavigator.count(); j++) {
            if (bus->isArchitectureEqual(*busNavigator.item(j))) {
                checkedBuses.insert(j);
                counter++;
            }
        }
        busMap.insert(
            std::pair<double, TTAMachine::Bus*>(
                Conversion::toDouble(counter) / 
                Conversion::toDouble(busNavigator.count()), bus->copy()));
        checkedBuses.insert(i);
    }

    assert(Conversion::toInt(checkedBuses.size()) == busNavigator.count());    
}


/**
 * Increase all different register files by given amount.
 *
 * @param registersToAdd Number of every register type will be added.
 * @param mach Machine which registers are added.
 */
void
MachineResourceModifier::increaseAllRFsThatDiffersByAmount(
    int registersToAdd, TTAMachine::Machine& mach) {

    RegisterMap rfMap;
    analyzeRegisters(mach, rfMap);
    
    std::multimap<double, TTAMachine::RegisterFile*>::const_iterator iter =
        rfMap.end();
    while (iter != rfMap.begin()) {
        iter--;
        if (!(*iter).second->isUsedAsGuard()) {
            int addedRegisters = 0;
            while (addedRegisters < registersToAdd) {
                TTAMachine::RegisterFile* newRF = 
                    new TTAMachine::RegisterFile((*iter).second->saveState());
                int rfNum = 1;
                std::string rfBaseName = "rf";
                TTAMachine::Machine::RegisterFileNavigator rfNavigator = 
                    mach.registerFileNavigator();
                while (rfNavigator.hasItem(
                           rfBaseName + Conversion::toString(rfNum))) {
                    rfNum++;
                }
                newRF->setName(rfBaseName + Conversion::toString(rfNum));
                mach.addRegisterFile(*newRF);
                // Fully connect the machine.
                FullyConnectedCheck check;
                check.fix(mach);
                addedRegisters++;
            }
        }
    }
}

/**
 * Increase all different register files by percentage value.
 *
 * Fully connects the machine after adding the units.
 *
 * @param percentualRegisterIncrease How much will the register files be
 * increased in percents.
 * @param mach Machine which registers are increased.
 */
void
MachineResourceModifier::percentualRegisterIncrease(
    double percentsOfRegistersToAdd, TTAMachine::Machine& mach) {

    if (percentsOfRegistersToAdd < 0.0) {
        // nothing to add
        return;
    }

    RegisterMap rfMap;
    analyzeRegisters(mach, rfMap);
    TTAMachine::Machine::RegisterFileNavigator rfNavigator =
        mach.registerFileNavigator();
    
    std::multimap<double, TTAMachine::RegisterFile*>::const_iterator iter =
        rfMap.end();
    while (iter != rfMap.begin()) {
        iter--;
        if (!(*iter).second->isUsedAsGuard()) {
            int addedRegisters = 0;
            int registersToAdd = 
                Conversion::toInt(
                    ceil((*iter).first * percentsOfRegistersToAdd));
            while (addedRegisters < registersToAdd) {
                TTAMachine::RegisterFile* newRF = 
                    new TTAMachine::RegisterFile((*iter).second->saveState());
                int rfNum = 1;
                std::string rfBaseName = "rf";
                TTAMachine::Machine::RegisterFileNavigator rfNavigator = 
                    mach.registerFileNavigator();
                while (rfNavigator.hasItem(
                           rfBaseName + Conversion::toString(rfNum))) {
                    rfNum++;
                }
                newRF->setName(rfBaseName + Conversion::toString(rfNum));
                mach.addRegisterFile(*newRF);
                // Fully connect the machine.
                FullyConnectedCheck check;
                check.fix(mach);
                addedRegisters++;
            }
        }
    }
}

/**
 * Analyzes the register types of the architecture.
 *
 * @param mach Machine which registers are analyzed.
 * @param registerMap Map where register counts are stored.
 */
void
MachineResourceModifier::analyzeRegisters(
    const TTAMachine::Machine& mach, RegisterMap& registerMap) const {

    TTAMachine::Machine::RegisterFileNavigator registerNavigator =
        mach.registerFileNavigator();

    std::set<int> checkedRegisters;
    for (int i = 0; i < registerNavigator.count(); i++) {
        // go trough only the registers that are not already counted
        if (checkedRegisters.find(i) != checkedRegisters.end()) {
            continue;
        }
        // @todo boolean registers are propably not needed to multiply
        TTAMachine::RegisterFile* rf = registerNavigator.item(i);        
        int counter = 1;
        for (int j = i + 1; j < registerNavigator.count(); j++) {
            if (rf->isArchitectureEqual(*registerNavigator.item(j))) {
                checkedRegisters.insert(j);
                counter++;
            }
        }
        
        registerMap.insert(
            std::pair<double, TTAMachine::RegisterFile*>(
                Conversion::toDouble(counter) / 
                Conversion::toDouble(registerNavigator.count()), rf));

        checkedRegisters.insert(i);
    }

    assert(Conversion::toInt(checkedRegisters.size())
           == registerNavigator.count());    
}

/**
 * Increase all different funtion units by given amount.
 * 
 * @param unitsToAdd How many function units are added for each fu type.
 * @param mach Machine where the function units are added.
 */
void
MachineResourceModifier::increaseAllFUsThatDiffersByAmount(
    int unitsToAdd, TTAMachine::Machine& mach) {

    FunctionUnitMap fuMap;
    analyzeFunctionUnits(mach, fuMap);
    
    std::multimap<double, TTAMachine::FunctionUnit*>::const_iterator iter =
        fuMap.end();
    while (iter != fuMap.begin()) {
        iter--;
        int addedUnits = 0;
        while (addedUnits < unitsToAdd) {
            ObjectState* newFuState = (*iter).second->saveState();
            TTAMachine::FunctionUnit* newFU = 
                new TTAMachine::FunctionUnit(newFuState);
            delete newFuState;

            newFuState = NULL;
            int fuNum = 1;
            std::string fuBaseName = "fu";
            TTAMachine::Machine::FunctionUnitNavigator fuNavigator = 
                mach.functionUnitNavigator();
            while (fuNavigator.hasItem(
                       fuBaseName + Conversion::toString(fuNum))) {
                fuNum++;
            }
            newFU->setName(fuBaseName + Conversion::toString(fuNum));
            mach.addFunctionUnit(*newFU);
            
            // set the same address space to the new unit.
            newFU->setAddressSpace((*iter).second->addressSpace());

            // Fully connect the machine (adds new sockets).
            FullyConnectedCheck check;
            check.fix(mach);
            addedUnits++;
        }
    }
}

/**
 * Increase all different function units by percentage value.
 *
 * Fully connects the machine after adding the units.
 *
 * @param percentualFUIncrease How much will the function units be
 * increased in percents.
 * @param mach Machine which function units are increased.
 */
void
MachineResourceModifier::percentualFUIncrease(
    double percentualFUIncrease, TTAMachine::Machine& mach) {

    if (percentualFUIncrease < 0.0) {
        // nothing to add
        return;
    }

    FunctionUnitMap fuMap;
    analyzeFunctionUnits(mach, fuMap);
    TTAMachine::Machine::FunctionUnitNavigator fuNavigator =
        mach.functionUnitNavigator();
    
    std::multimap<double, TTAMachine::FunctionUnit*>::const_iterator iter =
        fuMap.end();
    while (iter != fuMap.begin()) {
        iter--;
        int addedUnits = 0;
        int unitsToAdd = 
            Conversion::toInt(ceil((*iter).first * percentualFUIncrease));
        while (addedUnits < unitsToAdd) {
            TTAMachine::FunctionUnit* newFU = 
                new TTAMachine::FunctionUnit((*iter).second->saveState());
            int fuNum = 1;
            std::string fuBaseName = "fu";
            TTAMachine::Machine::FunctionUnitNavigator fuNavigator = 
                mach.functionUnitNavigator();
            while (fuNavigator.hasItem(
                       fuBaseName + Conversion::toString(fuNum))) {
                fuNum++;
            }
            newFU->setName(fuBaseName + Conversion::toString(fuNum));
            mach.addFunctionUnit(*newFU);

            // set the same address space to the new unit.
            newFU->setAddressSpace((*iter).second->addressSpace());

            // Fully connect the machine.
            FullyConnectedCheck check;
            check.fix(mach);
            addedUnits++;
        }
    }
}

/**
 * Analyzes the unit types of the architecture.
 *
 * @param mach Machine which function units are analyzed.
 * @param unitMap Map where unit counts are stored. FU pointers in the map
 *                are pointers to the machine units.
 */
void
MachineResourceModifier::analyzeFunctionUnits(
    const TTAMachine::Machine& mach, FunctionUnitMap& unitMap) const {

    TTAMachine::Machine::FunctionUnitNavigator unitNavigator =
        mach.functionUnitNavigator();

    std::set<int> checkedUnits;
    for (int i = 0; i < unitNavigator.count(); i++) {
        // go trough only the units that are not already counted
        if (checkedUnits.find(i) != checkedUnits.end()) {
            continue;
        }

        TTAMachine::FunctionUnit* fu = unitNavigator.item(i);        
        int counter = 1;
        for (int j = i + 1; j < unitNavigator.count(); j++) {
            if (fu->isArchitectureEqual(unitNavigator.item(j))) {
                checkedUnits.insert(j);
                counter++;
            }
        }

        unitMap.insert(
            std::pair<double, TTAMachine::FunctionUnit*>(
                Conversion::toDouble(counter) / 
                Conversion::toDouble(unitNavigator.count()), fu));
        checkedUnits.insert(i);
    }

    assert(Conversion::toInt(checkedUnits.size())
           == unitNavigator.count());    
}
