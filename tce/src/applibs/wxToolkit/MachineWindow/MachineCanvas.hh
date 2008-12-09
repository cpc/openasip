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
