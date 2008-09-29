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
 * @file MachineResourceModifier.hh
 *
 * Adds or removes machine resources.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_RESOURCE_MODIFIER_HH
#define TTA_MACHINE_RESOURCE_MODIFIER_HH

#include <map>
#include "Machine.hh"
#include "Bus.hh"

/**
 * Class that adds or removes resources of the given architecture.
 */
class MachineResourceModifier {
public:
    /// Map of bus amounts in percents.
    typedef std::multimap<double, TTAMachine::Bus*> BusMap;
    /// Map of register amounts in percents.
    typedef std::multimap<double, TTAMachine::RegisterFile*> RegisterMap;
    /// Map of function unit amounts in percents.
    typedef std::multimap<double, TTAMachine::FunctionUnit*> FunctionUnitMap;

    MachineResourceModifier();
    virtual ~MachineResourceModifier();
    void addBusesByAmount(int busesToAdd, TTAMachine::Machine& mach);

    //void percentualBusIncrease(
    //    int percentualBusIncrease, TTAMachine::Machine& mach);
    // not implemented

    void increaseAllRFsThatDiffersByAmount(
        int registersToAdd, TTAMachine::Machine& mach);
    void percentualRegisterIncrease(
        double percentsOfRegistersToAdd, TTAMachine::Machine& mach);
    void increaseAllFUsThatDiffersByAmount(
        int moreFUs, TTAMachine::Machine& mach);
    void percentualFUIncrease(
        double percentualFUIncrease, TTAMachine::Machine& mach);

    void reduceBuses(
        const int& busesToRemove, 
        TTAMachine::Machine& mach,
        std::list<std::string>& removedBusNames);
    bool removeBuses(
        const int& countToRemove, 
        TTAMachine::Machine& mach,
        std::list<std::string>& removedBusNames);

    void removeNotConnectedSockets(
        TTAMachine::Machine& mach, std::list<std::string>& removedSocketNames);

    void analyzeBuses(const TTAMachine::Machine& mach, BusMap& busMap) const;
    void analyzeRegisters(
        const TTAMachine::Machine& mach, RegisterMap& registerMap) const;
    void analyzeFunctionUnits(
        const TTAMachine::Machine& mach, FunctionUnitMap& unitMap) const;

private:
    bool hasSlot(const TTAMachine::Machine& mach, const std::string& slotName);
};

#endif
