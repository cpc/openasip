/**
 * @file ProximDisassemblyWindow.cc
 *
 * Definition of ProximDisassemblyWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#include <string>

#include "ProximDisassemblyWindow.hh"
#include "POMDisassembler.hh"
#include "WxConversion.hh"
#include "DisassemblyInstruction.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"
#include "StopPoint.hh"
#include "ProximConstants.hh"
#include "AssocTools.hh"
#include "Conversion.hh"
#include "NullInstruction.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyGridTable.hh"
#include "ProximToolbox.hh"
#include "DisasmTopCountAttrProvider.hh"

using namespace TTAProgram;

BEGIN_EVENT_TABLE(ProximDisassemblyWindow, ProximSimulatorWindow)
    EVT_SIMULATOR_PROGRAM_LOADED(0, ProximDisassemblyWindow::onProgramLoaded)
    EVT_SIMULATOR_COMMAND_DONE(
	0, ProximDisassemblyWindow::onSimulatorCommand)
    EVT_SIMULATOR_STOP(0, ProximDisassemblyWindow::onSimulatorStop)
    EVT_GRID_CELL_RIGHT_CLICK(ProximDisassemblyWindow::onRightClick)
    EVT_MENU(MENU_ID_SET_BP, ProximDisassemblyWindow::onSetBreakpoint)
    EVT_MENU(MENU_ID_SET_TEMP_BP, ProximDisassemblyWindow::onSetTempBp)
    EVT_MENU(MENU_ID_RUN_UNTIL, ProximDisassemblyWindow::onRunUntil)
    EVT_MENU_RANGE(MENU_ID_FIRST_MAPPED, MENU_ID_FIRST_MAPPED + 100,
		   ProximDisassemblyWindow::onMappedMenuCommand)
END_EVENT_TABLE()

using std::string;

const unsigned ProximDisassemblyWindow::INFO_COLUMN_WIDTH = 30;
const unsigned ProximDisassemblyWindow::ADDRESS_COLUMN_WIDTH = 60;
const unsigned ProximDisassemblyWindow::BP_COLUMN_WIDTH = 80;
const unsigned ProximDisassemblyWindow::DISASSEMBLY_COLUMN_WIDTH = 400;


/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximDisassemblyWindow::ProximDisassemblyWindow(
    ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id),
    codeGrid_(NULL),
    codeTable_(NULL) {

    initialize();
}


/**
 * Destructor.
 */
ProximDisassemblyWindow::~ProximDisassemblyWindow() {
}


/**
 * Creates the window contents.
 */
void
ProximDisassemblyWindow::initialize() {

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
}


/**
 * Resets the grid. This function has to be called if a new program
 * is loaded to the window.
 */
void
ProximDisassemblyWindow::resetGrid() {

    wxSizer* sizer = GetSizer();

    if (codeGrid_ != NULL) {
        sizer->Detach(codeGrid_);
        sizer->Layout();
        codeGrid_->Destroy();
    }

    codeGrid_ = new wxGrid(this, -1, wxDefaultPosition, wxDefaultSize);
    codeGrid_->SetTable(codeTable_);

    codeGrid_->EnableEditing(false);
    codeGrid_->DisableCellEditControl();
    codeGrid_->SetSelectionMode(wxGrid::wxGridSelectRows);
    codeGrid_->DisableDragGridSize();
    codeGrid_->DisableDragRowSize();
    codeGrid_->EnableGridLines(false);
    codeGrid_->SetDefaultColSize(200);

    codeGrid_->SetColSize(0, 100);
    codeGrid_->SetColSize(1, 80);

    codeGrid_->SetDefaultCellFont(*wxSMALL_FONT);
    codeGrid_->SetGridCursor(0, 0);

    sizer->Add(codeGrid_, 1, wxGROW);

    Layout();
}



/**
 * Called when the simulator program, machine and memory models are deleted.
 */
void
ProximDisassemblyWindow::reset() {

    wxSizer* sizer = GetSizer();

    if (codeGrid_ != NULL) {
        sizer->Detach(codeGrid_);
        sizer->Layout();
        codeGrid_->Destroy();
        codeGrid_ = NULL;
    }

    if (codeTable_ != NULL) {
        delete codeTable_;
        codeTable_ = NULL;
    }
}


/**
 * Loads a program model to the window.
 *
 * @param program Program to load.
 */
void
ProximDisassemblyWindow::loadProgram(const Program& program) {
    assert(codeTable_ == NULL);
    codeTable_ = new ProximDisassemblyGridTable();

    codeTable_->loadProgram(program);
    resetGrid();
    codeTable_->setCurrentInstruction(0);
    codeTable_->showPCArrow();
    StopPointManager& bpManager =
        ProximToolbox::frontend()->stopPointManager();
    codeTable_->setStopPointManager(bpManager);
}


/**
 * Updates disassembly window when a program is loaded to the
 * simulator.
 */
void
ProximDisassemblyWindow::onProgramLoaded(const SimulatorEvent&) {
    loadProgram(ProximToolbox::program());
}


/**
 * Handles RMB clicks on the disassembly window.
 *
 * A context menu is displayed at the cursor location.
 */
