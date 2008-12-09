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
 * @file ProximMemoryWindow.cc
 *
 * Declaration of ProximMemoryWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
