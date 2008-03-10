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
