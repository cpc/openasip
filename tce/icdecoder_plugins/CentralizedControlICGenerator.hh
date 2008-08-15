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
 * @file CentralizedControlICGenerator.hh
 *
 * Declaration of CentralizedControlICGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CENTRALIZED_CONTROl_IC_GENERATOR_HH
#define TTA_CENTRALIZED_CONTROL_IC_GENERATOR_HH

#include <string>
#include <map>

#include "Exception.hh"

namespace ProGe {
    class NetlistPort;
}

namespace TTAMachine {
    class Socket;
    class Bus;
    class Port;
    class Segment;
}

class CentralizedControlICGenerator {
public:
    CentralizedControlICGenerator();
    virtual ~CentralizedControlICGenerator();

    ProGe::NetlistPort& simmDataPort(const std::string& busName) const
        throw (InstanceNotFound);
    ProGe::NetlistPort& simmCntrlPort(const std::string& busName) const
        throw (InstanceNotFound);
    ProGe::NetlistPort& busCntrlPortOfSocket(
        const std::string& socketName) const
        throw (InstanceNotFound);
    ProGe::NetlistPort& dataCntrlPortOfSocket(
        const std::string& socketName) const
        throw (InstanceNotFound);

    virtual int outputSocketCntrlPinForSegment(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const
        throw (NotAvailable) = 0;

    virtual int outputSocketDataControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port) const
        throw (NotAvailable) = 0;
    
    virtual int inputSocketControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const
        throw (NotAvailable) = 0;

protected:
    void mapSImmDataPort(const std::string& busName, ProGe::NetlistPort& port);
    void mapSImmCntrlPort(const std::string& busName, ProGe::NetlistPort& port);
    void mapBusCntrlPortOfSocket(
        const std::string& socketName,
        ProGe::NetlistPort& port);
    void mapDataCntrlPortOfSocket(
        const std::string& socketName,
        ProGe::NetlistPort& port);

private:
    typedef std::map<std::string, ProGe::NetlistPort*> NetlistPortMap;

    /// Maps the short immediate data ports for buses.
    NetlistPortMap simmDataPortMap_;
    /// Maps the short immediate control ports for buses.
    NetlistPortMap simmCntrlPortMap_;
    /// Maps the data ports of sockets.
    NetlistPortMap socketDataPortMap_;
    /// Maps the bus control ports of sockets.
    NetlistPortMap busCntrlPortMap_;
    /// Maps the data control ports of sockets.
    NetlistPortMap dataCntrlPortMap_;
};

#endif
