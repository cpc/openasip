/**
 * @file ProximFUEditPolicy.cc
 *
 * Implementation of ProximFUEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximFUEditPolicy.hh"
#include "Request.hh"
#include "ProximShowPortsCmd.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximFUDetailsCmd.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximFUEditPolicy::ProximFUEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximFUEditPolicy::~ProximFUEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximFUEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
        type == Request::STATUS_REQUEST ||
        type == Request::DETAILS_REQUEST) {

        return true;
    }

    return false;
}


/**
 * Returns command correspoding to the request type.
 *
 * @param request Request to handle.
 * @return NULL, if the request can't be handled.
 */
ComponentCommand*
ProximFUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::MODIFY_REQUEST) {
        return new ProximShowPortsCmd(host_);
    } else if (type == Request::STATUS_REQUEST) {
        const FunctionUnit* fu = dynamic_cast<const FunctionUnit*>(
            host_->model());
        assert(fu != NULL);
        std::string status = ProximConstants::MACH_WIN_FU_LABEL + fu->name();
        return new ProximComponentStatusCmd(status);
    } else if (type == Request::DETAILS_REQUEST) {
        const FunctionUnit* fu = dynamic_cast<const FunctionUnit*>(
            host_->model());
        assert(fu != NULL);
        return new ProximFUDetailsCmd(*fu);
    }

    return NULL;
}
