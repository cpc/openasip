/**
 * @file ProximMachineCanvasTool.hh
 *
 * Declaration of ProximMachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_MACHINE_CANVAS_TOOL_HH
#define TTA_PROXIM_MACHINE_CANVAS_TOOL_HH

#include "MachineCanvasTool.hh"

class MachineCanvas;
class EditPart;
class wxTipWindow;

/**
 * MachineCanvasTool for Proxim MachineWindow.
 */
class ProximMachineCanvasTool : public MachineCanvasTool {
public:
    ProximMachineCanvasTool(MachineCanvas* canvas);
    virtual ~ProximMachineCanvasTool();
    virtual void activate();
    virtual void deactivate();
    virtual void onMouseEvent(wxMouseEvent& event, wxDC& dc);

private:
    void popupMenu(int x, int y);
    /// MachineCanvas where the tools is used.
    MachineCanvas* canvas_;

    wxTipWindow* toolTip_;
};

#endif
