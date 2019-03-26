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
 * @file BusContainerFigure.cc
 *
 * Definition of BusContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
