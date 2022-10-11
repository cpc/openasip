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
 * @file UnitContainerFigure.cc
 *
 * Definition of UnitContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "UnitContainerFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

/**
 * The Constructor.
 */
UnitContainerFigure::UnitContainerFigure(): Figure() {
}

/**
 * The Destructor.
 */
UnitContainerFigure::~UnitContainerFigure() {
}

/**
 * Lays out the units in a row, spaced evenly.
 *
 * @param dc Device context.
 */
void
UnitContainerFigure::layoutChildren(wxDC* dc) {

    int unitX = location_.x;
    int unitY = location_.y;

    for(unsigned int i = 0; i < children_.size(); i++) {

	if (!children_[i]->xSet()) {
	    children_[i]->setLocation(wxPoint(unitX, unitY));
	    children_[i]->layout(dc);
	    unitX += children_[i]->bounds().GetWidth() +
		MachineCanvasLayoutConstraints::UNIT_SPACE;

	} else {
	    children_[i]->layout(dc);
	    unitX = children_[i]->location().x +
		children_[i]->bounds().GetWidth() +
		MachineCanvasLayoutConstraints::UNIT_SPACE;
	}
    }
}

/**
 * Calculates and sets the size of the container.
 *
 * Depends on the amount and position of the units.
 */
void
UnitContainerFigure::layoutSelf(wxDC*) {

    int maxIndex = 0;
    int maxX = 0;

    for (unsigned int i = 0; i < children_.size(); i++) {
	if (children_[i]->location().x > maxX) {
	    maxX = children_[i]->location().x;
	    maxIndex = i;
	}
    }

    int width = maxX + children_[maxIndex]->bounds().GetWidth() -
	location_.x;

    if (width > size_.GetWidth()) {
        size_.SetWidth(width);
    }

    if (size_.GetHeight() == 0) {
	size_.SetHeight(children_[0]->bounds().GetHeight());
    }
}
