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
 * @file UnitPortFactory.cc
 *
 * Definition of UnitPortFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
            fig = new TriggeringPortFigure(port->name());
        } else {
            fig = new UnitPortFigure(port->name());
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
