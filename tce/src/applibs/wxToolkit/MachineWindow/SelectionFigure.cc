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
 * @file SelectToolFigure.cc
 *
 * Definition of SelectionFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SelectionFigure.hh"

const int SelectionFigure::SELECT_BOX_MARGIN = 4;


/**
 * The Constructor.
 *
 * @param selection Figure of the selected EditPart.
 */
SelectionFigure::SelectionFigure(Figure* selection):
    Figure(), selection_(selection) {
}

/**
 * The Destructor.
 */
SelectionFigure::~SelectionFigure() {
}

/**
 * Draws the selection's figure on the given device context.
 */
void
SelectionFigure::drawSelf(wxDC* dc) {
    // Draw blue rectangles around selected EditParts figures.
    if (selection_ != NULL) {
        wxPen pen = wxPen(wxColour(0,0,255), 2, wxSHORT_DASH);
        wxBrush brush = wxBrush(wxColor(0,0,0), wxTRANSPARENT);
        dc->SetBrush(brush);
        dc->SetPen(pen);
        wxRect bounds = selection_->bounds();
        dc->DrawRectangle(
            bounds.GetX()-SELECT_BOX_MARGIN,
            bounds.GetY()-SELECT_BOX_MARGIN,
            bounds.GetWidth() + 2*SELECT_BOX_MARGIN,
            bounds.GetHeight() + 2*SELECT_BOX_MARGIN);
    }
}


/**
 * Set the selected figure.
 *
 * Selection figure will be drawn around the selected figure.
 *
 * @param selection Figure of the selected part.
 */
void
SelectionFigure::setSelection(Figure* selection) {
    selection_ = selection;
}


/**
 * Returns the Figure's bounding rectangle.
 *
 * @return Figure's bounding rectangle.
 */
wxRect
SelectionFigure::bounds() const {
    if (selection_ == NULL) {
        return wxRect(0, 0, 0, 0);
    }
    wxRect bounds = selection_->bounds();
    bounds.Offset(SELECT_BOX_MARGIN * -2, SELECT_BOX_MARGIN);
    bounds.Inflate(SELECT_BOX_MARGIN * 4);
    return bounds;
}
