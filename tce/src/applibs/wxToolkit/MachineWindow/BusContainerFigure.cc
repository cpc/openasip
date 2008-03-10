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
