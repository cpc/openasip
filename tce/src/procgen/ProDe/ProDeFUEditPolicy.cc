/**
 * @file ProDeFUEditPolicy.cc
 *
 * Definition of ProDeFUEditPolicy class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 20 2004 by vpj, jn, am
 */

#include <boost/format.hpp>
#include "ProDeFUEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "ModifyFUCmd.hh"
#include "DeleteFUCmd.hh"
#include "CopyComponent.hh"
#include "SetStatusTextCmd.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ProDeFUEditPolicy::ProDeFUEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeFUEditPolicy::~ProDeFUEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeFUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
	ModifyFUCmd* modifyCmd = new ModifyFUCmd(host_);
	return modifyCmd;

    } else if (type == Request::DELETE_REQUEST) {
	DeleteFUCmd* deleteCmd = new DeleteFUCmd(host_);
	return deleteCmd;

    } else if (type == Request::COPY_REQUEST) {
	CopyComponent* copyCmd = new CopyComponent(host_);
	return copyCmd;

    } else if (type == Request::STATUS_REQUEST) {

        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        FunctionUnit* fu = dynamic_cast<FunctionUnit*>(host_->model());
        format fmt = generator->text(ProDeTextGenerator::STATUS_FUNCTION_UNIT);
        fmt % fu->name();
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
ProDeFUEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
	type == Request::DELETE_REQUEST ||
        type == Request::COPY_REQUEST ||
        type == Request::STATUS_REQUEST) {
	return true;
    } else {
	return false;
    }
}
