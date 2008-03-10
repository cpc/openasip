/**
 * @file SocketPortConnToolFigure.cc
 *
 * Definition of SocketPortConnToolFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
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
