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
