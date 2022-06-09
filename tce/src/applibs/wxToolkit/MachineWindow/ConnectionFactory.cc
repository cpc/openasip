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
 * @file ConnectionFactory.cc
 *
 * Definition of ConnectionFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "ConnectionFactory.hh"
#include "EditPart.hh"
#include "ConnectionEditPart.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
ConnectionFactory::ConnectionFactory() {
}

/**
 * The Destructor.
 */
ConnectionFactory::~ConnectionFactory() {
}

/**
 * Specialized factory function that returns an EditPart corresponding
 * to a connection.
 *
 * Doesn't create a figure.
 *
 * @param source Source of the connection.
 * @param target Target of the connection.
 * @return An EditPart corresponding to a connection.
 */
EditPart*
ConnectionFactory::createConnection(
    EditPart* source,
    EditPart* target) const {

    ConnectionEditPart* conn = new ConnectionEditPart();
    conn->setSource(source);
    conn->setTarget(target);
    return conn;
}

/**
 * There's no object in Machine that corresponds to a connection.
 *
 * @return NULL always.
 */
EditPart*
ConnectionFactory::createEditPart(MachinePart*) {
    return NULL;
}
