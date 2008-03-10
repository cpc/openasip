/**
 * @file SocketBusConnCmd.cc
 *
 * Definition of SocketBusConnCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "SocketBusConnCmd.hh"


using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param segment Segment to connect/disconnect.
 * @param socket Socket to connect/disconnect.
 */
SocketBusConnCmd::SocketBusConnCmd(Socket* socket, Segment* segment):
    ComponentCommand(),
    socket_(socket), segment_(segment) {
}


/**
 * The Destructor.
 */
SocketBusConnCmd::~SocketBusConnCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SocketBusConnCmd::Do() {

    if (socket_->isConnectedTo(*segment_)) {
        try {
            socket_->detachBus(*segment_);
        } catch (Exception& e) {
            return false;
        }
    } else {
        try {
            socket_->attachBus(*segment_);
        } catch (Exception& e) {
            return false;
        }
    }

    return true;
}
