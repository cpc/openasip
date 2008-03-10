/**
 * @file UnitFigure.hh
 *
 * Declaration of UnitFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_UNIT_FIGURE_HH
#define TTA_UNIT_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a unit.
 */
class UnitFigure : public Figure {
public:
    UnitFigure();
    virtual ~UnitFigure();

    void setName(const wxString& name);
    void setInfo(const wxString& info);

protected:
    virtual void layout(wxDC*);
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);
    virtual void drawSelf(wxDC*);

private:
    /// Assignment not allowed.
    UnitFigure& operator=(UnitFigure& old);
    /// Copying not allowed.
    UnitFigure(UnitFigure& old);

    /// Type identifier of the unit.
    wxString type_;
    /// Name of the unit.
    wxString name_;
    /// Extra information string describing the unit.
    wxString info_;

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;
    /// Default info string colour for the figure.
    static const wxColour DEFAULT_INFO_TEXT_COLOUR;
    /// Top and bottom margin in pixels.
    static const int MARGIN;
    /// Space between text rows in pixels.
    static const int SPACING;
};

#endif
