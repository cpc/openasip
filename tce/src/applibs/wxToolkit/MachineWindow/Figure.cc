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
 * @file Figure.cc
 *
 * Definition of Figure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "Application.hh"
#include "Figure.hh"

using std::vector;

/**
 * The Constructor.
 */
Figure::Figure():
    location_(wxPoint(0,0)), size_(wxSize(0,0)), minSize_(wxSize(0,0)),
    xSet_(false), laidOut_(false), drawn_(false), highlight_(*wxRED),
    highlighted_(false), options_(NULL) {
}

/**
 * The Destructor.
 */
Figure::~Figure() {
}

/**
 * Sets the size of the Figure's bounding rectangle.
 *
 * @param bounds New size.
 */
void
Figure::setBounds(wxSize bounds) {
    if (bounds.GetHeight() < size_.GetHeight()) {
	size_.SetHeight(minSize_.GetHeight());
    } else {
	size_.SetHeight(bounds.GetHeight());
    }
    if (bounds.GetWidth() < size_.GetWidth()) {
	size_.SetWidth(minSize_.GetWidth());
    } else {
	size_.SetWidth(bounds.GetWidth());
    }
}

/**
 * Sets the width of the Figure.
 *
 * @param width New width.
 */
void
Figure::setWidth(int width) {
    if (width < minSize_.GetWidth()) {
	size_.SetWidth(minSize_.GetWidth());
    } else {
	size_.SetWidth(width);
    }
}

/**
 * Sets the height of the Figure.
 *
 * @param height New height.
 */
void
Figure::setHeight(int height) {
    if (height < minSize_.GetHeight()) {
	size_.SetHeight(minSize_.GetHeight());
    } else {
	size_.SetHeight(height);
    }
}

/**
 * Lays out the Figure and its children.
 *
 * @param dc The device context to layout children on.
 */
void
Figure::layout(wxDC* dc) {
    layoutChildren(dc);
    layoutSelf(dc);
    laidOut_ = true;
}

/**
 * Empty default implementation that can be used if no specific laying
 * out needs to be done or the Figure doesn't have children.
 *
 * Tells all children to layout themselves.
 *
 * @param dc Device context.
 */
void
Figure::layoutChildren(wxDC* dc) {
    vector<Figure*>::iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	(*i)->layout(dc);
    }
}

/**
 * Draws the Figure and its children on the given device context.
 *
 * @param dc The device context to draw the Figure on.
 * @note The Figure must be laid out before drawing!
 */
void
Figure::draw(wxDC* dc) {
    assert(laidOut_ == true);
    drawSelf(dc);
    drawChildren(dc);
    drawn_ = true;
}

/**
 * Draws the Figure's children.
 *
 * @param dc The device context to draw the children on.
 */
void
Figure::drawChildren(wxDC* dc) {
    vector<Figure*>::iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	(*i)->draw(dc);
    }
}

/**
 * Clears highlighting of the figure.
 *
 * The highlight is cleared next time the figure is drawn.
 * This function has no effect if the figure isn't highlighted.
 */
void
Figure::clearHighlight() {
    highlighted_ = false;
    // Clear highlighting of child figures.
    vector<Figure*>::iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	(*i)->clearHighlight();
    }
}

/**
 * Highlights the figure with given colour.
 *
 * The figure is highlighted next time it's drawn.
 *
 * @param colour Highlight colour.
 */
void
Figure::highlight(const wxColour& colour) {

    highlight_ = colour;
    highlighted_ = true;
    // Highlight child figures.
    vector<Figure*>::iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	(*i)->highlight(colour);
    }
}

/**
 * Returns the options object used by this figure and it's children.
 *
 * @return Current figure options.
 */
MachineCanvasOptions*
Figure::options() {
    return options_;
}


/**
 * Sets the options object used for setting drawing options.
 *
 * Options are set recursively for figure children.
 *
 * @param options Options to set.
 */
void
Figure::setOptions(MachineCanvasOptions* options) {
    vector<Figure*>::iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	(*i)->setOptions(options);
    }
    options_ = options;
}
