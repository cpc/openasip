/**
 * @file UnknownSocketFigure.cc
 *
 * Definition of UnknownSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    dc->SetBrush(brush);
    dc->DrawRectangle(
	location_.x, location_.y, size_.GetWidth(), size_.GetHeight());
}
