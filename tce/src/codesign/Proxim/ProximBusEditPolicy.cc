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
