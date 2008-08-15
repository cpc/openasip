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
 * @file MachineCanvasTool.hh
 *
 * Declaration of MachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
