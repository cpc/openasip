/**
 * @file FullyConnectedCheck.hh
 *
 * Declaration of FullyConnectedCheck class that checks and fixes machines
 * fully connectivity.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef FULLY_CONNECTED_CHECK_HH
#define FULLY_CONNECTED_CHECK_HH

#include "MachineCheck.hh"
#include "Socket.hh"

namespace TTAMachine {
    class FUPort;
    class RFPort;
    class Unit;
    class SpecialRegisterPort;
}

/**
 * MachineCheck to check is the machine fully connected.
 */
class FullyConnectedCheck : public MachineCheck {
public:
    FullyConnectedCheck();
    virtual ~FullyConnectedCheck();

    virtual bool check(
        const TTAMachine::Machine& mach, MachineCheckResults& results) const;
    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& mach) const
        throw (InvalidData);
protected:
    FullyConnectedCheck(const std::string& shortDesc);
private:
    void connectIUPort(TTAMachine::RFPort& port) const;
    void connectSpecialRegisterPort(
        TTAMachine::SpecialRegisterPort& port) const;
    void connectRFPort(TTAMachine::RFPort& port) const;
    TTAMachine::Socket* createSocket(
        const TTAMachine::Unit& unit, TTAMachine::Socket::Direction direction)
        const;
    void connectFUPort(TTAMachine::FUPort& port) const;
    void attachSocketToAllBusses(TTAMachine::Socket& socket) const;
    bool socketAttachedToAllBusses(const TTAMachine::Socket& socket) const;
};

#endif
