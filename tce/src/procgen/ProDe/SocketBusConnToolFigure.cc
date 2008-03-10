/**
 * @file SocketBusConnToolFigure.cc
 *
 * Definition of SocketBusConnToolFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "SocketBusConnToolFigure.hh"
#include "MDFLayoutConstraints.hh"
#include "ProDeConstants.hh"

const wxColor SocketBusConnToolFigure::LINE_COLOR_GREEN = wxColor(0, 200, 0);
const wxColor SocketBusConnToolFigure::LINE_COLOR_RED = wxColor(255, 0, 0);

/**
 * The Constructor.
 */
SocketBusConnToolFigure::SocketBusConnToolFigure(bool creating):
    SocketBusConnFigure() {

    if (creating) {
        color_ = LINE_COLOR_GREEN;
    } else {
        color_ = LINE_COLOR_RED;
    }
}

/**
 * The Destructor.
 */
SocketBusConnToolFigure::~SocketBusConnToolFigure() {
}

/**
 * Draws the connection's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SocketBusConnToolFigure::drawSelf(wxDC* dc) {
    assert(source_ != NULL && target_ != NULL);
    wxPen pen = wxPen(color_, 1, wxSOLID);
    wxBrush brush = wxBrush(color_, wxSOLID);
    dc->SetPen(pen);
    dc->SetBrush(brush);
    drawConnection(dc);
}
