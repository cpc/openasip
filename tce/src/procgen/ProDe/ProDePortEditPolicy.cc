/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProDePortEditPolicy.cc
 *
 * Definition of ProDePortEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
