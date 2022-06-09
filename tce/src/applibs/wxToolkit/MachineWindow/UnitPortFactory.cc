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
 * @file UnitPortFactory.cc
 *
 * Definition of UnitPortFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "Application.hh"
#include "UnitPortFactory.hh"
#include "FUPort.hh"
#include "Port.hh"
#include "EditPart.hh"
#include "UnitPortFigure.hh"
#include "SocketFactory.hh"
#include "SocketPortConnFactory.hh"
#include "Socket.hh"
#include "EditPolicyFactory.hh"
#include "TriggeringPortFigure.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
UnitPortFactory::UnitPortFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new SocketFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
UnitPortFactory::~UnitPortFactory() {
}

/**
 * Returns an EditPart corresponding to a port.
 *
 * @param component Port of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the Port class.
 */
EditPart*
UnitPortFactory::createEditPart(MachinePart* component) {

    Port* port = dynamic_cast<Port*>(component);
    EditPart* portEditPart = NULL;

    if (port != NULL) {
	
	portEditPart = new EditPart();
	portEditPart->setModel(port);

        Figure* fig = NULL;

        FUPort* fuPort = dynamic_cast<FUPort*>(component);
        if (fuPort != NULL && fuPort->isTriggering()) {
            fig = new TriggeringPortFigure(port->name(), port->width());
        } else {
            fig = new UnitPortFigure(port->name(), port->width());
        }

	portEditPart->setFigure(fig);

	if (port->inputSocket() != NULL) {
	    addSocket(portEditPart, port->inputSocket());
	}

	if (port->outputSocket() != NULL) {
	    addSocket(portEditPart, port->outputSocket());
	}
	
	// number of possible unconnected sockets
 	int unconnectedSockets = 2 - portEditPart->childCount();

 	while (unconnectedSockets > 0) {
 	    if (port->unconnectedSocket(unconnectedSockets - 1) != NULL) {
 		addSocket(
		    portEditPart,
		    port->unconnectedSocket(unconnectedSockets - 1));
 	    }
 	    unconnectedSockets--;
 	}
    } else {
	return NULL;
    }

    portEditPart->setSelectable(true);

    // Install editpolicy common to all ports.
    EditPolicy* editPolicy = editPolicyFactory_.createPortEditPolicy();
    if (editPolicy != NULL) {
	portEditPart->installEditPolicy(editPolicy);
    }

    return portEditPart;
}

/**
 * Creates an EditPart of a socket and adds it as a child to the given
 * EditPart.
 *
 * @param socket Socket of which to create the EditPart.
 * @param portEditPart Port in which to add the socket as a child.
 */
void
UnitPortFactory::addSocket(
    EditPart* portEditPart,
    MachinePart* socket) const {

    EditPart* socketEditPart = NULL;
    vector<Factory*>::const_iterator i = factories_.begin();

    while (socketEditPart == NULL && i != factories_.end()) {
	socketEditPart = (*i)->createEditPart(socket);
	i++;
    }

    assert(socketEditPart != NULL);

    SocketPortConnFactory connFactory;

    socketEditPart->addChild(
	connFactory.createConnection(
	    portEditPart, socketEditPart));
    portEditPart->addChild(socketEditPart);
}
