/**
 * @file ProDeGCUEditPolicy.cc
 *
 * Definition of ProDeGCUEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <string>
#include <boost/format.hpp>

#include "ProDeGCUEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyGCUCmd.hh"
#include "DeleteGCUCmd.hh"
#include "CopyComponent.hh"
#include "EditPart.hh"
#include "ControlUnit.hh"
#include "ProDeTextGenerator.hh"
#include "SetStatusTextCmd.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeGCUEditPolicy::ProDeGCUEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeGCUEditPolicy::~ProDeGCUEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeGCUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        ModifyGCUCmd* modifyCmd = new ModifyGCUCmd(host_);
        return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
        DeleteGCUCmd* deleteCmd = new DeleteGCUCmd(host_);
        return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
        CopyComponent* copyCmd = new CopyComponent(host_);
        return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {
        ControlUnit* gcu = dynamic_cast<ControlUnit*>(host_->model());
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format fmt = generator->text(ProDeTextGenerator::STATUS_GCU);
        fmt % gcu->name();
        SetStatusTextCmd* statusCmd = new SetStatusTextCmd(fmt.str());
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
ProDeGCUEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::COPY_REQUEST) {
	return true;
    } else {
	return false;
    }
}
