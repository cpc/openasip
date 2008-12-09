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
 * @file ProDeSegmentEditPolicy.cc
 *
 * Definition of ProDeSegmentEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>
#include "ProDeSegmentEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ConnectRequest.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "MachineTester.hh"
#include "SocketBusConnCmd.hh"
#include "ModifyBusCmd.hh"
#include "Bus.hh"
#include "ProDeTextGenerator.hh"
#include "DeleteSegmentCmd.hh"
#include "SetStatusTextCmd.hh"

using boost::format;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeSegmentEditPolicy::ProDeSegmentEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeSegmentEditPolicy::~ProDeSegmentEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 * @note ConnectCommand is not yet implemented, thus returns always NULL.
 */
ComponentCommand*
ProDeSegmentEditPolicy::getCommand(Request* request) {


    Request::RequestType type = request->type();
    if (type == Request::CONNECT_REQUEST) {
        return createConnectCmd(request);
    } else if (type == Request::MODIFY_REQUEST) {
        return new ModifyBusCmd(host_->parent());
    } else if (type == Request::DELETE_REQUEST) {
        return new DeleteSegmentCmd(host_);
    } else if (type == Request::STATUS_REQUEST) {
        // Editing segments is unsupported feature => return empty command
        return NULL;
        Segment* segment = dynamic_cast<Segment*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_SEGMENT);
        fmt % segment->name();
        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;
    }
    return NULL;
}

/**
 * Tells whether this EditPolicy is able to handle a certain type
 * of Request.
 *
 * @param request Request to be asked if it can be handled.
 * @return True if the Request can be handled, false otherwise.
 */
bool
ProDeSegmentEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::CONNECT_REQUEST) {
        ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
        assert(cr != NULL);
        EditPart* part = cr->part();

        if (part == NULL) {
            // No selection.
            // Segment can be selected for connecting, return true.
            return true;
        }
        Socket* socket = dynamic_cast<Socket*>(part->model());
        if (socket == NULL) {
            // Segment can be connected only to sockets, return false.
            return false;
        }
        MachineTester tester(*(socket->machine()));
        Segment* segment = dynamic_cast<Segment*>(host_->model());
        if (socket->isConnectedTo(*segment) ||
            tester.canConnect(*socket, *segment)) {
            return true;
        }
        return false;
    } else if (type == Request::STATUS_REQUEST) {
        return true;
    } else if (type == Request::DELETE_REQUEST) {
        return true;
    } else if (type == Request::MODIFY_REQUEST) {
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
ProDeSegmentEditPolicy::createConnectCmd(Request* request) {

    ConnectRequest* cr = dynamic_cast<ConnectRequest*>(request);
    assert(cr != NULL);
    EditPart* part = cr->part();

    if (canHandle(request)) {
        Segment* segment = dynamic_cast<Segment*>(host_->model());
        Socket* socket = dynamic_cast<Socket*>(part->model());
        if (socket == NULL) {
            return NULL;
        }
        ComponentCommand* cmd = NULL;
        cmd = new SocketBusConnCmd(socket, segment);
        return cmd;
    } else {
        return NULL;
    }
}
