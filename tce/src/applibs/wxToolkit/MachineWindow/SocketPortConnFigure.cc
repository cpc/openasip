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
 * @file SocketPortConnFigure.cc
 *
 * Definition of SocketPortConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;
    // assumes that port is always "source"
    dc->DrawLine(source_->location().x + offset,
		 source_->location().y + offset*2,
		 target_->location().x + offset,
		 target_->location().y);
}

/**
 * Sets the figure's bounds.
 */
void
SocketPortConnFigure::layoutSelf(wxDC*) {
    // assumes that port is always "source"
    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;    
    setHeight(target_->location().y - source_->location().y - offset*2);
    if(source_->location().x < target_->location().x) {
        setWidth(target_->location().x - source_->location().x + 3);
        setLocation(
            wxPoint(source_->location().x - 1 + offset,
                    source_->location().y + offset*2));

    } else {
        setWidth(source_->location().x - target_->location().x + 3);
        setLocation(
            wxPoint(target_->location().x - 1 + offset,
                    source_->location().y + offset*2));
    }
}
