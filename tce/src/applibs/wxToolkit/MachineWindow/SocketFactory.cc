/**
 * @file SocketFactory.cc
 *
 * Definition of SocketFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include "Machine.hh"
#include "SocketFactory.hh"
#include "Socket.hh"
#include "EditPart.hh"
#include "InputSocketFigure.hh"
#include "OutputSocketFigure.hh"
#include "UnknownSocketFigure.hh"
#include "EditPolicyFactory.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
SocketFactory::SocketFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {
}

/**
 * The Destructor.
 */
SocketFactory::~SocketFactory() {
}

/**
 * Returns an EditPart corresponding to a socket.
 *
 * @param component Socket of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the Socket class.
 */
EditPart*
SocketFactory::createEditPart(MachinePart* component) {

    EditPart* socketEditPart = EditPartFactory::checkCache(component);

    if (socketEditPart != NULL) {
	return socketEditPart;
    }

    Socket* socket = dynamic_cast<Socket*>(component);
    
    if (socket != NULL) {

	socketEditPart = new EditPart();
	socketEditPart->setModel(socket);

	Figure* fig = NULL;

	if (socket->direction() == Socket::INPUT) {
	    fig = new InputSocketFigure();
	} else if (socket->direction() == Socket::OUTPUT) {
	    fig = new OutputSocketFigure();
	} else {
	    fig = new UnknownSocketFigure();
	}
	socketEditPart->setFigure(fig);

	socketEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createSocketEditPolicy();
	if (editPolicy != NULL) {
	    socketEditPart->installEditPolicy(editPolicy);
	}

	EditPartFactory::writeToCache(socketEditPart);
	return socketEditPart;

    } else {
	return NULL;
    }
}
