/**
 * @file ProDeIUPortEditPolicy.cc
 *
 * Definition of ProDeIUPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProDeIUPortEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "EditPart.hh"
#include "ModifyIUPortCmd.hh"


/**
 * The Constructor.
 */
ProDeIUPortEditPolicy::ProDeIUPortEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeIUPortEditPolicy::~ProDeIUPortEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeIUPortEditPolicy::getCommand(Request* request) {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
	return new ModifyIUPortCmd(host_);
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
ProDeIUPortEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return true;
    } else {
	return false;
    }
}
