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
 * @file MachineCanvasTool.hh
 *
 * Declaration of MachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
 
#ifndef TTA_MACHINE_CANVAS_TOOL_HH
#define TTA_MACHINE_CANVAS_TOOL_HH

class wxMouseEvent;
class wxDC;
class Figure;
class MachineCanvas;

/**
 * Base class for the mouse tools used on the MachineCanvas.
 *
 * MachineCanvas and MachineCanvasTool follow the 'state' -design pattern.
 * MachineCanvas has always an active tool, which handles the mouse events
 * on the canvas, such as mouse button clicks. The actual tools handling the
 * mouse events are subclasses of this interface class.
 */
class MachineCanvasTool {
public:
    virtual ~MachineCanvasTool();

    /**
     * Activate the tool.
     */
    virtual void activate() = 0;

    /**
     * Deactivate the tool.
     */
    virtual void deactivate() = 0;

    /**
     * Handle the mouse events of the canvas in a tool-specific way.
     *
     * @param event Mouse event to handle.
     * @param dc Device context.
     */
    virtual void onMouseEvent(wxMouseEvent& event, wxDC& dc) = 0;

    virtual Figure* figure();

protected:
    MachineCanvasTool(MachineCanvas* canvas);

    /// Machine canvas where the tool is used.
    MachineCanvas* canvas_;
};
#endif
