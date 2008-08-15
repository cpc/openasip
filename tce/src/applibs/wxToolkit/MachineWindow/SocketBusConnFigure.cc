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
 * @file SocketBusConnFigure.cc
 *
 * Definition of SocketBusConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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

    int xOffset = MachineCanvasLayoutConstraints::SOCKET_WIDTH / 2;
    int yOffset = MachineCanvasLayoutConstraints::SEGMENT_HEIGHT / 2;

    // assumes that socket is always "source"
    dc->DrawCircle(source_->location().x + xOffset,
		   target_->location().y + yOffset,
		   xOffset);
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
            (MachineCanvasLayoutConstraints::SEGMENT_HEIGHT / 2) -
            (MachineCanvasLayoutConstraints::SOCKET_WIDTH / 2)));
    setWidth(MachineCanvasLayoutConstraints::SOCKET_WIDTH);
    setHeight(MachineCanvasLayoutConstraints::SOCKET_WIDTH + 2);
}
