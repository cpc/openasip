/**
 * @file TriggeringPortFigure.hh
 *
 * Declaration of TriggeringPortFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRIGGERING_PORT_FIGURE_HH
#define TTA_TRIGGERING_PORT_FIGURE_HH

#include <wx/wx.h>

#include "UnitPortFigure.hh"

class wxDC;

/**
 * Figure of a triggering function unit port.
 */
class TriggeringPortFigure : public UnitPortFigure {
public:
    TriggeringPortFigure(std::string name);
    virtual ~TriggeringPortFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    TriggeringPortFigure& operator=(TriggeringPortFigure& old);
    /// Copying not allowed.
    TriggeringPortFigure(TriggeringPortFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;
};

#endif
