/**
 * @file SelectTool.hh
 *
 * Declaration of SelectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
