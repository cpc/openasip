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
 * @file SocketPortConnFactory.cc
 *
 * Definition of SocketPortConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "SocketPortConnFactory.hh"
#include "EditPart.hh"
#include "ConnectionEditPart.hh"
#include "SocketPortConnFigure.hh"

using std::vector;

/**
 * The Constructor.
 */
SocketPortConnFactory::SocketPortConnFactory(): ConnectionFactory() {
}

/**
 * The Destructor.
 */
SocketPortConnFactory::~SocketPortConnFactory() {
}

/**
 * Specialized factory function that returns an EditPart corresponding
 * to a socket-port connection.
 *
 * @note Source and target distinction does not imply the direction of
 *       the connection.
 * @param port Source (port) of the connection.
 * @param socket Target (socket) of the connection.
 * @return An EditPart corresponding to a socket-port connection.
 */
EditPart*
SocketPortConnFactory::createConnection(
    EditPart* port,
    EditPart* socket) const {

    ConnectionEditPart* conn = new ConnectionEditPart();
    SocketPortConnFigure* connFig = new SocketPortConnFigure();
    conn->setFigure(connFig);
    conn->setSource(port);
    conn->setTarget(socket);
    return conn;
}
