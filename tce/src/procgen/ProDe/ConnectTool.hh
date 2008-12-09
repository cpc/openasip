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
 * @file ConnectTool.hh
 *
 * Declaration of ConnectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONNECT_TOOL_HH
#define TTA_CONNECT_TOOL_HH

#include "MachineCanvasTool.hh"

class MDFView;
class EditPart;
class Figure;
class ChildFrame;

/**
 * Mouse tool for creating and removing connections between ports, sockets
 * and bus segments.
 */
class ConnectTool : public MachineCanvasTool {
public:
    ConnectTool(ChildFrame* frame, MDFView* view);
    virtual ~ConnectTool();
    virtual void activate();
    virtual void deactivate();
    virtual void onMouseEvent(wxMouseEvent& event, wxDC& dc);
    virtual Figure* figure();
private:
    void updateStatusline(EditPart* part);
    void leftClick(EditPart* part);
    void rightClick(wxMouseEvent& event);

    /// Parent frame of the canvas.
    ChildFrame* frame_;
    /// View displayed on the Canvas.
    MDFView* view_;
    /// Tells if the tool is active or not.
    bool active_;
    /// Target EditPart of the connection.
    EditPart* target_;
    /// Connection figure.
    Figure* figure_;
};

#endif
