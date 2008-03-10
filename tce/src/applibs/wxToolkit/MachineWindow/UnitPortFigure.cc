/**
 * @file UnitPortFigure.cc
 *
 * Definition of UnitPortFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>

#include "UnitPortFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

using std::vector;

const wxColour UnitPortFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour UnitPortFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

/**
 * The Constructor.
 */
UnitPortFigure::UnitPortFigure(std::string name): Figure(), name_(name) {
    size_ = wxSize(
	MachineCanvasLayoutConstraints::PORT_WIDTH,
	MachineCanvasLayoutConstraints::PORT_WIDTH);
}

/**
 * The Destructor.
 */
UnitPortFigure::~UnitPortFigure() {
}

/**
 * Sets the x-coordinate for its child sockets.
 */
void
UnitPortFigure::layoutChildren(wxDC*) {

    int socketX = location_.x;
    bool oneSet = false;

    for (unsigned int i = 0; i < children_.size(); i++) {

	if (!oneSet && !children_[i]->xSet()) {
	    children_[i]->setX(location_.x);
	    oneSet = true;
	    socketX += MachineCanvasLayoutConstraints::SOCKET_WIDTH +
		       MachineCanvasLayoutConstraints::SOCKET_MIN_SPACE;

	} else if (children_[i]->location().x == 0) {
	    children_[i]->setPreferredX(socketX);
	}
    }
}

/**
 * Draws the port's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
UnitPortFigure::drawSelf(wxDC* dc) {
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);
    dc->DrawRectangle(location_.x, location_.y, size_.GetWidth(),
		      size_.GetHeight());
}

/**
 * Returns name of the port.
 *
 * @return Name of the port.
 */
std::string
UnitPortFigure::name() const {
    return name_;
}
