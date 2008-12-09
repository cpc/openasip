/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BidirBridgeFigure.cc
 *
 * Definition of BidirBridgeFigure class.
 *
 * @author Ari Metsähalme 2004 (ari.metsahalme-no.spam-tut.fi)
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
