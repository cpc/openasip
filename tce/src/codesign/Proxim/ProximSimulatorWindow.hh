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
