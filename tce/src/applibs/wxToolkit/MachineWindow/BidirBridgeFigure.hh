/**
 * @file BidirBridgeFigure.hh
 *
 * Declaration of BidirBridgeFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed Jul 27 2004 by ml, pj, am
 * @note rating: yellow
 */

#ifndef TTA_BIDIR_BRIDGE_FIGURE_HH
#define TTA_BIDIR_BRIDGE_FIGURE_HH

#include <wx/wx.h>

#include "BridgeFigure.hh"

class wxDC;

/**
 * Figure of a bidirectional bridge.
 */
class BidirBridgeFigure : public BridgeFigure {
public:
    BidirBridgeFigure();
    virtual ~BidirBridgeFigure();

protected:
    virtual void drawSelf(wxDC* dc);
    virtual void layoutChildren(wxDC*);

private:
    /// Assignment not allowed.
    BidirBridgeFigure& operator=(BidirBridgeFigure& old);
    /// Copying not allowed.
    BidirBridgeFigure(BidirBridgeFigure& old);
};

#endif
