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
 * @file SocketPortConnFigure.cc
 *
 * Definition of SocketPortConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed Jul 27 2004 by ml, pj, am
 * @note rating: yellow
 */

#include <vector>

#include "Application.hh"
#include "SocketPortConnFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour SocketPortConnFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);

/**
 * The Constructor.
 */
SocketPortConnFigure::SocketPortConnFigure(): ConnectionFigure() {
}

/**
 * The Destructor.
 */
SocketPortConnFigure::~SocketPortConnFigure() {
}

/**
 * Draws the connection's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SocketPortConnFigure::drawSelf(wxDC* dc) {
    assert(source_ != NULL && target_ != NULL);
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    drawConnection(dc);
}

/**
 * Draws the connection line on the dc.
 */
void
SocketPortConnFigure::drawConnection(wxDC* dc) {
    int xOffset = source_->bounds().GetWidth() / 2;

    int yOffset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;
    // assumes that port is always "source"
    dc->DrawLine(source_->location().x + xOffset,
		 source_->location().y + yOffset*2,
		 target_->location().x + xOffset,
		 target_->location().y);
}

/**
 * Sets the figure's bounds.
 */
void
SocketPortConnFigure::layoutSelf(wxDC*) {
    // assumes that port is always "source"
    int yOffset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;
    int xOffset = source_->bounds().GetWidth() / 2;
        
    setHeight(target_->location().y - source_->location().y - yOffset*2);
    if(source_->location().x < target_->location().x) {
        setWidth(target_->location().x - source_->location().x + 3);
        setLocation(
            wxPoint(source_->location().x - 1 + xOffset,
                    source_->location().y + yOffset*2));

    } else {
        setWidth(source_->location().x - target_->location().x + 3);
        setLocation(
            wxPoint(target_->location().x - 1 + xOffset,
                    source_->location().y + yOffset*2));
    }
}
