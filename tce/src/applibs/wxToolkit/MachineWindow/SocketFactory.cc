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
 * @file SocketFactory.cc
 *
 * Definition of SocketFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
