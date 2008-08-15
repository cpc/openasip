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
 * @file ProximIUEditPolicy.cc
 *
 * Implementation of ProximIUEditPolicy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximIUEditPolicy.hh"
#include "Application.hh"
#include "Request.hh"
#include "ImmediateUnit.hh"
#include "EditPart.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximIUDetailsCmd.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;
using std::string;

/**
 * The Constructor.
 */
ProximIUEditPolicy::ProximIUEditPolicy() :
    EditPolicy() {
}

/**
 * The Destructor.
 */
ProximIUEditPolicy::~ProximIUEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximIUEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST ||
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
ProximIUEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST) {

        const ImmediateUnit* iu = dynamic_cast<const ImmediateUnit*>(
            host_->model());

        assert(iu != NULL);
        string status = ProximConstants::MACH_WIN_IU_LABEL + iu->name();
        return new ProximComponentStatusCmd(status);

    } else if (type == Request::DETAILS_REQUEST) {

        const ImmediateUnit* iu = dynamic_cast<const ImmediateUnit*>(
            host_->model());

        assert(iu != NULL);
        return new ProximIUDetailsCmd(*iu);
    }
    return NULL;
}
