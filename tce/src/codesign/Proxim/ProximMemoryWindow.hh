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
 * @file ProximMemoryWindow.cc
 *
 * Declaration of ProximMemoryWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_MEMORY_WINDOW_HH
#define TTA_PROXIM_MEMORY_WINDOW_HH

#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"
#include "AddressSpace.hh"

class MemorySystem;
class MemoryControl;
class SimulatorFrontend;

/**
 * Proxim subwindow which displays memory contents.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximMemoryWindow : public ProximSimulatorWindow {
public:
    ProximMemoryWindow(ProximMainFrame* parent, int id);
    virtual ~ProximMemoryWindow();
    virtual void reset();
private:
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void loadProgramMemory();
    void loadMemory(const TTAMachine::AddressSpace& as);
    void initialize();
    void onASChoice(wxCommandEvent& event);
    void onDisplayRangeChoice(wxCommandEvent& event);
    void createContents();

    /// Memory system to display in teh window.
    MemorySystem* memorySystem_;
    /// MemoryControl widget which displays the memory contents.
    MemoryControl* memoryControl_;
    /// Simulator instance which contains the memory system to display.
    SimulatorFrontend* simulator_;
    /// Toplevel sizer for the window widgets.
    wxBoxSizer* sizer_;
    /// Address space choicer widget.
    wxChoice* asChoice_;
    /// Sizer for address range widgets.
    wxStaticBoxSizer* addressRangeSizer_;
    /// Static text control displaying address space information.
    wxStaticText* asInfoText_;

    // Widget IDs.
    enum {
	ID_AS_CHOICE = 10000,
	ID_TEXT_AS,
        ID_TEXT_AS_INFO,
	ID_TEXT_START_ADDRESS,
	ID_START_ADDRESS,
	ID_TEXT_DISPLAY,
	ID_DISPLAY_SIZE_CHOICE,
	ID_LINE
    };
    DECLARE_EVENT_TABLE()
};

#endif
