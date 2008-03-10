/**
 * @file SocketPortConnFigure.hh
 *
 * Declaration of SocketPortConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed Jul 27 2004 by ml, pj, am
 * @note rating: yellow
 */

#ifndef TTA_SOCKET_PORT_CONN_FIGURE_HH
#define TTA_SOCKET_PORT_CONN_FIGURE_HH

#include <wx/wx.h>

#include "ConnectionFigure.hh"

class wxDC;

/**
 * Figure of a socket-to-port connection.
 */
class SocketPortConnFigure : public ConnectionFigure {
public:
    SocketPortConnFigure();
    virtual ~SocketPortConnFigure();

protected:
    virtual void drawSelf(wxDC* dc);
    virtual void layoutSelf(wxDC* dc);
    virtual void drawConnection(wxDC* dc);
private:
    /// Assignment not allowed.
    SocketPortConnFigure& operator=(SocketPortConnFigure& old);
    /// Copying not allowed.
    SocketPortConnFigure(SocketPortConnFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
};

#endif
