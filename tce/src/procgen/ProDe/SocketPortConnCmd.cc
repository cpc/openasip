/**
 * @file SocketPortConnCmd.cc
 *
 * Definition of SocketPortConnCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "SocketPortConnCmd.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param port Port to connect/disconnect.
 * @param socket Socket to connect/disconnect.
 */
SocketPortConnCmd::SocketPortConnCmd(Socket* socket, Port* port):
    ComponentCommand(),
    socket_(socket), port_(port) {
}


/**
 * The Destructor.
 */
SocketPortConnCmd::~SocketPortConnCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SocketPortConnCmd::Do() {

    if (port_->isConnectedTo(*socket_)) {
        try {
            port_->detachSocket(*socket_);
        } catch (Exception& e) {
            return false;
        }
    } else {
        try {
            port_->attachSocket(*socket_);
        } catch (Exception& e) {
            return false;
        }
    }

    return true;
}
