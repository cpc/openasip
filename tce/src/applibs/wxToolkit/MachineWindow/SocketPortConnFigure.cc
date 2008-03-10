/**
 * @file SocketPortConnFigure.cc
 *
 * Definition of SocketPortConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed Jul 27 2004 by ml, pj, am
 * @note rating: yellow
 */

#include <vector>

#include "Application.hh"
#include "SocketPortConnFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"

const wxColour SocketPortConnFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);

/**
 * The Constructor.
 */
SocketPortConnFigure::SocketPortConnFigure(): ConnectionFigure() {
}

/**
 * The Destructor.
 */
SocketPortConnFigure::~SocketPortConnFigure() {
}

/**
 * Draws the connection's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
SocketPortConnFigure::drawSelf(wxDC* dc) {
    assert(source_ != NULL && target_ != NULL);
    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    drawConnection(dc);
}

/**
 * Draws the connection line on the dc.
 */
void
SocketPortConnFigure::drawConnection(wxDC* dc) {
    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;
    // assumes that port is always "source"
    dc->DrawLine(source_->location().x + offset,
		 source_->location().y + offset*2,
		 target_->location().x + offset,
		 target_->location().y);
}

/**
 * Sets the figure's bounds.
 */
void
SocketPortConnFigure::layoutSelf(wxDC*) {
    // assumes that port is always "source"
    int offset = MachineCanvasLayoutConstraints::PORT_WIDTH / 2;    
    setHeight(target_->location().y - source_->location().y - offset*2);
    if(source_->location().x < target_->location().x) {
        setWidth(target_->location().x - source_->location().x + 3);
        setLocation(
            wxPoint(source_->location().x - 1 + offset,
                    source_->location().y + offset*2));

    } else {
        setWidth(source_->location().x - target_->location().x + 3);
        setLocation(
            wxPoint(target_->location().x - 1 + offset,
                    source_->location().y + offset*2));
    }
}
