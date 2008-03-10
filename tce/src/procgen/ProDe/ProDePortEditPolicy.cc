/**
 * @file ProDePortEditPolicy.cc
 *
 * Definition of ProDePortEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <string>
#include <boost/format.hpp>

#include "ProDePortEditPolicy.hh"
#include "ConnectRequest.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "EditPart.hh"
#include "Socket.hh"
#include "Port.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "ProDeTextGenerator.hh"
#include "SetStatusTextCmd.hh"
#include "DeletePortCmd.hh"
#include "SocketPortConnCmd.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDePortEditPolicy::ProDePortEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDePortEditPolicy::~ProDePortEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 * @note ConnectCommand is not yet implemented, thus returns always NULL.
 */
ComponentCommand*
ProDePortEditPolicy::getCommand(Request* request) {
    Request::RequestType type = request->type();
    if (type == Request::CONNECT_REQUEST) {
	return createConnectCmd(request);
    } else if (type == Request::STATUS_REQUEST) {
        Port* port = dynamic_cast<Port*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_PORT);
        fmt % port->name();
        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;
    } else if (type == Request::DELETE_REQUEST) {
        DeletePortCmd* deleteCmd = new DeletePortCmd(host_);
        return deleteCmd;
    } else {
	return NULL;
    }
}

/**
 * Tells whether this EditPolicy is able to handle a certain type
 * of Request.
 *
 * @param request Request to be asked if it can be handled.
 * @return True if the Request can be handled, false otherwise.
 */
bool
ProDePortEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::CONNECT_REQUEST) {
        ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
        assert(cr != NULL);
        EditPart* part = cr->part();

        if (part == NULL) {
            // No selection.
            // Port can be selected for connecting, return true.
            return true;
        }

        Socket* socket = dynamic_cast<Socket*>(part->model());
        if (socket == NULL) {
            // Port can be connected only to sockets, return false.
            return false;
        }
        MachineTester tester(*(socket->machine()));
        Port* port = dynamic_cast<Port*>(host_->model());
        if (port->isConnectedTo(*socket) ||
            tester.canConnect(*socket, *port)) {
            return true;
        }
        return false;
    } else if (type == Request::STATUS_REQUEST) {
        return true;
    } else if (type == Request::DELETE_REQUEST) {
        return true;
    } else {
	return false;
    }
}

/**
 * Creates a command which connects the selected component to the
 * requested component.
 *
 * @param request ConnectionRequest with the target component for the
 *                connection.
 * @return Command for connecting the selected and requested components.
 */
ComponentCommand*
ProDePortEditPolicy::createConnectCmd(Request* request) {

    ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
    assert(cr != NULL);
    EditPart* part = cr->part();

    if (canHandle(request)) {
        Port* port = dynamic_cast<Port*>(host_->model());
        Socket* socket = dynamic_cast<Socket*>(part->model());
        if (port == NULL) {
            return NULL;
        }
        ComponentCommand* cmd = NULL;
        cmd = new SocketPortConnCmd(socket, port);
        return cmd;
    } else {
        return NULL;
    }
}
