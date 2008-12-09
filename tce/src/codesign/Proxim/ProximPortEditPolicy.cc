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
 * @file ProximPortEditPolicy.cc
 *
 * Implementation of ProximPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximPortEditPolicy.hh"
#include "Request.hh"
#include "ProximComponentStatusCmd.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "EditPart.hh"
#include "ProximFUPortDetailsCmd.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ProximPortEditPolicy::ProximPortEditPolicy() :
    EditPolicy() {
}


/**
 * The Destructor.
 */
ProximPortEditPolicy::~ProximPortEditPolicy() {
}

/**
 * Tells whether this EditPolicy can handle request of the given type.
 *
 * @param Request to be handled.
 * @return True, if the editpolicy can handle give request type.
 */
bool
ProximPortEditPolicy::canHandle(Request* request) const {

    Request::RequestType type = request->type();
    if (type == Request::STATUS_REQUEST) {
        return true;
    } else if (type == Request::DETAILS_REQUEST) {
        // Details only available for function unit ports.
        const FUPort* fuPort =
            dynamic_cast<const FUPort*>(host_->model());
        
        if (fuPort != NULL) {
            return true;
        }
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
ProximPortEditPolicy::getCommand(Request* request) {

    Request::RequestType type = request->type();

    if (type == Request::STATUS_REQUEST) {
        const Port* port = dynamic_cast<Port*>(host_->model());
        assert(port != NULL);
        std::string name = port->name();
        return new ProximComponentStatusCmd("Port: " + name);
    } else if (type == Request::DETAILS_REQUEST) {
        const FUPort* fuPort =
            dynamic_cast<const FUPort*>(host_->model());
        if (fuPort == NULL) {
            return NULL;
        } else {
            return new ProximFUPortDetailsCmd(*fuPort);
        }
    }

    return NULL;
}
