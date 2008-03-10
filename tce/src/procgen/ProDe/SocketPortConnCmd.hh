/**
 * @file SocketPortConnCmd.hh
 *
 * Declaration of SocketPortConnCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_PORT_CONN_CMD_HH
#define TTA_SOCKET_PORT_CONN_CMD_HH

#include "Port.hh"
#include "Socket.hh"
#include "ComponentCommand.hh"

namespace TTAMachine {
    class Socket;
    class Port;
}

/**
 * Command for connecting/disconencting ports to sockets.
 */
class SocketPortConnCmd: public ComponentCommand {
public:
    SocketPortConnCmd(TTAMachine::Socket* socket, TTAMachine::Port* port);
    virtual ~SocketPortConnCmd();
    virtual bool Do();

private:
    /// Socket to connect.
    TTAMachine::Socket* socket_;
    /// Port to connect.
    TTAMachine::Port* port_;
};
#endif
