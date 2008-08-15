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
 * @file TriggeringPortFigure.cc
 *
 * Definition of TriggeringPortFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <vector>

#include "TriggeringPortFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

const wxColour TriggeringPortFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour TriggeringPortFigure::DEFAULT_BG_COLOUR =
    wxColour(255, 255, 255);


/**
 * The Constructor.
 *
 * @param name Name of the port.
 */
TriggeringPortFigure::TriggeringPortFigure(std::string name):
    UnitPortFigure(name) {
}

/**
 * The Destructor.
 */
TriggeringPortFigure::~TriggeringPortFigure() {
}


/**
 * Draws the port's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
TriggeringPortFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }

    dc->SetBrush(brush);

    dc->DrawRectangle(location_.x, location_.y, size_.GetWidth(),
		      size_.GetHeight());

    dc->DrawLine(
        location_.x, location_.y,
        location_.x + size_.GetWidth() - 1,
        location_.y + size_.GetHeight() - 1);
    dc->DrawLine(
        location_.x, location_.y + size_.GetHeight() - 1,
        location_.x + size_.GetWidth() - 1, location_.y);

}
