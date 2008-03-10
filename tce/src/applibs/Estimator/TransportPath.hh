/**
 * @file TransportPath.hh
 *
 * Declaration of TransportPath class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRANSPORT_PATH_HH
#define TTA_TRANSPORT_PATH_HH

#include <list>

namespace TTAMachine {
    class Port;
    class Socket;
    class Bus;
}

namespace CostEstimator {
/**
 * Represents a single transport path in a machine.
 *
 * The class is simply a storage for source port, source socket, bus, 
 * destination socket, destination port components that make a path for
 * data transports in a machine. 
 */
class TransportPath {
public:
    TransportPath(
        const TTAMachine::Port& sourcePort, 
        const TTAMachine::Socket& sourceSocket,
        const TTAMachine::Bus& bus,
        const TTAMachine::Socket& destinationSocket,
        const TTAMachine::Port& destinationPort);
    virtual ~TransportPath();

    const TTAMachine::Port& sourcePort() const;
    const TTAMachine::Socket& sourceSocket() const;
    const TTAMachine::Bus& bus() const;
    const TTAMachine::Socket& destinationSocket() const;
    const TTAMachine::Port& destinationPort() const;

private:
    const TTAMachine::Port& sourcePort_;
    const TTAMachine::Socket& sourceSocket_;
    const TTAMachine::Bus& bus_;
    const TTAMachine::Socket& destinationSocket_;
    const TTAMachine::Port& destinationPort_;    
};

typedef std::list<TransportPath> TransportPathList;
}

#endif
