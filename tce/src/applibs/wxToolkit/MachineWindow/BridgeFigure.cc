/**
 * @file BridgeFigure.cc
 *
 * Definition of BridgeFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "BridgeFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour BridgeFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour BridgeFigure::DEFAULT_BG_COLOUR = wxColour(150, 200, 255);

/**
 * The Constructor.
 */
BridgeFigure::BridgeFigure():
    ConnectionFigure(), direction_(BridgeFigure::DIR_RIGHT) {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::BRIDGE_WIDTH,
	MachineCanvasLayoutConstraints::BRIDGE_HEIGHT);
}

/**
 * The Destructor.
 */
BridgeFigure::~BridgeFigure() {
}

/**
 * Draws the bridge's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
BridgeFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    dc->SetBrush(brush);

    const unsigned int triangleWidth = 200;
    
    wxPoint trianglePoints[3];

    if (direction_ == DIR_RIGHT) {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y );
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, location_.y);
	trianglePoints[2] = wxPoint(
	    trianglePoints[0].x + triangleWidth / 2,
	    location_.y + size_.GetHeight());

    } else {
	trianglePoints[0] = wxPoint(
	    location_.x + size_.GetWidth() / 2 - triangleWidth / 2,
	    location_.y + size_.GetHeight());
	trianglePoints[1] = wxPoint(
	    trianglePoints[0].x + triangleWidth, trianglePoints[0].y);
	trianglePoints[2] = wxPoint(
	    location_.x + size_.GetWidth() / 2, location_.y);
    }

    dc->DrawPolygon(3, trianglePoints);
}
