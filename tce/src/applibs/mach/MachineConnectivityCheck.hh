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
 * @file MachineConnectivityCheck.hh
 *
 * Declaration of MachineConnectivityCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CONNECTIVITY_CHECK_HH
#define TTA_MACHINE_CONNECTIVITY_CHECK_HH

#include <set>
#include <map>

#include "MachineCheckResults.hh"
#include "MachineCheck.hh"


namespace TTAMachine {
    class Port;
    class Bus;
    class RegisterFile;
    class BaseRegisterFile;
    class Unit;
    class FunctionUnit;
}

namespace TTAProgram {
    class TerminalImmediate;
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
        const TTAMachine::Port& destinationPort);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::Port& destPort);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::BaseRegisterFile& destRF);

    static bool isConnected(
        const TTAMachine::Port& sourcePort,
        const TTAMachine::RegisterFile& destRF);

    static bool isConnected(
        const TTAMachine::BaseRegisterFile& sourceRF,
        const TTAMachine::FunctionUnit& destFU);

    static bool canWriteAllImmediates(TTAMachine::Port& destPort);

    static bool canTransportImmediate(
        const TTAProgram::TerminalImmediate& immediate,
        const TTAMachine::BaseRegisterFile& destRF);

    static bool canTransportImmediate(
        const TTAProgram::TerminalImmediate& immediate,
        const TTAMachine::Port& destinationPort);
    
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
        const TTAProgram::TerminalImmediate& source);

    MachineConnectivityCheck();
    virtual ~MachineConnectivityCheck();

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
