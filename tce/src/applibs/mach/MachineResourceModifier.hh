/**
 * @file MachineResourceModifier.hh
 *
 * Adds or removes machine resources.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
        int busesToRemove, TTAMachine::Machine& mach,
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
