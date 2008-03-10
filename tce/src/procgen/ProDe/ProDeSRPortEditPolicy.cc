/**
 * @file ProDeSRPortEditPolicy.cc
 *
 * Definition of ProDeSRPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProDeSRPortEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "EditPart.hh"
#include "ModifySRPortCmd.hh"

/**
 * The Constructor.
 */
ProDeSRPortEditPolicy::ProDeSRPortEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeSRPortEditPolicy::~ProDeSRPortEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeSRPortEditPolicy::getCommand(Request* request) {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
	return new ModifySRPortCmd(host_);
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
ProDeSRPortEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return true;
    } else {
	return false;
    }
}
