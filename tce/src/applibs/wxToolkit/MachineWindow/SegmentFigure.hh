/**
 * @file SegmentFigure.hh
 *
 * Declaration of SegmentFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_SEGMENT_FIGURE_HH
#define TTA_SEGMENT_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a segment.
 */
class SegmentFigure : public Figure {
public:
    SegmentFigure();
    virtual ~SegmentFigure();

    void setLast(bool last);

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    SegmentFigure& operator=(SegmentFigure& old);
    /// Copying not allowed.
    SegmentFigure(SegmentFigure& old);

    /// True if segment is the last segment in a bus.
    bool last_;

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
};

#include "SegmentFigure.icc"

#endif
