/**
 * @file SelectToolFigure.cc
 *
 * Definition of SelectionFigure class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SelectionFigure.hh"

const int SelectionFigure::SELECT_BOX_MARGIN = 4;


/**
 * The Constructor.
 *
 * @param selection Figure of the selected EditPart.
 */
SelectionFigure::SelectionFigure(Figure* selection):
    Figure(), selection_(selection) {
}

/**
 * The Destructor.
 */
SelectionFigure::~SelectionFigure() {
}

/**
 * Draws the selection's figure on the given device context.
 */
void
SelectionFigure::drawSelf(wxDC* dc) {
    // Draw blue rectangles around selected EditParts figures.
    if (selection_ != NULL) {
        wxPen pen = wxPen(wxColour(0,0,255), 2, wxSHORT_DASH);
        wxBrush brush = wxBrush(wxColor(0,0,0), wxTRANSPARENT);
        dc->SetBrush(brush);
        dc->SetPen(pen);
        wxRect bounds = selection_->bounds();
        dc->DrawRectangle(
            bounds.GetX()-SELECT_BOX_MARGIN,
            bounds.GetY()-SELECT_BOX_MARGIN,
            bounds.GetWidth() + 2*SELECT_BOX_MARGIN,
            bounds.GetHeight() + 2*SELECT_BOX_MARGIN);
    }
}


/**
 * Set the selected figure.
 *
 * Selection figure will be drawn around the selected figure.
 *
 * @param selection Figure of the selected part.
 */
void
SelectionFigure::setSelection(Figure* selection) {
    selection_ = selection;
}


/**
 * Returns the Figure's bounding rectangle.
 *
 * @return Figure's bounding rectangle.
 */
wxRect
SelectionFigure::bounds() const {
    if (selection_ == NULL) {
        return wxRect(0, 0, 0, 0);
    }
    wxRect bounds = selection_->bounds();
    bounds.Offset(SELECT_BOX_MARGIN * -2, SELECT_BOX_MARGIN);
    bounds.Inflate(SELECT_BOX_MARGIN * 4);
    return bounds;
}
