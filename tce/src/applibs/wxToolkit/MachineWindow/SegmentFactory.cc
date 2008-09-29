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
 * @file SegmentFactory.cc
 *
 * Definition of SegmentFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#include <vector>

#include "Segment.hh"
#include "SegmentFigure.hh"
#include "SegmentFactory.hh"
#include "EditPart.hh"
#include "SegmentFactory.hh"
#include "SocketFactory.hh"
#include "SocketBusConnFactory.hh"
#include "EditPolicyFactory.hh"

using std::vector;
using namespace TTAMachine;

/**
 * The Constructor.
 */
SegmentFactory::SegmentFactory(EditPolicyFactory& editPolicyFactory):
    EditPartFactory(editPolicyFactory) {

    registerFactory(new SocketFactory(editPolicyFactory));
}

/**
 * The Destructor.
 */
SegmentFactory::~SegmentFactory() {
}

/**
 * Returns an EditPart corresponding to a segment.
 *
 * @param component Segment of which to create the EditPart.
 * @return NULL if the parameter is not an instance of the
 *         Segment class.
 */
EditPart*
SegmentFactory::createEditPart(MachinePart* component) {

    Segment* segment = dynamic_cast<Segment*>(component);

    if (segment != NULL) {
	EditPart* segmentEditPart = new EditPart();
	segmentEditPart->setModel(segment);

	SegmentFigure* fig = new SegmentFigure();
	segmentEditPart->setFigure(fig);

	SocketBusConnFactory connFactory;

	for (int j = 0; j < segment->connectionCount(); j++) {
	    vector<Factory*>::const_iterator i = factories_.begin();
	    for (i = factories_.begin(); i != factories_.end(); i++) {
		EditPart* socketEditPart =
		    (*i)->createEditPart((MachinePart*)segment->connection(j));
		if (socketEditPart != NULL) {
		    socketEditPart->addChild(
			connFactory.createConnection(
			    socketEditPart, segmentEditPart));
		    break;
		}
	    }
	}

	EditPolicy* editPolicy = editPolicyFactory_.createSegmentEditPolicy();
	if (editPolicy != NULL) {
	    segmentEditPart->installEditPolicy(editPolicy);
	}

	return segmentEditPart;

    } else {
	return NULL;
    } 
}
