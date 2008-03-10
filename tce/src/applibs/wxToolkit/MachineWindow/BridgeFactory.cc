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
