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
 * @file ProximRFEditPolicy.cc
 *
 * Implementation of ProximRFEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