void
ProximDisassemblyWindow::onRightClick(wxGridEvent& event) {

    codeGrid_->SelectRow(event.GetRow());

    wxMenu* contextMenu = new wxMenu();
    Word address = codeTable_->addressOfRow(event.GetRow());
    contextMenu->Append(MENU_ID_SET_BP, _T("Set breakpoint"));
    contextMenu->Append(
        MENU_ID_SET_TEMP_BP, _T("Set temporary breakpoint"));
    
    contextMenu->AppendSeparator();
    contextMenu->Append(MENU_ID_RUN_UNTIL, _T("Run until"));
    contextMenu->AppendSeparator();

    menuCommand_.clear();

    StopPointManager& bpManager =
        ProximToolbox::frontend()->stopPointManager();

    // Create submenu for each breakpoint at the selected line.
    unsigned itemID = MENU_ID_FIRST_MAPPED;
    for (unsigned i = 0; i < bpManager.stopPointCount(); i++) {

	unsigned handle = bpManager.stopPointHandle(i);
	const Breakpoint* bp = dynamic_cast<const Breakpoint*>(
                &bpManager.stopPointWithHandleConst(handle));

        if (bp != NULL) {
            unsigned bpAddress = bp->address();
            if (bpAddress == address) {

                wxMenu* bpSubMenu = new wxMenu();
                string handleStr = Conversion::toString(handle);

                // Info menuitem.
                menuCommand_[itemID] = "info breakpoints " + handleStr;
                wxString infoLabel = _T("info");
                bpSubMenu->Append(itemID, infoLabel);
                itemID++;


                // Delete menuitem.
                menuCommand_[itemID] = "deletebp " + handleStr;
                wxString deleteLabel = _T("delete");
                bpSubMenu->Append(itemID, deleteLabel);
                itemID++;

                // Enable/disable menuitem.
                if (bp->isEnabled()) {
                    menuCommand_[itemID] =
                        "disablebp " + handleStr;
                    wxString disableLabel = _T("disable");
                    bpSubMenu->Append(itemID, disableLabel);
                } else {
                    menuCommand_[itemID] =
                        "enablebp " + handleStr;
                    wxString enableLabel = _T("enable");
                    bpSubMenu->Append(itemID, enableLabel);
                }
                itemID++;
                wxString menuLabel =
                    _T("breakpoint ") + WxConversion::toWxString(handle);

                contextMenu->Append(0, menuLabel, bpSubMenu);
            }
        }
    }

    PopupMenu(contextMenu, event.GetPosition());
}



/**
 * Handles the event of adding new breakpoint with the context menu.
 */
void
ProximDisassemblyWindow::onSetBreakpoint(wxCommandEvent&) {
    unsigned address =
        codeTable_->addressOfRow(codeGrid_->GetSelectedRows().Item(0));
    string command = ProximConstants::SCL_SET_BREAKPOINT + " " +
        Conversion::toString(address);

    wxGetApp().simulation()->lineReader().input(command);
}


/**
 * Handles the event of adding new temporary breakpoint with the context menu.
 */
void
ProximDisassemblyWindow::onSetTempBp(wxCommandEvent&) {
    unsigned address =
        codeTable_->addressOfRow(codeGrid_->GetSelectedRows().Item(0));
    string command = ProximConstants::SCL_SET_TEMP_BP + " " +
        Conversion::toString(address);

    wxGetApp().simulation()->lineReader().input(command);
}


/**
 * Handles the context menu "run until" clicks.
 */
void
ProximDisassemblyWindow::onRunUntil(wxCommandEvent&) {
    unsigned address =
        codeTable_->addressOfRow(codeGrid_->GetSelectedRows().Item(0));
    string command = ProximConstants::SCL_RUN_UNTIL + " " +
        Conversion::toString(address);

    wxGetApp().simulation()->lineReader().input(command);
}

/**
 * Updates the arrow displaying the current instruction when the simulation
 * stops.
 */
void
ProximDisassemblyWindow::onSimulatorStop(const SimulatorEvent&) {
    Word pc = wxGetApp().simulation()->frontend()->programCounter();
    if (codeTable_->moveCellAttrProvider() != NULL) {
        codeTable_->moveCellAttrProvider()->update();
    }
    codeTable_->setCurrentInstruction(pc);
    codeGrid_->SelectRow(pc);
    codeGrid_->MakeCellVisible(pc, 1);
    codeGrid_->ForceRefresh();
}

/**
 * Updates the disassembly window when a command is executed.
 *

 * @param event Event of a command execution.
 */
void
ProximDisassemblyWindow::onSimulatorCommand(SimulatorEvent& event) {
    if (codeGrid_ != NULL) {
        codeGrid_->ForceRefresh();
    }
    event.Skip();
}

/**
 * Selects the instruction at given address.
 *
 * @param address Address of the isntruction to show.
 */
void
ProximDisassemblyWindow::showAddress(unsigned address) {
    unsigned row = codeTable_->rowOfAddress(address);
    codeGrid_->SelectRow(row);
    codeGrid_->MakeCellVisible(row, 0);

}


/**
 * Event handler for breakpoint submenus of the context menu.
 *
 * @param event Menu event to handle.
 */
void
ProximDisassemblyWindow::onMappedMenuCommand(wxCommandEvent& event) {

    if (!AssocTools::containsKey(menuCommand_, event.GetId())) {
	// Menu item ID doesn't have command associated, the event is skipped.
	event.Skip();
    }

    string command = menuCommand_[event.GetId()];
    wxGetApp().simulation()->lineReader().input(command);
}


/**
 * Sets the move cell attribute provider.
 *
 *  @param attrProvider New attribute provider.
 */
void
ProximDisassemblyWindow::setMoveAttrProvider(
    ProximDisasmAttrProvider* attrProvider) {

    codeTable_->setMoveCellAttrProvider(attrProvider);
}
