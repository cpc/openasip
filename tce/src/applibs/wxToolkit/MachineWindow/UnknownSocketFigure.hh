/**
 * @file UnknownSocketFigure.hh
 *
 * Declaration of UnknownSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_UNKNOWN_SOCKET_FIGURE_HH
#define TTA_UNKNOWN_SOCKET_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a socket with unknown direction.
 */
class UnknownSocketFigure : public Figure {
public:
    UnknownSocketFigure();
    virtual ~UnknownSocketFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    UnknownSocketFigure& operator=(UnknownSocketFigure& old);
    /// Copying not allowed.
    UnknownSocketFigure(UnknownSocketFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;
};

#endif
