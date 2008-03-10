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
