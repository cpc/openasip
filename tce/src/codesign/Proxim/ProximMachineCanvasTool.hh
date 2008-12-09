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
 * @file ProximMachineCanvasTool.hh
 *
 * Declaration of ProximMachineCanvasTool class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
