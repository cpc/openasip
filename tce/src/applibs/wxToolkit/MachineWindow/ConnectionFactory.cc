/**
 * @file ConnectionFactory.cc
 *
 * Definition of ConnectionFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
