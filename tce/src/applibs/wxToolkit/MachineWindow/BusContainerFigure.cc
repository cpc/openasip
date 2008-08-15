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
 * @file BusContainerFigure.cc
 *
 * Definition of BusContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "BusContainerFigure.hh"
#include "BusFigure.hh"
#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"
#include "BusChainFigure.hh"

using std::vector;

/**
 * The Constructor.
 */
BusContainerFigure::BusContainerFigure(): Figure() {
}

/**
 * The Destructor.
 */
BusContainerFigure::~BusContainerFigure() {
}

/**
 * Lays out buses and bridges.
 *
 * @param dc Device context.
 */
void
BusContainerFigure::layoutChildren(wxDC* dc) {

    int busX = location_.x;
    int busY = location_.y;

    vector<Figure*>::iterator i = children_.begin();
    while (i != children_.end()) {
	(*i)->setLocation(wxPoint(busX, busY));
	(*i)->setWidth(size_.GetWidth());
	(*i)->layout(dc);

	busY += (*i)->bounds().GetHeight() +
	    MachineCanvasLayoutConstraints::BUS_SPACE;

	i++;
    }
}

/**
 * Calculates and sets the size of the container.
 *
 * Depends on the size and amount of buses.
 */
void
BusContainerFigure::layoutSelf(wxDC*) {

    int height = 0;

    vector<Figure*>::const_iterator i = children_.begin();
    while (i != children_.end()) {
	height += (*i)->bounds().GetHeight();
	i++;
    }

    height += MachineCanvasLayoutConstraints::BUS_SPACE * (children_.size());
    size_.SetHeight(height);

    if (size_.GetWidth() == 0) {
	size_.SetWidth(children_[0]->bounds().GetWidth());
    }
}
