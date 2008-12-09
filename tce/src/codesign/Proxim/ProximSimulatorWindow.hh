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
 * @file ProximSimulatorWindow.hh
 *
 * Declaration of ProximSimulatorWindow class.
 *
 * @note rating: red
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_PROXIM_SIMULATOR_WINDOW_HH
#define TTA_PROXIM_SIMULATOR_WINDOW_HH

#include <wx/panel.h>

class ProximMainFrame;
class SimulatorEvent;

/**
 * Base class for Proxim windows which listen to the simulator events.
 *
 * This baseclass handles the adding and removing of the window from the
 * ProximMainFrame's list of subwindows automatically.
 */
class ProximSimulatorWindow : public wxPanel {
public:
    virtual void reset();
protected:
    ProximSimulatorWindow(
        ProximMainFrame* mainFrame,
        wxWindowID id = -1,
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL);
    virtual ~ProximSimulatorWindow();
private:
    virtual void onSimulatorBusy(SimulatorEvent& event);
    virtual void onSimulatorDone(SimulatorEvent& event);
    ProximSimulatorWindow(const ProximSimulatorWindow&);
    ProximSimulatorWindow& operator=(const ProximSimulatorWindow&);

    DECLARE_EVENT_TABLE()
};

#endif
