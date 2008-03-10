/**
 * @file InputSocketFigure.cc
 *
 * Definition of InputSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "InputSocketFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour InputSocketFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour InputSocketFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

/**
 * The Constructor.
 */
InputSocketFigure::InputSocketFigure(): Figure() {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::SOCKET_WIDTH,
	MachineCanvasLayoutConstraints::SOCKET_HEIGHT);
}

/**
 * The Destructor.
 */
InputSocketFigure::~InputSocketFigure() {
}

/**
 * Draws the input socket's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
InputSocketFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);

    int triangleHeight = MachineCanvasLayoutConstraints::TRIANGLE_HEIGHT;

    wxPoint point1 = wxPoint(location_.x + size_.GetWidth()/2, location_.y);
    wxPoint point2 = wxPoint(location_.x, location_.y + triangleHeight);
    wxPoint point3 = wxPoint(
	location_.x + size_.GetWidth(), location_.y + triangleHeight);
    wxPoint trianglePoints[3] = {point1, point2, point3};

    dc->DrawPolygon(3, trianglePoints);
    dc->DrawRectangle(
	location_.x, location_.y + triangleHeight, size_.GetWidth(),
	size_.GetHeight() - triangleHeight);
}
