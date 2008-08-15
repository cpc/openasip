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
 * @file ConnectionFigure.hh
 *
 * Declaration of ConnectionFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_CONNECTION_FIGURE_HH
#define TTA_CONNECTION_FIGURE_HH

#include "Figure.hh"

class wxDC;

/**
 * Figure of a connection.
 */
class ConnectionFigure : public Figure {
public:
    virtual ~ConnectionFigure();

    void setSource(Figure* figure);
    void setTarget(Figure* figure);
    Figure* source() const;
    Figure* target() const;

protected:
    ConnectionFigure();
    /// Source of connection.
    Figure* source_;
    /// Target of connection.
    Figure* target_;

private:
    /// Assignment not allowed.
    ConnectionFigure& operator=(ConnectionFigure& old);
    /// Copying not allowed.
    ConnectionFigure(ConnectionFigure& old);

};

#include "ConnectionFigure.icc"

#endif
