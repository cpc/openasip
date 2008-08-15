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
 * @file ContentsFigure.cc
 *
 * Definition of ContentsFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "Application.hh"
#include "ContentsFigure.hh"
#include "UnitContainerFigure.hh"
#include "BusContainerFigure.hh"
#include "SocketContainerFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

/**
 * The Constructor.
 */
ContentsFigure::ContentsFigure():
    Figure(), units_(NULL), buses_(NULL), sockets_(NULL) {
}

/**
 * The Destructor.
 */
ContentsFigure::~ContentsFigure() {
}

/**
 * Overloaded implementation allows only addition of certain types of
 * Figures for layout purposes.
 *
 * These are UnitContainerFigure, BusContainerFigure and
 * SocketContainerFigure.
 *
 * @param figure Child figure to add.
 */
void
ContentsFigure::addChild(Figure* figure) {

    Figure::addChild(figure);

    UnitContainerFigure* units = dynamic_cast<UnitContainerFigure*>(figure);
    if (units != NULL) {
	units_ = units;
	return;
    }

    BusContainerFigure* buses = dynamic_cast<BusContainerFigure*>(figure);
    if (buses != NULL) {
	buses_ = buses;
	return;
    }

    SocketContainerFigure* sockets =
	dynamic_cast<SocketContainerFigure*>(figure);
    if (sockets != NULL) {
	sockets_ = sockets;
	return;
    }

    // added a non supported type of a child figure
    assert(false);
}

/**
 * Draws figure on a given device context.
 *
 * Figure::draw() is overridden to enseure that the containers are drawn
 * in the correct order. (sockets over busses)
 *
 * @param dc Device context to draw figures on.
 */
void
ContentsFigure::draw(wxDC* dc) {
    if (units_ != NULL) {
        units_->draw(dc);
    }
    if (buses_ != NULL) {
        buses_->draw(dc);
    }
    if (sockets_ != NULL) {
        sockets_->draw(dc);
    }
}

/**
 * Lays out the subcontainers.
 *
 * @param dc Device context.
 */
void
ContentsFigure::layoutChildren(wxDC* dc) {

    int containerX =
	location_.x + MachineCanvasLayoutConstraints::VIEW_LEFT_MARGIN;
    int containerY =
	location_.y + MachineCanvasLayoutConstraints::VIEW_MARGIN;

    // first layout units if there are any

    if (units_ != NULL) {
	units_->setLocation(wxPoint(containerX, containerY));
	units_->layout(dc);
	containerY += units_->bounds().GetHeight() +
	    MachineCanvasLayoutConstraints::CONNECTIONS_SPACE;
    }

    // then layout buses

    if (buses_ != NULL) {

	buses_->setLocation(
	    wxPoint(containerX,
		    containerY + MachineCanvasLayoutConstraints::BUS_SPACE));

 	if (units_ != NULL && units_->bounds().GetWidth() >
	    MachineCanvasLayoutConstraints::BUS_MIN_WIDTH) {
	    buses_->setWidth(units_->bounds().GetWidth());
	} else {
	    buses_->setWidth(MachineCanvasLayoutConstraints::BUS_MIN_WIDTH);
	}
	
	buses_->layout(dc);
    }

    // layout sockets

    if (sockets_ != NULL) {

	sockets_->setLocation(wxPoint(containerX, containerY));

 	if (buses_ != NULL) {
 	    sockets_->setHeight(buses_->bounds().GetHeight() +
 				MachineCanvasLayoutConstraints::BUS_SPACE);
 	    sockets_->setWidth(buses_->bounds().GetWidth() +
 				MachineCanvasLayoutConstraints::BUS_SPACE);
 	} else if (units_ != NULL) {
 	    sockets_->setWidth(units_->bounds().GetWidth() +
			       MachineCanvasLayoutConstraints::BUS_SPACE);
	}	    
	
	sockets_->layout(dc);

 	if (buses_ != NULL) {
 	    buses_->setWidth(sockets_->bounds().GetWidth());
 	    buses_->layout(dc);
 	}
    }
}

/**
 * Calculates and sets the size of the container.
 * 
 * Depends on the sizes of the child containers: height will be equal
 * to the size of the unit container plus the height of the socket or
 * the bus container, whichever is higher. Width will be equal to the
 * width of the unit container, or the socket container if it is
 * wider.
 */
void
ContentsFigure::layoutSelf(wxDC*) {

    int width = 0;
    int height = 0;

    if (units_ != NULL) {
 	width = units_->bounds().GetWidth();
 	height = units_->bounds().GetHeight();
    }
    
    int higher = 0;

    if (sockets_ != NULL) {
	higher = sockets_->bounds().GetHeight();
 	if (sockets_->bounds().GetWidth() > width) {
 	    width = sockets_->bounds().GetWidth();
 	}
    }
    
    if (buses_ != NULL) {
  	if (buses_->bounds().GetHeight() > higher) {
  	    higher = buses_->bounds().GetHeight();
  	}
  	if (buses_->bounds().GetWidth() > width) {
  	    width = buses_->bounds().GetWidth();
  	}
    }

    height += higher + MachineCanvasLayoutConstraints::VIEW_MARGIN * 2;
    width += MachineCanvasLayoutConstraints::VIEW_MARGIN * 2;

    if (units_ != NULL && (buses_ != NULL || sockets_ != NULL)) {
	height += MachineCanvasLayoutConstraints::CONNECTIONS_SPACE;
    }

    size_.SetWidth(width);
    size_.SetHeight(height);
}
