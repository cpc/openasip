/**
 * @file BidirBridgeFigure.cc
 *
 * Definition of BidirBridgeFigure class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include <vector>

#include "Application.hh"
#include "BidirBridgeFigure.hh"
#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

/**
 * The Constructor.
 */
BidirBridgeFigure::BidirBridgeFigure(): BridgeFigure() {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BRIDGE_WIDTH,
	MachineCanvasLayoutConstraints::BRIDGE_HEIGHT * 2);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
BidirBridgeFigure::~BidirBridgeFigure() {
}

/**
 * Lays out the two child bridges.
 *
 * @param dc Device context.
 */
void
BidirBridgeFigure::layoutChildren(wxDC* dc) {

    assert(children_.size() == 2);

    dynamic_cast<BridgeFigure*>(
	children_[0])->setDirection(BridgeFigure::DIR_LEFT);

    children_[0]->setX(location_.x);
    children_[0]->setY(location_.y);
    children_[0]->setWidth(size_.GetWidth());
    children_[0]->layout(dc);

    dynamic_cast<BridgeFigure*>(
	children_[1])->setDirection(BridgeFigure::DIR_RIGHT);

    children_[1]->setX(location_.x);
    children_[1]->setY(
	location_.y + MachineCanvasLayoutConstraints::BRIDGE_HEIGHT);

    children_[1]->setWidth(size_.GetWidth());
    children_[1]->layout(dc);
}

/**
 * This figure is not visible on the canvas.
 *
 * Empty implementation is defined to override BridgeFigure.
 */
void
BidirBridgeFigure::drawSelf(wxDC*) {
}
