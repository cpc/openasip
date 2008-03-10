/**
 * @file ProximPortEditPolicy.cc
 *
 * Implementation of ProximPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximPortEditPolicy.hh"
#include "Request.hh"
#include "ProximComponentStatusCmd.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "EditPart.hh"
#include "ProximFUPortDetailsCmd.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximPortEditPolicy::ProximPortEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximPortEditPolicy::~ProximPortEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximPortEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST) {
        return true;
    } else if (type == Request::DETAILS_REQUEST) {
        // Details only available for function unit ports.
        const FUPort* fuPort =
            dynamic_cast<const FUPort*>(host_->model());
        
        if (fuPort != NULL) {
            return true;
        }
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
ProximPortEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::STATUS_REQUEST) {
        const Port* port = dynamic_cast<Port*>(host_->model());
        assert(port != NULL);
        std::string name = port->name();
        return new ProximComponentStatusCmd("Port: " + name);
    } else if (type == Request::DETAILS_REQUEST) {
        const FUPort* fuPort =
            dynamic_cast<const FUPort*>(host_->model());
        if (fuPort == NULL) {
            return NULL;
        } else {
            return new ProximFUPortDetailsCmd(*fuPort);
        }
    }

    return NULL;
}
