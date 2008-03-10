/**
 * @file BusFigure.hh
 *
 * Declaration of BusFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_BUS_FIGURE_HH
#define TTA_BUS_FIGURE_HH

#include <wx/wx.h>
#include <vector>

#include "Figure.hh"

class wxDC;
class wxColour;

/**
 * Figure of a bus.
 */
class BusFigure : public Figure {
public:
    BusFigure(unsigned int slot);
    virtual ~BusFigure();

    virtual wxRect virtualBounds() const;

protected:
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);
    virtual void drawSelf(wxDC*);

private:
    /// Assignment not allowed.
    BusFigure& operator=(BusFigure& old);
    /// Copying not allowed.
    BusFigure(BusFigure& old);

    /// Slot of the bus.
    unsigned int slot_;
    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
};

#endif
