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
 * @file ProximDisassemblyWindow.cc
 *
 * Declaration of ProximDisassemblyWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_DISASSEMBLY_WINDOW_HH
#define TTA_PROXIM_DISASSEMBLY_WINDOW_HH

#include <map>
#include <wx/wx.h>
#include <wx/grid.h>
#include "SimulatorEvent.hh"
#include "ProximSimulatorWindow.hh"
#include "Program.hh"

class ProximMainFrame;
class ProximDisassemblyGridTable;
class ProximDisasmAttrProvider;

/**
 * A window which can be utilized to display disassembly of a program object
 * model.
 *
 * This class listens to SimulatorEvents and updates the window information
 * automatically.
 */
class ProximDisassemblyWindow : public ProximSimulatorWindow {
public:
    ProximDisassemblyWindow(ProximMainFrame* parent, int id);
    virtual ~ProximDisassemblyWindow();
    virtual void reset();
    void loadProgram(const TTAProgram::Program& program);
    void showAddress(unsigned address);
    void setMoveAttrProvider(ProximDisasmAttrProvider* attrProvider);

private:
    void initialize();
    void resetGrid();
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulatorStop(const SimulatorEvent& event);
    void onSimulatorCommand(SimulatorEvent& event);
    void onRightClick(wxGridEvent& event);
    void onSetBreakpoint(wxCommandEvent& event);
    void onSetTempBp(wxCommandEvent& event);
    void onRunUntil(wxCommandEvent& event);
    void onMappedMenuCommand(wxCommandEvent& event);

    /// The grid disapleying the disassembly.
    wxGrid* codeGrid_;
    /// Grid table which handles on-the-fly disassembly of the loaded program.
    ProximDisassemblyGridTable* codeTable_;
    
    /// A map which contains command strings for context menu items.
    std::map<unsigned, std::string> menuCommand_;

    /// Width of the column displaying the arrow pointign current instruction.
    static const unsigned INFO_COLUMN_WIDTH;
    /// Width of the instruction address column.
    static const unsigned ADDRESS_COLUMN_WIDTH;
    /// Width of the breakpoint info column.
    static const unsigned BP_COLUMN_WIDTH;
    /// Width of the instruction disassembly column.
    static const unsigned DISASSEMBLY_COLUMN_WIDTH;

    /// Context menu command IDs.
    enum {
	MENU_ID_SET_BP = 10000,
        MENU_ID_SET_TEMP_BP,
        MENU_ID_RUN_UNTIL,
	MENU_ID_FIRST_MAPPED // Keep this as the last menu item.
    };

    DECLARE_EVENT_TABLE()
};

#endif
