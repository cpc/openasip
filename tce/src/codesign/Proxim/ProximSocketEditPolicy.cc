/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProximSocketEditPolicy.cc
 *
 * Implementation of ProximSocketEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
