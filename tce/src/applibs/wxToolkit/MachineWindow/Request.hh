/**
 * @file Request.hh
 *
 * Declaration of Request class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_REQUEST_HH
#define TTA_REQUEST_HH

/**
 * Used to communicate with EditPart objects.
 *
 * Type of the Request determines the function an EditPart should
 * perform.
 */
class Request {
public:
    /// Data type for determining the type of a Request.
    enum RequestType {
	NONE,            ///< No type.
        MODIFY_REQUEST,  ///< Modfify request.
	DELETE_REQUEST,  ///< Delete request.
	CONNECT_REQUEST, ///< Connect request.
	COPY_REQUEST,    ///< Copy request.
        STATUS_REQUEST,  ///< Status request.
        DETAILS_REQUEST  ///< Detailed info request.
    };

    explicit Request(RequestType type);
    virtual ~Request();

    void setType(RequestType type);
    RequestType type() const;

private:
    /// Assignment not allowed.
    Request& operator=(Request& old);
    /// Copying not allowed.
    Request(Request& old);

    /// Determines type of this Request.
    RequestType type_;
};

#include "Request.icc"

#endif
