/**
 * @file ProximSocketEditPolicy.cc
 *
 * Implementation of ProximSocketEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximSocketEditPolicy.hh"
#include "Request.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximSocketDetailsCmd.hh"
#include "Socket.hh"
#include "EditPart.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximSocketEditPolicy::ProximSocketEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximSocketEditPolicy::~ProximSocketEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximSocketEditPolicy::canHandle(Request* request) const {

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
ProximSocketEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();
    
    const Socket* socket = dynamic_cast<Socket*>(host_->model());
    assert(socket != NULL);

    if (type == Request::STATUS_REQUEST) {
        std::string status = ProximConstants::MACH_WIN_SOCKET_LABEL +
            socket->name();
        return new ProximComponentStatusCmd(status);
    } else if (type == Request::DETAILS_REQUEST) {
        return new ProximSocketDetailsCmd(*socket);
    }
    return NULL;
}
