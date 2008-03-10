/**
 * @file BridgeFigure.hh
 *
 * Declaration of BridgeFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_BRIDGE_FIGURE_HH
#define TTA_BRIDGE_FIGURE_HH

#include <wx/wx.h>

#include "ConnectionFigure.hh"

class wxDC;

/**
 * Figure of a bridge.
 */
class BridgeFigure : public ConnectionFigure {
public:
    /// Direction of the bridge.
    enum Direction {DIR_RIGHT, DIR_LEFT, DIR_BIDIR};

    BridgeFigure();
    virtual ~BridgeFigure();

    void setDirection(BridgeFigure::Direction direction);
    Direction direction() const;

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    BridgeFigure& operator=(BridgeFigure& old);
    /// Copying not allowed.
    BridgeFigure(BridgeFigure& old);

    /// Direction of the bridge.
    Direction direction_;

    /// Default colour for figure lines.
    static const wxColour DEFAULT_COLOUR;
    /// Default colour for figure background.
    static const wxColour DEFAULT_BG_COLOUR;
};

#include "BridgeFigure.icc"

#endif
