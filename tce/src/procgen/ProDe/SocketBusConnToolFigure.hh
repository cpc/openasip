/**
 * @file SocketBusConnToolFigure.hh
 *
 * Declaration of SocketBusConnToolFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_BUS_CONN_TOOL_FIGURE_HH
#define TTA_SOCKET_BUS_CONN_TOOL_FIGURE_HH

#include <wx/wx.h>

#include "SocketBusConnFigure.hh"

class wxDC;

/**
 * Figure of a socket-to-bus connection for the connection tool.
 */
class SocketBusConnToolFigure : public SocketBusConnFigure {
public:
    SocketBusConnToolFigure(bool creating);
    virtual ~SocketBusConnToolFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    SocketBusConnToolFigure& operator=(SocketBusConnToolFigure& old);
    /// Copying not allowed.
    SocketBusConnToolFigure(SocketBusConnToolFigure& old);

    /// Figure color for connections being removed
    static const wxColor LINE_COLOR_RED;
    /// Figure color for connections being removed.
    static const wxColor LINE_COLOR_GREEN;

    /// Current color of the connection figure.
    wxColor color_;
};

#endif
