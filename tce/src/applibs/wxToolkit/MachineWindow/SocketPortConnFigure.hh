/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
