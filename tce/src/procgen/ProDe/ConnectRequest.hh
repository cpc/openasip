/**
 * @file ConnectRequest.hh
 *
 * Declaration of ConnectRequest class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONNECT_REQUEST_HH
#define TTA_CONNECT_REQUEST_HH

#include "Request.hh"
#include "EditPart.hh"

/**
 * Request which is used to request EditPart to create connection between
 * itself and another EditPart.
 */
class ConnectRequest : public Request {
public:
    ConnectRequest(EditPart* part);
    virtual ~ConnectRequest();
    EditPart* part();
private:
    /// Assignment not allowed.
    ConnectRequest& operator=(Request& old);
    /// Copying not allowed.
    ConnectRequest(Request& old);
    /// EditPart associated with the request.
    EditPart* part_;
};
#endif
