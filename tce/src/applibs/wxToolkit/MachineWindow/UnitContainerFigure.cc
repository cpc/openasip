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
 * @file UnitContainerFigure.cc
 *
 * Definition of UnitContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
