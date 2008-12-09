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
 * @file ProDeSRPortEditPolicy.cc
 *
 * Definition of ProDeSRPortEditPolicy class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProDeSRPortEditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"
#include "EditPart.hh"
#include "ModifySRPortCmd.hh"

/**
 * The Constructor.
 */
ProDeSRPortEditPolicy::ProDeSRPortEditPolicy(): EditPolicy() {
}

/**
 * The Destructor.
 */
ProDeSRPortEditPolicy::~ProDeSRPortEditPolicy() {
}

/**
 * Returns the Command corresponding to the type of the Request.
 *
 * @param request Request to be handled.
 * @return NULL if the Request cannot be handled.
 */
ComponentCommand*
ProDeSRPortEditPolicy::getCommand(Request* request) {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
	return new ModifySRPortCmd(host_);
    }
    return NULL;
}

/**
 * Tells whether this EditPolicy is able to handle a certain type
 * of Request.
 *
 * @param request Request to be asked if it can be handled.
 * @return True if the Request can be handled, false otherwise.
 */
bool
ProDeSRPortEditPolicy::canHandle(Request* request) const {
    Request::RequestType type = request->type();
    if (type == Request::MODIFY_REQUEST) {
        return true;
    } else {
	return false;
    }
}
