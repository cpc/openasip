/**
 * @file SegmentFigure.cc
 *
 * Definition of SegmentFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include "SegmentFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour SegmentFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);


/**
 * The Constructor.
 */
SegmentFigure::SegmentFigure(): Figure(), last_(false) {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::BUS_MIN_WIDTH,
	MachineCanvasLayoutConstraints::SEGMENT_HEIGHT);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
SegmentFigure::~SegmentFigure() {
}

/**
 * Draws the segment's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SegmentFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    wxBrush brush = wxBrush(DEFAULT_COLOUR, wxSOLID);
    if (highlighted_) {
	pen = wxPen(highlight_, 1, wxSOLID);
	brush = wxBrush(highlight_, wxSOLID);
    }

    dc->SetPen(pen);
    dc->SetBrush(brush);

    if (!last_) {

	// draw arrow 
	const int arrowSize = 8;
	wxPoint arrowPoints[3];
	arrowPoints[0] = wxPoint(
	    location_.x + size_.GetWidth() - arrowSize,
	    location_.y - arrowSize / 2 + size_.GetHeight() / 2);
	arrowPoints[1] = wxPoint(
	    arrowPoints[0].x + arrowSize, arrowPoints[0].y + arrowSize / 2);
	arrowPoints[2] = wxPoint(
	    arrowPoints[0].x, arrowPoints[0].y + arrowSize);
	dc->DrawPolygon(3, arrowPoints);	
	dc->DrawRectangle(
	    location_.x, location_.y, size_.GetWidth() - arrowSize,
	    size_.GetHeight());

    } else {
	dc->DrawRectangle(
	    location_.x, location_.y, size_.GetWidth(), size_.GetHeight());
    }
}
