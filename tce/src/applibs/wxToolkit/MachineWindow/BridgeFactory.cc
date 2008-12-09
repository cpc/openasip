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
 * @file BridgeFactory.cc
 *
 * Definition of BridgeFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "Application.hh"
#include "Bridge.hh"
#include "BridgeFigure.hh"
#include "BridgeFactory.hh"
#include "ConnectionEditPart.hh"
#include "BusFactory.hh"
#include "EditPolicyFactory.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
BridgeFactory::BridgeFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new BusFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
BridgeFactory::~BridgeFactory() {
}

/**
 * Returns an EditPart corresponding to a bridge.
 *
 * @param component Bridge of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         Bridge class.
 */
EditPart*
BridgeFactory::createEditPart(MachinePart* component) {

    Bridge* bridge = dynamic_cast<Bridge*>(component);

    if (bridge != NULL) {

	ConnectionEditPart* bridgeEditPart = new ConnectionEditPart();

	bridgeEditPart->setModel(bridge);
	BridgeFigure* fig = new BridgeFigure();
	bridgeEditPart->setFigure(fig);

	vector<Factory*>::const_iterator iter;

	for (iter = factories_.begin(); iter != factories_.end(); iter++) {

	    EditPart* busEditPart = (*iter)->createEditPart(
		(MachinePart*)bridge->sourceBus());

	    if (busEditPart != NULL) {
		bridgeEditPart->setSource(busEditPart);
		bridgeEditPart->setTarget(
		    (*iter)->createEditPart(
			(MachinePart*)bridge->destinationBus()));
		break;
	    }
	}

	assert(bridgeEditPart->target() != NULL &&
	       bridgeEditPart->source() != NULL);
	
	bridgeEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createBridgeEditPolicy();
	if (editPolicy != NULL) {
	    bridgeEditPart->installEditPolicy(editPolicy);
	}
	
	return bridgeEditPart;
	
    } else {
	return NULL;
    } 
}
