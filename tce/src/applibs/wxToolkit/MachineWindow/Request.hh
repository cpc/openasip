/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
