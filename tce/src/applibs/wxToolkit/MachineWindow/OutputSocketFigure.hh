/**
 * @file OutputSocketFigure.hh
 *
 * Declaration of OutputSocketFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_OUTPUT_SOCKET_FIGURE_HH
#define TTA_OUTPUT_SOCKET_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of an output socket.
 */
class OutputSocketFigure : public Figure {
public:
    OutputSocketFigure();
    virtual ~OutputSocketFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    OutputSocketFigure& operator=(OutputSocketFigure& old);
    /// Copying not allowed.
    OutputSocketFigure(OutputSocketFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Default background colour for the figure.
    static const wxColour DEFAULT_BG_COLOUR;
};

#endif
