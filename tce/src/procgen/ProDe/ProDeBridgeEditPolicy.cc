/**
 * @file ProDeBridgeEditPolicy.cc
 *
 * Definition of ProDeBridgeEditPolicy class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme@tut.fi)
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
