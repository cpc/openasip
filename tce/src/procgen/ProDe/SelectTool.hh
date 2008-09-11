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
 * @file SelectTool.hh
 *
 * Declaration of SelectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SELECT_TOOL_HH
#define TTA_SELECT_TOOL_HH

#include "MachineCanvasTool.hh"

class MDFView;
class ChildFrame;
class EditPart;
class wxMenu;
class SelectionFigure;

/**
 * Tool for selecting components in the canvas.
 */
class SelectTool : public MachineCanvasTool {
public:
    SelectTool(ChildFrame* frame, MDFView* view);
    virtual ~SelectTool();
    virtual void activate();
    virtual void deactivate();
    virtual void onMouseEvent(wxMouseEvent& event, wxDC& dc);
    virtual Figure* figure();

private:
    void leftClick(EditPart* part);
    void popContextMenu(EditPart* part, int x, int y);
    wxMenu* createDefaultMenu();
    /// Parent frame of the Canvas.
    ChildFrame* frame_;
    /// View displayed on the Canvas.
    MDFView* view_;
    /// Tells if the tool is active or not.
    bool active_;
    /// Selection figure;
    SelectionFigure* figure_;
    /// Current selection.
    EditPart* selection_;

    /// IDs for the context menu items.
    enum {
	ID_CONTEXT_MODIFY = 11000,
	ID_CONTEXT_DELETE,
	ID_CONTEXT_COPY,
	ID_CONTEXT_CUT,
	ID_ADD_SUBMENU
    };
};

#endif
