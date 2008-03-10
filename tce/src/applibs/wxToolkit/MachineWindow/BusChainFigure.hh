/**
 * @file BusChainFigure.hh
 *
 * Declaration of BusChainFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_BUS_CHAIN_FIGURE_HH
#define TTA_BUS_CHAIN_FIGURE_HH

#include <wx/wx.h>
#include <vector>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a bus chain.
 */
class BusChainFigure : public Figure {
public:
    BusChainFigure();
    virtual ~BusChainFigure();

protected:
    virtual void layoutChildren(wxDC*);
    virtual void drawSelf(wxDC*);

private:
    /// Assignment not allowed.
    BusChainFigure& operator=(BusChainFigure& old);
    /// Copying not allowed.
    BusChainFigure(BusChainFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
};

#endif
