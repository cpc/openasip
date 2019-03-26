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
 * @file SocketBusConnFigure.cc
 *
 * Definition of SocketBusConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include <vector>

#include "Application.hh"
#include "SocketBusConnFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour SocketBusConnFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);

/**
 * The Constructor.
 */
SocketBusConnFigure::SocketBusConnFigure(): ConnectionFigure() {
}

/**
 * The Destructor.
 */
SocketBusConnFigure::~SocketBusConnFigure() {
}

/**
 * Draws the connection's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SocketBusConnFigure::drawSelf(wxDC* dc) {
    assert(source_ != NULL && target_ != NULL);
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
    dc->SetPen(pen);
    dc->SetBrush(brush);
    drawConnection(dc);
}


/**
 * Draws the connection's Figure on the give device context.
 */
void
SocketBusConnFigure::drawConnection(wxDC* dc) {

    int xOffset = source_->bounds().GetWidth()/2;
    int yOffset = target_->bounds().GetHeight()/2;

    // assumes that socket is always "source"
    dc->DrawCircle(source_->location().x + xOffset,
		   target_->location().y + yOffset,
		   size_.GetWidth()/2);
}


/**
 * Sets the figure's bounds.
 */
void
SocketBusConnFigure::layoutSelf(wxDC*) {
    setLocation(
        wxPoint(
            source_->location().x,
            target_->location().y +   
            (target_->bounds().GetHeight()/2) -
            (MachineCanvasLayoutConstraints::SOCKET_WIDTH / 2)));
    int size = std::max(std::min(source_->bounds().GetWidth(),
                                 (MachineCanvasLayoutConstraints::BUS_SPACE)),
                        (MachineCanvasLayoutConstraints::SOCKET_WIDTH));
    setWidth(size);
    setHeight(size);
                       
}
