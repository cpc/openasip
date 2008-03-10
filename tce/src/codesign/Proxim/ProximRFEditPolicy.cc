/**
 * @file ProximRFEditPolicy.cc
 *
 * Implementation of ProximRFEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximRFEditPolicy.hh"
#include "Application.hh"
#include "Request.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "ProximShowRegistersCmd.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximRFDetailsCmd.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;
using std::string;

/**
 * The Constructor.
 */
ProximRFEditPolicy::ProximRFEditPolicy() :
    EditPolicy() {
}

/**
 * The Destructor.
 */
ProximRFEditPolicy::~ProximRFEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximRFEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
        type == Request::STATUS_REQUEST ||
        type == Request::DETAILS_REQUEST) {

        return true;
    }

    return false;
}


/**
 * Creates command corresponding to a request.
 *
 * @paran request Request to handle.
 * @return Null, if the request can't be handled.
 */ 
ComponentCommand*
ProximRFEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return new ProximShowRegistersCmd(host_);
    } else if (type == Request::STATUS_REQUEST) {

        const RegisterFile* rf = dynamic_cast<const RegisterFile*>(
            host_->model());

        assert(rf != NULL);
        string status = ProximConstants::MACH_WIN_RF_LABEL + rf->name();
        return new ProximComponentStatusCmd(status);

    } else if (type == Request::DETAILS_REQUEST) {

        const RegisterFile* rf = dynamic_cast<const RegisterFile*>(
            host_->model());

        assert(rf != NULL);
        return new ProximRFDetailsCmd(*rf);
    }
    return NULL;
}
