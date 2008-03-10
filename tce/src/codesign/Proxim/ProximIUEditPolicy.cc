/**
 * @file ProximIUEditPolicy.cc
 *
 * Implementation of ProximIUEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximIUEditPolicy.hh"
#include "Application.hh"
#include "Request.hh"
#include "ImmediateUnit.hh"
#include "EditPart.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximIUDetailsCmd.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;
using std::string;

/**
 * The Constructor.
 */
ProximIUEditPolicy::ProximIUEditPolicy() :
    EditPolicy() {
}

/**
 * The Destructor.
 */
ProximIUEditPolicy::~ProximIUEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximIUEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST ||
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
ProximIUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST) {

        const ImmediateUnit* iu = dynamic_cast<const ImmediateUnit*>(
            host_->model());

        assert(iu != NULL);
        string status = ProximConstants::MACH_WIN_IU_LABEL + iu->name();
        return new ProximComponentStatusCmd(status);

    } else if (type == Request::DETAILS_REQUEST) {

        const ImmediateUnit* iu = dynamic_cast<const ImmediateUnit*>(
            host_->model());

        assert(iu != NULL);
        return new ProximIUDetailsCmd(*iu);
    }
    return NULL;
}
