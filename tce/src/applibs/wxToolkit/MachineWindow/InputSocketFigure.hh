/**
 * @file InputSocketFigure.hh
 *
 * Declaration of InputSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_INPUT_SOCKET_FIGURE_HH
#define TTA_INPUT_SOCKET_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of an input socket.
 */
class InputSocketFigure : public Figure {
public:
    InputSocketFigure();
    virtual ~InputSocketFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    InputSocketFigure& operator=(InputSocketFigure& old);
    /// Copying not allowed.
    InputSocketFigure(InputSocketFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;
};

#endif
