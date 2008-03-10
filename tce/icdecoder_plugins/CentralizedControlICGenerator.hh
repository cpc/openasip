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
