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
 * @file SocketContainerFigure.cc
 *
 * Definition of SocketContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "SocketContainerFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

/**
 * The Constructor.
 */
SocketContainerFigure::SocketContainerFigure(): Figure() {
}

/**
 * The Destructor.
 */
SocketContainerFigure::~SocketContainerFigure() {
}

/**
 * Lays out child sockets.
 *
 * @param dc Device context.
 */
void
SocketContainerFigure::layoutChildren(wxDC* dc) {
    
    int unknownSocketX = location_.x + size_.GetWidth();
    
    // set locations for sockets not connected to any port
    
    vector<Figure*>::iterator i = children_.begin();
    for(; i != children_.end(); i++) {

	if ((*i)->location().x == 0) {
	    (*i)->setX(unknownSocketX);
	    unknownSocketX += MachineCanvasLayoutConstraints::SOCKET_WIDTH +
		MachineCanvasLayoutConstraints::SOCKET_SPACE;
	}

	(*i)->setY(location_.y);
	if (size_.GetHeight() > (*i)->bounds().GetHeight()) {
	    (*i)->setHeight(size_.GetHeight());
	}
	(*i)->layout(dc);
	(*i)->clearXSetFlag();
    }
}

/**
 * Calculates and sets the width of the container.
 *
 * Depends on the amount and position of sockets.
 */
void
SocketContainerFigure::layoutSelf(wxDC*) {
 
    int maxIndex = 0;
    int maxX = 0;

    for (unsigned int i = 0; i < children_.size(); i++) {
	if (children_[i]->location().x > maxX) {
	    maxX = children_[i]->location().x;
	    maxIndex = i;
	}
    }

    int rightBound = maxX + children_[maxIndex]->bounds().GetWidth() +
	MachineCanvasLayoutConstraints::SOCKET_WIDTH;

    if (rightBound > location_.x + size_.GetWidth()) {
        size_.SetWidth(rightBound - location_.x);
    }

    if (children_[0]->bounds().GetHeight() > size_.GetHeight()) {
	size_.SetHeight(children_[0]->bounds().GetHeight());
    }
}
