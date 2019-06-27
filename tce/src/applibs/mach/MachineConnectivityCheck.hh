/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MachineConnectivityCheck.hh
 *
 * Declaration of MachineConnectivityCheck class.
 *
 * @author Pekka J��skel�inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CONNECTIVITY_CHECK_HH
#define TTA_MACHINE_CONNECTIVITY_CHECK_HH

#include <set>
#include <map>
#include <vector>

#include "MachineCheck.hh"
#include "ProgramAnnotation.hh"

class TCEString;
class MoveNode;
class Operation;

namespace TTAMachine {
    class Port;
    class Bus;
    class RegisterFile;
    class BaseRegisterFile;
    class Unit;
    class FunctionUnit;
    class Guard;
}

namespace TTAProgram {
    class TerminalImmediate;
    class Move;
}

/**
 * Base class for machine interconnection connectivity checks.
 *
 * Provides helper functions that are useful for many derived checks.
 */
class MachineConnectivityCheck : MachineCheck {
public:
    static bool isConnected(
        const TTAMachine::Port& sourcePort,
        const TTAMachine::Port& destinationPort,
        TTAMachine::Guard* guard = NULL);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::Port& destPort);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::BaseRegisterFile& destRF,
        TTAMachine::Guard* guard = NULL);

    static bool isConnectedWithBothGuards(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::BaseRegisterFile& destRF,
        std::pair<TTAMachine::RegisterFile*,int> guardReg);

    static bool isConnected(
        const TTAMachine::Port& sourcePort,
        const TTAMachine::RegisterFile& destRF);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::FunctionUnit& destFU);

    static bool isConnected(
        std::set<const TTAMachine::Port*> sourcePorts,
        std::set<const TTAMachine::Port*> destinationPorts,
        TTAMachine::Guard* guard = NULL);

    static bool canWriteAllImmediates(TTAMachine::Port& destPort);

    static bool canTransportImmediate(
        const TTAProgram::TerminalImmediate& immediate,
        const TTAMachine::BaseRegisterFile& destRF,
        TTAMachine::Guard* guard = NULL);

    static bool canTransportImmediate(
        const TTAProgram::TerminalImmediate& immediate,
        const TTAMachine::Port& destinationPort,
        TTAMachine::Guard* guard = NULL);

    static bool canTransportImmediate(
        const TTAProgram::TerminalImmediate& immediate,
        std::set<const TTAMachine::Port*> destinationPorts,
        TTAMachine::Guard* guard = NULL);
    
    static bool canTransportMove(
        MoveNode& moveNode, const TTAMachine::Machine& machine);

    static bool rfConnected(
        const TTAMachine::RegisterFile& rf);

    static bool fromRfConnected(
        const TTAMachine::BaseRegisterFile& brf);

    static bool toRfConnected(
        const TTAMachine::RegisterFile& brf);

    static std::set<TTAMachine::Bus*> connectedSourceBuses(
        const TTAMachine::Port& port);

    static std::set<TTAMachine::Bus*> connectedDestinationBuses(
        const TTAMachine::Port& port);

    static void appendConnectedSourceBuses(
        const TTAMachine::Port& port, std::set<TTAMachine::Bus*>& buses);

    static void appendConnectedDestinationBuses(
        const TTAMachine::Port& port, std::set<TTAMachine::Bus*>& buses);

    static void appendConnectedSourceBuses(
        const TTAMachine::Unit& unit, std::set<TTAMachine::Bus*>& buses);

    static void appendConnectedDestinationBuses(
        const TTAMachine::Unit& unit, std::set<TTAMachine::Bus*>& buses);

    static std::vector<TTAMachine::RegisterFile*> tempRegisterFiles(
        const TTAMachine::Machine& machine);

    static int requiredImmediateWidth(
        bool signExtension,
        const TTAProgram::TerminalImmediate& source,
        const TTAMachine::Machine& mach);

    static bool busConnectedToPort(
        const TTAMachine::Bus& bus, const TTAMachine::Port& port);
    
    static bool busConnectedToRF(
        const TTAMachine::Bus& bus, const TTAMachine::Unit& destRF);

    static bool busConnectedToFU(
        const TTAMachine::Bus& bus, const TTAMachine::FunctionUnit& fu, 
        const TCEString& opName, int opIndex);

    static bool busConnectedToAnyFU(
        const TTAMachine::Bus& bus, const MoveNode& moveNode);

    static bool busConnectedToDestination(
        const TTAMachine::Bus& bus, const MoveNode& moveNode);

    static int totalConnectionCount(const TTAMachine::Machine& mach);
    
    static std::set<const TTAMachine::Port*> findPossibleDestinationPorts(
        const TTAMachine::Machine& mach, const MoveNode& node);

    static std::set<const TTAMachine::Port*> findPossibleSourcePorts(
        const TTAMachine::Machine& mach, const MoveNode& node);

    static int canSourceWriteToAnyDestinationPort(
        const MoveNode& src, 
        std::set<const TTAMachine::Port*>& ports);

    static bool canAnyPortWriteToDestination(
        std::set<const TTAMachine::Port*>& ports,
        const MoveNode& dest);

    static std::set<const TTAMachine::Port*> findWritePorts(
        TTAMachine::Unit& rf);

    static std::set<const TTAMachine::Port*> findReadPorts(
        TTAMachine::Unit& rf);

    static bool hasConditionalMoves(const TTAMachine::Machine& mach);

    MachineConnectivityCheck();
    virtual ~MachineConnectivityCheck();

    static void addAnnotatedFUs(std::set<TCEString>& candidateFUs, const TTAProgram::Move& m, 
	TTAProgram::ProgramAnnotation::Id id);

    static std::pair<bool,bool> hasBothGuards(
        const TTAMachine::Bus* bus, 
        std::pair<TTAMachine::RegisterFile*,int> guardReg);

protected:
    MachineConnectivityCheck(const std::string& shortDesc_);
private:
    typedef std::pair<const TTAMachine::Port*,const TTAMachine::Port*> 
    PortPortPair;

    typedef std::pair<const TTAMachine::BaseRegisterFile*,
                      const TTAMachine::BaseRegisterFile*> RfRfPair;
    
    typedef std::pair<const TTAMachine::BaseRegisterFile*,
                      const TTAMachine::Port*> RfPortPair;
    
    typedef std::pair<const TTAMachine::Port*,
                      const TTAMachine::BaseRegisterFile*> PortRfPair;

    typedef std::map<PortPortPair,bool> PortPortBoolMap;
    typedef std::map<RfRfPair,bool> RfRfBoolMap;
    typedef std::map<RfPortPair,bool> RfPortBoolMap;
    typedef std::map<PortRfPair,bool> PortRfBoolMap;

    static PortPortBoolMap portPortCache_;
    static RfRfBoolMap rfRfCache_;
    static RfPortBoolMap rfPortCache_;
    static PortRfBoolMap portRfCache_;
};

#endif
