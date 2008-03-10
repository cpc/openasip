/**
 * @file SocketBusConnFactory.cc
 *
 * Definition of SocketBusConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "SocketBusConnFactory.hh"
#include "EditPart.hh"
#include "ConnectionEditPart.hh"
#include "SocketBusConnFigure.hh"

using std::vector;

/**
 * The Constructor.
 */
SocketBusConnFactory::SocketBusConnFactory(): ConnectionFactory() {
}

/**
 * The Destructor.
 */
SocketBusConnFactory::~SocketBusConnFactory() {
}

/**
 * Specialized factory function that returns an EditPart corresponding
 * to a socket-bus connection.
 *
 * @note Source and target does not imply a direction.
 * @param socket Source (socket) of the connection.
 * @param segment Target (segment) of the connection.
 * @return An EditPart corresponding to a socket-bus connection.
 */
EditPart*
SocketBusConnFactory::createConnection(
    EditPart* socket,
    EditPart* segment) const {

    ConnectionEditPart* conn = new ConnectionEditPart();
    SocketBusConnFigure* connFig = new SocketBusConnFigure();
    conn->setFigure(connFig);
    conn->setSource(socket);
    conn->setTarget(segment);
    return conn;
}
