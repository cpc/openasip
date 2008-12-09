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
