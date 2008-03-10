/**
 * @file ConnectTool.hh
 *
 * Declaration of ConnectTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
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
