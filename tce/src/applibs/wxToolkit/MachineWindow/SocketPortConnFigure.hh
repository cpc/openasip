/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SocketPortConnFigure.hh
 *
 * Declaration of SocketPortConnFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
