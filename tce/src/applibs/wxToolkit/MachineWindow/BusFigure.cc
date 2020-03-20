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
 * @file BusFigure.cc
 *
 * Definition of BusFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include <vector>

#include "BusFigure.hh"
#include "SegmentFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"
#include "WxConversion.hh"

using std::vector;

const wxColour BusFigure::DEFAULT_COLOUR = wxColour(150, 150, 150);

/**
 * The Constructor.
 */
BusFigure::BusFigure(unsigned int slot): Figure(), slot_(slot) {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BUS_MIN_WIDTH,
	MachineCanvasLayoutConstraints::BUS_MIN_HEIGHT);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
BusFigure::~BusFigure() {}

/**
 * Returns the virtual bounds of a bus, which are a bit higher than
 * the visible bounds to make selection easier.
 *
 * @return The virtual bounds of a bus.
 */
wxRect
BusFigure::virtualBounds() const {
    if (childCount() <= 1) {
	wxRect virtualLocation = wxRect(
	    wxPoint(location_.x, location_.y - size_.GetHeight()),
	    wxSize(size_.GetWidth(), size_.GetHeight() * 3));
	return virtualLocation;
    } else {
	return wxRect(location_, size_);
    }
}

/** 
 * Lays ouf the segments in a column, spaced evenly.
 *
 * @param dc Device context.
 */
void
BusFigure::layoutChildren(wxDC* dc) {

    int segY = location_.y;
    int segX = location_.x;

    for(unsigned int i = 0; i < children_.size(); i++) {
        children_[i]->setLocation(wxPoint(segX, segY));
	children_[i]->setWidth(size_.GetWidth());
        children_[i]->layout(dc);
        segY += children_[i]->bounds().GetHeight() +
	    MachineCanvasLayoutConstraints::SEGMENT_SPACE;
    }
}

/**
 * Calculates and sets bus size according to the size of segments.
 */
void
BusFigure::layoutSelf(wxDC*) {

    if (children_.size() > 0) {
	int segsHeight = 0;

	vector<Figure*>::const_iterator i = children_.begin();
	for (; i != children_.end(); i++) {
	    segsHeight += (*i)->bounds().GetHeight();
	}
	
	dynamic_cast<SegmentFigure*>(children_.back())->setLast(true);
	
	segsHeight += (children_.size() - 1)
	    * MachineCanvasLayoutConstraints::SEGMENT_SPACE;
	
	size_.SetHeight(segsHeight);
    }
}

/**
 * Draws the Figure of the bus on the given device context if it has
 * no segments, otherwise only the segments are drawn.
 *
 * @param dc The device context.
 */
void
BusFigure::drawSelf(wxDC* dc) {

    if (children_.empty()) {
	wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
	wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
	dc->SetPen(pen);
	dc->SetBrush(brush);
	
	dc->DrawRectangle(location_.x, location_.y,
			  size_.GetWidth(), size_.GetHeight());
    }

    // Draw slot label.
    int slotLabelWidth;
    int slotLabelHeight;
    wxString slotLabel = WxConversion::toWxString(slot_);
    dc->GetTextExtent(slotLabel, &slotLabelWidth, &slotLabelHeight);

    int slotLabelX = location_.x - slotLabelWidth - 10;
    int slotLabelY =
        ((size_.GetHeight() / 2) + location_.y) - (slotLabelHeight / 2);

    dc->SetBackgroundMode(wxTRANSPARENT);
    dc->SetTextForeground(*wxBLACK);
    dc->DrawText(slotLabel, slotLabelX, slotLabelY);
}
