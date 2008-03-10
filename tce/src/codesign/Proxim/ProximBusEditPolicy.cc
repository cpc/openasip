/**
 * @file ProximBusEditPolicy.cc
 *
 * Implementation of ProximBusEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximBusEditPolicy.hh"
#include "Request.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximBusDetailsCmd.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "EditPart.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximBusEditPolicy::ProximBusEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximBusEditPolicy::~ProximBusEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximBusEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST ||
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
ProximBusEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    const Bus* bus = dynamic_cast<const Bus*>(host_->model());
    if (bus == NULL) {
        const Segment* segment = dynamic_cast<const Segment*>(host_->model());
        assert(segment != NULL);
        bus = segment->parentBus();
    }

    assert(bus != NULL);

    if (type == Request::STATUS_REQUEST) {
        std::string status = ProximConstants::MACH_WIN_BUS_LABEL + bus->name();
        return new ProximComponentStatusCmd(status);
    } else if (type == Request::DETAILS_REQUEST) {
        return new ProximBusDetailsCmd(*bus);
    }
    return NULL;
}
