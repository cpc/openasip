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
 * @file TriggeringPortFigure.cc
 *
 * Definition of TriggeringPortFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
TriggeringPortFigure::TriggeringPortFigure(std::string name, int bitWidth):
    UnitPortFigure(name, bitWidth) {
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
