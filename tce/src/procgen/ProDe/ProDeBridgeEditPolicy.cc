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
 * @file ProDeBridgeEditPolicy.cc
 *
 * Definition of ProDeBridgeEditPolicy class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>

#include "ProDeBridgeEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyBridgeCmd.hh"
#include "DeleteBridgeCmd.hh"
#include "Bridge.hh"
#include "EditPart.hh"
#include "ProDeTextGenerator.hh"
#include "SetStatusTextCmd.hh"

using boost::format;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeBridgeEditPolicy::ProDeBridgeEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeBridgeEditPolicy::~ProDeBridgeEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeBridgeEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifyBridgeCmd* modifyCmd = new ModifyBridgeCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteBridgeCmd* deleteCmd = new DeleteBridgeCmd(host_);
        return deleteCmd;

    } else if (type == Request::STATUS_REQUEST) {
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        Bridge* bridge = dynamic_cast<Bridge*>(host_->model());
        format fmt = generator->text(ProDeTextGenerator::STATUS_BRIDGE);
        fmt % bridge->name();
        SetStatusTextCmd*  statusCmd = new SetStatusTextCmd(fmt.str());
        return statusCmd;

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
ProDeBridgeEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::STATUS_REQUEST) {
	return true;
    } else {
	return false;
    }
}
