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
 * @file BusFactory.cc
 *
 * Definition of BusFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
