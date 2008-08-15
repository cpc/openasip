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
