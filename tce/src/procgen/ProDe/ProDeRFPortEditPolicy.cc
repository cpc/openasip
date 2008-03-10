/**
 * @file ProDeRFPortEditPolicy.cc
 *
 * Definition of ProDeRFPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProDeRFPortEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "EditPart.hh"
#include "ModifyRFPortCmd.hh"


/**
 * The Constructor.
 */
ProDeRFPortEditPolicy::ProDeRFPortEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeRFPortEditPolicy::~ProDeRFPortEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeRFPortEditPolicy::getCommand(Request* request) {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
	return new ModifyRFPortCmd(host_);
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
ProDeRFPortEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return true;
    } else {
	return false;
    }
}
