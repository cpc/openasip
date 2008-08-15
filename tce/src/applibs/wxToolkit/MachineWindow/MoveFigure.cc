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
 * @file MoveFigure.cc
 *
 * Implementation of MoveFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
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

    wxPen pen = wxPen(DEFAULT_COLOUR, 4, wxSOLID);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
    dc->SetPen(pen);
    dc->SetBrush(brush);

    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;

    int busY = bus_->location().y + 
        (MachineCanvasLayoutConstraints::SEGMENT_HEIGHT / 2);

    int sourceX = 0;
    int targetX = 0;

    // Draw source port -> bus line.
    if (source_ != NULL) {
        sourceX = source_->location().x + offset;
        int sourceY = source_->location().y + offset * 2;
        dc->DrawLine(sourceX, sourceY, sourceX, busY);
        dc->DrawCircle(sourceX, busY, offset - 1);
    }

    // Draw bus -> target port line.
    if (target_ != NULL) {
        targetX = target_->location().x + offset;
        int targetY = target_->location().y + offset * 2;
        dc->DrawLine(targetX, targetY, targetX, busY);
        dc->DrawCircle(targetX, busY, offset - 1);
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
