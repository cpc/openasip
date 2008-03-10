/**
 * @file TriggeringPortFigure.cc
 *
 * Definition of TriggeringPortFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
TriggeringPortFigure::TriggeringPortFigure(std::string name):
    UnitPortFigure(name) {
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
