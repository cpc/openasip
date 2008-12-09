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
 * @file ContentsFigure.hh
 *
 * Declaration of ContentsFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_CONTENTS_FIGURE_HH
#define TTA_CONTENTS_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;
class UnitContainerFigure;
class BusContainerFigure;
class SocketContainerFigure;

/**
 * Figure of a container EditPart for the whole machine.
 */
class ContentsFigure : public Figure {
public:
    ContentsFigure();
    virtual ~ContentsFigure();

    virtual void addChild(Figure* child);

protected:
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);
    virtual void draw(wxDC* dc);
private:
    /// Assignment not allowed.
    ContentsFigure& operator=(ContentsFigure& old);
    /// Copying not allowed.
    ContentsFigure(ContentsFigure& old);

    /// Contains all the units.
    UnitContainerFigure* units_;
    /// Contains all the buses.
    BusContainerFigure* buses_;
    /// Contains all the sockets.
    SocketContainerFigure* sockets_;
};

#endif
