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
 * @file Segment.hh
 *
 * Declaration of Segment class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#ifndef TTA_SEGMENT_HH
#define TTA_SEGMENT_HH

#include <string>
#include <vector>

#include "MachinePart.hh"
#include "Exception.hh"

class ObjectState;

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
