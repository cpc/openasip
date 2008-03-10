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
