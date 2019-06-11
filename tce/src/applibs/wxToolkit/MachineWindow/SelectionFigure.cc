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
 * @file SelectToolFigure.cc
 *
 * Definition of SelectionFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SelectionFigure.hh"

#if wxCHECK_VERSION(3, 0, 0)
#include <wx/dcsvg.h>   // wxSVGFileDC only available in wxwidgets 3.*
#endif

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
    // wxWidgets implementation of SVGFileDC doesnt support dashed lines, so
    // we draw solid blue line when some item is selected when saving to *.svg
    bool isSVGcontext = false;

#if wxCHECK_VERSION(3, 0, 0)
    if (dynamic_cast<wxSVGFileDC*> (dc)) {
        isSVGcontext = true;
    }
#endif

    // Draw blue rectangles around selected EditParts figures.
    if (selection_ != NULL) {

        wxPen pen = wxPen(wxColour(0,0,255), 2, wxSHORT_DASH);
        if (isSVGcontext) {
            pen.SetStyle(wxSOLID);
        }
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
