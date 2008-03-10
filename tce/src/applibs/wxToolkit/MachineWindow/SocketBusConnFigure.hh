/**
 * @file SocketBusConnFigure.hh
 *
 * Declaration of SocketBusConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_SOCKET_BUS_CONN_FIGURE_HH
#define TTA_SOCKET_BUS_CONN_FIGURE_HH

#include <wx/wx.h>

#include "ConnectionFigure.hh"

class wxDC;

/**
 * Figure of a socket-to-bus connection.
 */
class SocketBusConnFigure : public ConnectionFigure {
public:
    SocketBusConnFigure();
    virtual ~SocketBusConnFigure();

protected:
    virtual void drawSelf(wxDC* dc);
    virtual void drawConnection(wxDC* dc);
    virtual void layoutSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    SocketBusConnFigure& operator=(SocketBusConnFigure& old);
    /// Copying not allowed.
    SocketBusConnFigure(SocketBusConnFigure& old);

    /// Default colour for the figure.
    static const wxColour DEFAULT_COLOUR;
};

#endif
