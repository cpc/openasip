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
 * @file MoveFigure.cc
 *
 * Implementation of MoveFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MoveFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour MoveFigure::DEFAULT_COLOUR = wxColour(255, 255, 0);

/**
 * The Constructor.
 *
 * @param bus Figure of the bus which transports the move.
 * @param source Figure of the source port of the move.
 * @param target Figure of the target port of the move.
 */
MoveFigure::MoveFigure(Figure* bus, Figure* source, Figure* target):
    Figure(), bus_(bus), source_(source), target_(target) {

        laidOut_ = true;
}


/**
 * The Destructor.
 */
MoveFigure::~MoveFigure() {
}


/**
 * Draws the move figure on a device context.
 *
 * @param dc Device context where to draw the figure.
 */
void
MoveFigure::drawSelf(wxDC* dc) {

    assert(bus_ != NULL);

    wxPen pen = wxPen(DEFAULT_COLOUR, 4, wxPENSTYLE_SOLID);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxBRUSHSTYLE_SOLID);
    dc->SetPen(pen);
    dc->SetBrush(brush);

    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;
    int busY = bus_->location().y + bus_->bounds().GetHeight()/2;

    int sourceX = 0;
    int targetX = 0;

    // Draw source port -> bus line.
    if (source_ != NULL) {
        sourceX = source_->location().x + source_->bounds().GetWidth()/2;
        int sourceY = source_->location().y + offset * 2;
        dc->DrawLine(sourceX, sourceY, sourceX, busY);
        int size = std::max(
            std::min(source_->bounds().GetWidth(),
                     (MachineCanvasLayoutConstraints::BUS_SPACE-2)),
            (MachineCanvasLayoutConstraints::SOCKET_WIDTH));
        dc->DrawCircle(sourceX, busY, size/2);
    }

    // Draw bus -> target port line.
    if (target_ != NULL) {
        targetX = target_->location().x + target_->bounds().GetWidth()/2;
        int targetY = target_->location().y + offset * 2;
        dc->DrawLine(targetX, targetY, targetX, busY);
        int size = std::max(
            std::min(target_->bounds().GetWidth(),
                     (MachineCanvasLayoutConstraints::BUS_SPACE-2)),
            (MachineCanvasLayoutConstraints::SOCKET_WIDTH));
        dc->DrawCircle(targetX, busY, size/2);
    }

    // Draw line on the bus.
    if (target_ != NULL && source_ != NULL) {
        dc->DrawLine(sourceX, busY, targetX, busY);
    } else {
        // Source or target missing. Draw move over the whole bus.
        sourceX = bus_->location().x;
        targetX = bus_->location().x + bus_->bounds().GetWidth();
        dc->DrawLine(sourceX, busY, targetX, busY);
    }
}
