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
 * @file ProDeSocketEditPolicy.cc
 *
 * Definition of ProDeSocketEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>

#include "ProDeSocketEditPolicy.hh"
#include "Request.hh"
#include "ConnectRequest.hh"
#include "ComponentCommand.hh"
#include "ModifySocketCmd.hh"
#include "DeleteSocketCmd.hh"
#include "CopyComponent.hh"
#include "SetStatusTextCmd.hh"
#include "ProDeTextGenerator.hh"
#include "EditPart.hh"
#include "Socket.hh"
#include "Port.hh"
#include "Segment.hh"
#include "Unit.hh"
#include "Bus.hh"
#include "MachineTester.hh"
#include "SocketPortConnCmd.hh"
#include "SocketBusConnCmd.hh"

using boost::format;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeSocketEditPolicy::ProDeSocketEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeSocketEditPolicy::~ProDeSocketEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeSocketEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifySocketCmd* modifyCmd = new ModifySocketCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteSocketCmd* deleteCmd = new DeleteSocketCmd(host_);
        return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
        CopyComponent* copyCmd = new CopyComponent(host_);
        return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {
        Socket* socket = dynamic_cast<Socket*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_SOCKET);
        fmt % socket->name();
        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;
    } if (type == Request::CONNECT_REQUEST) {
        return createConnectCmd(request);
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
ProDeSocketEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::COPY_REQUEST ||
        type == Request::STATUS_REQUEST) {
	return true;
    } else if (type == Request::CONNECT_REQUEST) {
        ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
        assert(cr != NULL);
        EditPart* part = cr->part();

        if (part == NULL) {
            // No selection.
            // Socket can be selected for connecting, return true.
            return true;
        }

        Port* port = dynamic_cast<Port*>(part->model());
        Segment* segment = dynamic_cast<Segment*>(part->model());
        if (port == NULL && segment == NULL) {
            // Socket can be connected only to ports and segments.
            return false;
        }
        MachineTester* tester = NULL;
        if (port != NULL) {
            tester = new MachineTester(*(port->parentUnit()->machine()));
        } else {
            tester = new MachineTester(*(segment->parentBus()->machine()));
        }
        Socket* socket = dynamic_cast<Socket*>(host_->model());
        if ((port != NULL && (
                 port->isConnectedTo(*socket) ||
                 tester->canConnect(*socket, *port))) ||
            (segment != NULL && (
                socket->isConnectedTo(*segment) ||
                tester->canConnect(*socket, *segment)))) {

            delete tester;
            return true;
        }
        delete tester;
        return false;
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
ProDeSocketEditPolicy::createConnectCmd(Request* request) {

    ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
    assert(cr != NULL);
    EditPart* part = cr->part();

    if (canHandle(request)) {
        Port* port = dynamic_cast<Port*>(part->model());
        Segment* segment = dynamic_cast<Segment*>(part->model());
        Socket* socket = dynamic_cast<Socket*>(host_->model());
        if (port == NULL && segment == NULL) {
            return NULL;
        }
        ComponentCommand* cmd = NULL;
        if (port != NULL) {
            cmd = new SocketPortConnCmd(socket, port);
        } else {
            cmd = new SocketBusConnCmd(socket, segment);
        }
        return cmd;
    } else {
        return NULL;
    }
}
