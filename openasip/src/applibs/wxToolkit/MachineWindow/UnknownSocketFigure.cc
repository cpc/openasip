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
 * @file UnknownSocketFigure.cc
 *
 * Definition of UnknownSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "UnknownSocketFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour UnknownSocketFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour UnknownSocketFigure::DEFAULT_BG_COLOUR =
    wxColour(255, 255, 255);

/**
 * The Constructor.
 */
UnknownSocketFigure::UnknownSocketFigure(): Figure() {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::SOCKET_WIDTH,
	MachineCanvasLayoutConstraints::SOCKET_HEIGHT);
}

/**
 * The Destructor.
 */
UnknownSocketFigure::~UnknownSocketFigure() {
}

/**
 * Draws the socket's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
UnknownSocketFigure::drawSelf(wxDC* dc) {
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxPENSTYLE_SOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxBRUSHSTYLE_SOLID);
    dc->SetBrush(brush);
    dc->DrawRectangle(
	location_.x, location_.y, size_.GetWidth(), size_.GetHeight());
}
