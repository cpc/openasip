/**
 * @file SocketPortConnToolFigure.hh
 *
 * Declaration of SocketPortConnToolFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_PORT_CONN_TOOL_FIGURE_HH
#define TTA_SOCKET_PORT_CONN_TOOL_FIGURE_HH

#include <wx/wx.h>

#include "SocketPortConnFigure.hh"

class wxDC;

/**
 * Figure of a socket-to-port connection for the connection tool.
 */
class SocketPortConnToolFigure : public SocketPortConnFigure {
public:
    SocketPortConnToolFigure(bool creating);
    virtual ~SocketPortConnToolFigure();

protected:
    virtual void drawSelf(wxDC* dc);
private:
    /// Assignment not allowed.
    SocketPortConnToolFigure& operator=(SocketPortConnToolFigure& old);
    /// Copying not allowed.
    SocketPortConnToolFigure(SocketPortConnToolFigure& old);

    /// Figure color for connections being created.
    static const wxColor LINE_COLOR_GREEN;
    /// Figure color for connections being removed.
    static const wxColor LINE_COLOR_RED;
    /// Current color of the figure.
    wxColor color_;
};

#endif
