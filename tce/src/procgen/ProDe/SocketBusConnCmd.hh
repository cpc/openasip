/**
 * @file SocketBusConnCmd.hh
 *
 * Declaration of SocketBusConnCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_BUS_CONN_CMD_HH
#define TTA_SOCKET_BUS_CONN_CMD_HH

#include "Segment.hh"
#include "Socket.hh"
#include "ComponentCommand.hh"

/**
 * Command for connecting/disconencting sockets to bus segments.
 */
class SocketBusConnCmd: public ComponentCommand {
public:
    SocketBusConnCmd(
        TTAMachine::Socket* socket,
        TTAMachine::Segment* segment);
    virtual ~SocketBusConnCmd();
    virtual bool Do();

private:
    /// Socket to connect.
    TTAMachine::Socket* socket_;
    /// Bus segment to connect.
    TTAMachine::Segment* segment_;
};
#endif
