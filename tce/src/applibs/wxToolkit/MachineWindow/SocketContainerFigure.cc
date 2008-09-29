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
