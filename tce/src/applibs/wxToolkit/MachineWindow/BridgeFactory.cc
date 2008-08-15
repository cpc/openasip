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
 * @file BridgeFactory.cc
 *
 * Definition of BridgeFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
