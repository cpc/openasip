/**
 * @file Segment.hh
 *
 * Declaration of Segment class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#ifndef TTA_SEGMENT_HH
#define TTA_SEGMENT_HH

#include <string>
#include <vector>

#include "MachinePart.hh"
#include "Exception.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class Connection;
class Bus;
class Socket;

/**
 * Represents a segment of a bus.
 */
class Segment : public SubComponent {
public:
    Segment(const std::string& name, Bus& parent)
        throw (ComponentAlreadyExists, InvalidName);
    Segment(const ObjectState* state, Bus& parent)
        throw (ComponentAlreadyExists, ObjectStateLoadingException);
    ~Segment();

    void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    std::string name() const;

    void attachSocket(Socket& socket)
        throw (IllegalRegistration, IllegalConnectivity);
    void detachSocket(Socket& socket)
        throw (InstanceNotFound);
    void detachAllSockets();

    bool isConnectedTo(const Socket& socket) const;
    const Connection& connection(const Socket& socket) const;
    Socket* connection(int index) const
        throw (OutOfRange);
    int connectionCount() const;

    Bus* parentBus() const;

    void moveBefore(Segment& segment)
        throw (IllegalRegistration);
    void moveAfter(Segment& segment)
        throw (IllegalRegistration);

    bool hasSourceSegment() const;
    bool hasDestinationSegment() const;

    Segment* sourceSegment() const;
    Segment* destinationSegment() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for Segment.
    static const std::string OSNAME_SEGMENT;
    /// ObjectState attribute key for segment name.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for destination segment name.
    static const std::string OSKEY_DESTINATION;

private:
    /// Vector of Connection pointers.
    typedef std::vector<const Connection*> ConnectionTable;

    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void removeConnection(const Connection* connection);

    /// Name of the segment.
    std::string name_;
    /// The parent bus.
    Bus* parent_;
    /// Source segment.
    Segment* sourceSegment_;
    /// Destination segment.
    Segment* destinationSegment_;
    /// Contains all the connections to sockets.
    ConnectionTable connections_;
};
}

#include "Segment.icc"

#endif
