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
