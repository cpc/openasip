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
 * @file MachineCanvas.hh
 *
 * Declaration of MachineCanvas class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_CANVAS_HH
#define TTA_MACHINE_CANVAS_HH

#include <vector>
#include <wx/scrolwin.h>
#include <wx/docmdi.h>
#include "Exception.hh"
#include "MachineCanvasOptions.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
    class Bus;
    class Port;
}

class MachineCanvasTool;
class RootEditPart;
class EditPolicyFactory;
class EditPart;
class Figure;

/**
 * Machine Object Model visualization canvas.
 *
 * MachineCanvas is a window which can be utilized to display visualization
 * of a machine object model. The visualization is interactive, and behaviour
 * of machine components can be defined using custom EditPolicies and custom
 * MachineCanvasTools.
 */
class MachineCanvas : public wxScrolledWindow {
public:
    MachineCanvas(wxWindow* parent, EditPolicyFactory* policyFactory);
    virtual ~MachineCanvas();
    virtual void OnDraw(wxDC& dc);

    double zoomFactor();
    void setZoomFactor(double factor);

    void setTool(MachineCanvasTool* tool);
    MachineCanvasTool* tool();

    void setMachine(TTAMachine::Machine* machine);
    void updateMachine();
    EditPart* selection();
    EditPart* findEditPart(int x, int y);
    EditPart* findEditPart(const TTAMachine::MachinePart* model);
    void highlight(TTAMachine::MachinePart* model, const wxColour& colour);
    void clearHighlights();
    void clearSelection();
    void select(EditPart* part);
    void refreshToolFigure();

    void addMove(
        const TTAMachine::Bus* bus,
        const TTAMachine::Port* source,
        const TTAMachine::Port* target) throw (InstanceNotFound);
    void clearMoves();

    bool saveEPS(
        const std::string& filename,
        const std::string& title,
        const std::string& creator = "");

    bool savePNG(const std::string& filename);

    MachineCanvasOptions& options();

protected:
    void onMouseEvent(wxMouseEvent& event);
    void refreshLogicalRect(const wxRect& rectangle);

private:   
    /// Tool which handles mouse events on the MachineCanvas.
    MachineCanvasTool* tool_;
    /// Machine to draw on the canvas.
    TTAMachine::Machine* machine_;
    /// EditPolicyFactory which creates edit policies for machine components.
    EditPolicyFactory* editPolicyFactory_;

    /// Zoom factor of the canvas.
    double zoomFactor_;
    /// Tells if the figures need to be laid out before next draw.
    bool dirty_;
    /// Root of the machine EditPart hierarchy.
    RootEditPart* root_;

    /// Pointer to the the selected component.
    EditPart* selection_;
    /// Bounding box of the last tool figure drawn.
    wxRect toolBounds_;

    /// Move figures to be drawn on the canvas.
    std::vector<Figure*> moveFigures_;

    MachineCanvasOptions options_;

    /// Event table of the canvas.
    DECLARE_EVENT_TABLE()
};
#endif
