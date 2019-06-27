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
 * @file SocketPortConnToolFigure.cc
 *
 * Definition of SocketPortConnToolFigure class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "SocketPortConnToolFigure.hh"
#include "ProDeConstants.hh"

const wxColor SocketPortConnToolFigure::LINE_COLOR_GREEN = wxColor(0, 200, 0);
const wxColor SocketPortConnToolFigure::LINE_COLOR_RED = wxColor(255, 0, 0);

/**
 * The Constructor.
 *
 * @param creating True, if the tool is creating a new connection.
 *                 False, if an old connection is being removed.
 */
SocketPortConnToolFigure::SocketPortConnToolFigure(bool creating):
    SocketPortConnFigure() {

    if (creating) {
        color_ = LINE_COLOR_GREEN;
    } else {
        color_ = LINE_COLOR_RED;
    }
}


/**
 * The Destructor.
 */
SocketPortConnToolFigure::~SocketPortConnToolFigure() {
}

/**
 * Draws the connection's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SocketPortConnToolFigure::drawSelf(wxDC* dc) {
    assert(source_ != NULL && target_ != NULL);
    wxPen pen = wxPen(color_, 3, wxSHORT_DASH);
    dc->SetPen(pen);
    drawConnection(dc);
}
