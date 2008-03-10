/**
 * @file SocketPortConnFactory.cc
 *
 * Definition of SocketPortConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
