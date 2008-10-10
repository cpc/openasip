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
