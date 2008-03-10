/**
 * @file BusFactory.cc
 *
 * Definition of BusFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "MachinePart.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "BusFigure.hh"
#include "BusFactory.hh"
#include "EditPart.hh"
#include "SegmentFactory.hh"
#include "EditPolicyFactory.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
BusFactory::BusFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new SegmentFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
BusFactory::~BusFactory() {
}

/**
 * Returns an EditPart corresponding to a bus.
 *
 * @param component Bus of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         Bus class.
 */
EditPart*
BusFactory::createEditPart(MachinePart* component) {

    EditPart* busEditPart = EditPartFactory::checkCache(component);

    if (busEditPart != NULL) {
	return busEditPart;
    }

    Bus* bus = dynamic_cast<Bus*>(component);

    if (bus != NULL) {

	busEditPart = new EditPart();
	busEditPart->setModel(bus);

	BusFigure* fig = new BusFigure(bus->position());
	busEditPart->setFigure(fig);

        for (int i = 0; i < bus->segmentCount(); i++) {
            vector<Factory*>::const_iterator iter = factories_.begin();
            while (iter != factories_.end()) {
                EditPart* segEditPart =
                    (*iter)->createEditPart((MachinePart*)bus->segment(i));
                if (segEditPart != NULL) {
                    busEditPart->addChild(segEditPart);
                    segEditPart->setSelectable(true);
                    segEditPart->setParent(busEditPart);
                }
		iter++;
            }
        }
	
	busEditPart->setSelectable(true);

	EditPolicy* editPolicy = editPolicyFactory_.createBusEditPolicy();
	if (editPolicy != NULL) {
	    busEditPart->installEditPolicy(editPolicy);
	}
	EditPartFactory::writeToCache(busEditPart);
	return busEditPart;

    } else {
	return NULL;
    } 
}
