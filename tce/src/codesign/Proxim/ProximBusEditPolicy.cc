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
 * @file ProximBusEditPolicy.cc
 *
 * Implementation of ProximBusEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximBusEditPolicy.hh"
#include "Request.hh"
#include "ProximComponentStatusCmd.hh"
#include "ProximBusDetailsCmd.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "EditPart.hh"
#include "ProximConstants.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximBusEditPolicy::ProximBusEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximBusEditPolicy::~ProximBusEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximBusEditPolicy::canHandle(Request* request) const {

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
ProximBusEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    const Bus* bus = dynamic_cast<const Bus*>(host_->model());
    if (bus == NULL) {
        const Segment* segment = dynamic_cast<const Segment*>(host_->model());
        assert(segment != NULL);
        bus = segment->parentBus();
    }

    assert(bus != NULL);

    if (type == Request::STATUS_REQUEST) {
        std::string status = ProximConstants::MACH_WIN_BUS_LABEL + bus->name();
        return new ProximComponentStatusCmd(status);
    } else if (type == Request::DETAILS_REQUEST) {
        return new ProximBusDetailsCmd(*bus);
    }
    return NULL;
}
