/**
 * @file UnitPortFigure.hh
 *
 * Declaration of UnitPortFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_UNIT_PORT_FIGURE_HH
#define TTA_UNIT_PORT_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a port.
 */
class UnitPortFigure : public Figure {
public:
    UnitPortFigure(std::string name);
    virtual ~UnitPortFigure();
    std::string name() const;

protected:
    virtual void layoutChildren(wxDC*);
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    UnitPortFigure& operator=(UnitPortFigure& old);
    /// Copying not allowed.
    UnitPortFigure(UnitPortFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;

    /// Name of the port.
    std::string name_;
};

#endif
