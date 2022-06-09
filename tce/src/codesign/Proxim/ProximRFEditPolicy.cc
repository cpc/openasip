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
 * @file ProximRFEditPolicy.cc
 *
 * Implementation of ProximRFEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximRFEditPolicy.hh"
#include "Application.hh"
#include "Request.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "ProximShowRegistersCmd.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximRFDetailsCmd.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;
using std::string;

/**
 * The Constructor.
 */
ProximRFEditPolicy::ProximRFEditPolicy() :
    EditPolicy() {
}

/**
 * The Destructor.
 */
ProximRFEditPolicy::~ProximRFEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximRFEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST ||
        type == Request::STATUS_REQUEST ||
        type == Request::DETAILS_REQUEST) {

        return true;
    }

    return false;
}


/**
 * Creates command corresponding to a request.
 *
 * @paran request Request to handle.
 * @return Null, if the request can't be handled.
 */ 
ComponentCommand*
ProximRFEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return new ProximShowRegistersCmd(host_);
    } else if (type == Request::STATUS_REQUEST) {

        const RegisterFile* rf = dynamic_cast<const RegisterFile*>(
            host_->model());

        assert(rf != NULL);
        string status = ProximConstants::MACH_WIN_RF_LABEL + rf->name();
        return new ProximComponentStatusCmd(status);

    } else if (type == Request::DETAILS_REQUEST) {

        const RegisterFile* rf = dynamic_cast<const RegisterFile*>(
            host_->model());

        assert(rf != NULL);
        return new ProximRFDetailsCmd(*rf);
    }
    return NULL;
}
