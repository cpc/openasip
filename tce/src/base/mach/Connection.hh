/**
 * @file Connection.hh
 *
 * Declaration of Connection class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#ifndef TTA_CONNECTION_HH
#define TTA_CONNECTION_HH

#include "ObjectState.hh"

namespace TTAMachine {

class Segment;
class Socket;

/**
 * Joins Bus and Socket objects if they are connected in the machine.
 */
class Connection {
public:
    Connection(Socket& socket, Segment& bus);
    ~Connection();
    Segment* bus() const;
    Socket* socket() const;

    ObjectState* saveState() const;

    /// ObjectState name for Connection.
    static const std::string OSNAME_CONNECTION;
    /// ObjectState attribute key for socket name.
    static const std::string OSKEY_SOCKET;
    /// ObjectState attribute key for bus name.
    static const std::string OSKEY_BUS;
    /// ObjectState attribute key for segment name.
    static const std::string OSKEY_SEGMENT;

private:
    /// Socket which is connected to the bus.
    Socket* socket_;
    /// Bus which is connected to the socket.
    Segment* bus_;
};
}

#include "Connection.icc"

#endif
