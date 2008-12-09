/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file Request.hh
 *
 * Declaration of Request class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
