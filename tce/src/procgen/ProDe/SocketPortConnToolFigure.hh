/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SocketPortConnToolFigure.hh
 *
 * Declaration of SocketPortConnToolFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
