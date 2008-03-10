/**
 * @file ConnectRequest.cc
 *
 * Definition of ConnectRequest class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ConnectRequest.hh"

/**
 * The Constructor.
 *
 * @param part EditPart to connect the part with.
 */
ConnectRequest::ConnectRequest(
    EditPart* part):
    Request(Request::CONNECT_REQUEST),
    part_(part) {

}

/**
 * The Destructor.
 */
ConnectRequest::~ConnectRequest() {
}


/**
 * Returns the EditPart associated with the request.
 */
EditPart*
ConnectRequest::part() {
    return part_;
}
