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
#include <list>
#include <string>

namespace TTAMachine {
    class Machine;
    class Bus;
    class RegisterFile;
    class FunctionUnit;
}
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
