/**
 * @file DisasmTopCountAttrProvider.cc
 *
 * Implementation of DisasmTopCountAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/minifram.h>
#include <wx/grid.h>
#include "DisasmTopCountAttrProvider.hh"
#include "TracedSimulatorFrontend.hh"
#include "ExecutableInstruction.hh"
#include "Program.hh"
#include "NullInstruction.hh"
#include "Instruction.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyWindow.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "ProximConstants.hh"
#include "ProximMainFrame.hh"

BEGIN_EVENT_TABLE(DisasmExecCountFrame, wxMiniFrame)
    EVT_LIST_ITEM_SELECTED(-1, DisasmExecCountFrame::onSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the miniframe.
 * @param id Window identifier for the frame.
 * @param topCountTable Table of top execution counts to display.
 */
DisasmExecCountFrame::DisasmExecCountFrame(
    wxWindow* parent, wxWindowID id,
    const DisasmTopCountAttrProvider::ExecutionCountTable& topCountTable) :
    wxMiniFrame(parent, id, _T("Top Execution Counts")),
    topCountTable_(topCountTable) {

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    list_ = new wxListCtrl(
        this, -1, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_SINGLE_SEL);

    list_->InsertColumn(0, _T("Exec Count"), wxLIST_FORMAT_LEFT, 100);
    list_->InsertColumn(1, _T("Address range"), wxLIST_FORMAT_LEFT, 200);
    sizer->Add(list_, 1, wxEXPAND |wxALL, 5);
    SetSizer(sizer);
}

/**
 * The Destructor.
 */
DisasmExecCountFrame::~DisasmExecCountFrame() {
}

/**
 * Updates the execution count list.
 */
void
DisasmExecCountFrame::update() {

    list_->DeleteAllItems();

    DisasmTopCountAttrProvider::ExecutionCountTable::const_iterator iter =
        topCountTable_.begin();

    int pos = topCountTable_.size();
    // Iterate through all entries in the topcount table.
    for (; iter != topCountTable_.end(); iter++) {

        pos--;
        unsigned count = (*iter).first;
        const DisasmTopCountAttrProvider::AddressSet addressSet =
            (*iter).second;

        DisasmTopCountAttrProvider::AddressSet::const_iterator citer =
            addressSet.begin();

        // Iterate through all addresses with the same execution count and
        // combine consecutive addresses to address ranges.
        while (citer != addressSet.end()) {
            unsigned rangeFirst = *citer;
            unsigned rangeLast = *citer;
            while (citer != addressSet.end() && (*citer == rangeLast)) {
                citer++;
                rangeLast++;
            }
            rangeLast--;

            // Add list entry for the address range.
            wxString range = WxConversion::toWxString(rangeFirst);
            if (rangeFirst != rangeLast) {
                range.Append(_T(" - "));
                range.Append(WxConversion::toWxString(rangeLast));
            }

            list_->InsertItem(0, WxConversion::toWxString(count));
            list_->SetItem(0, 1, range);
            list_->SetItemData(0, rangeFirst);

            // Set background color for the list item.
            // TODO: more elegant solution :)
            if (pos < 5) {
                list_->SetItemBackgroundColour(
                    0,  wxColour(255 , pos * 50, 0));
            } else {
                list_->SetItemBackgroundColour(
                    0, wxColour(255 - (2 * (pos * 25 - 128)), 255, 0));
            }
            

            if (citer != addressSet.end()) {
                citer++;
            }
        }
    }
}


/**
 * Event handler for the execution count list selection events.
 *
 * Selects and shows the start address of the selected range in the disassembly
 * winow.
 *
 * @param event Selection event to handle.
 */
void
DisasmExecCountFrame::onSelection(wxListEvent& event) {
    unsigned addr = list_->GetItemData(event.GetIndex());
    ProximDisassemblyWindow& disasmwin = *ProximToolbox::disassemblyWindow();
    disasmwin.showAddress(addr);
}


/**
 * The Constructor.
 *
 * @param simulator Simulator frontend for accessing instructions and execution
 *                  counts.
 * @param topCounts Number of top execution counts to list and highlight.
 */
DisasmTopCountAttrProvider::DisasmTopCountAttrProvider(
    TracedSimulatorFrontend& simulator, size_t topCounts):
    ProximDisasmAttrProvider(),
    simulator_(simulator), topCounts_(topCounts) {

    listWin_ = new DisasmExecCountFrame(
        ProximToolbox::disassemblyWindow(), -1, topCountTable_);

    listWin_->Show();
}


/**
 * The Destructor.
 */
DisasmTopCountAttrProvider::~DisasmTopCountAttrProvider() {
    if (listWin_ != NULL) {
        listWin_->Destroy();
    }
}

/**
 * Updates the list of top execution counts when simulation stops.
 */
void
DisasmTopCountAttrProvider::update() {
    updateTopCountTable();
    listWin_->update();
}


/**
 * Returns grid cell attributes for cell with given move.
 *
 * @param address Address of the cell's instruction.
 */
wxGridCellAttr*
DisasmTopCountAttrProvider::moveCellAttr(
    InstructionAddress address, int /* move */) {

    wxGridCellAttr* attr = new wxGridCellAttr();

    ClockCycleCount execCount =
        simulator_.executableInstructionAt(address).executionCount();

    if (execCount == 0) {
        attr->SetBackgroundColour(wxColour(220, 220, 220));
    } else {
        attr->SetBackgroundColour(bgColour(execCount));
    }

    return attr;
}

/**
 * Updates the list of top execution counts.
 */
void
DisasmTopCountAttrProvider::updateTopCountTable() {

    topCountTable_.clear();
    const TTAProgram::Program& program = simulator_.program();
    const TTAProgram::Instruction* instruction = &program.firstInstruction();

    while (instruction != &TTAProgram::NullInstruction::instance()) {
        InstructionAddress address = instruction->address().location();
        ClockCycleCount execCount =
            simulator_.executableInstructionAt(address).executionCount();

        if (execCount > 0) {
            addToTopCountTable(address, execCount);
        }

        instruction = &program.nextInstruction(*instruction);
    } 

}

/**
 * Adds an instruction to the execution top count list if the count is high
 * enough for the list.
 *
 * @param address Address of the instruction.
 * @param execCount Execution count of the instruction.
 */
void
DisasmTopCountAttrProvider::addToTopCountTable(
    InstructionAddress address, ClockCycleCount execCount) {

    ExecutionCountTable::iterator iter = topCountTable_.find(execCount);
    if (iter != topCountTable_.end()) {
        (*iter).second.insert(address);
    } else if (topCountTable_.empty() ||
               execCount > ((*topCountTable_.begin())).first) {

        AddressSet addressSet;
        addressSet.insert(address);
        topCountTable_.insert(
            std::pair<ClockCycleCount, AddressSet>(execCount, addressSet));
    }

    if (topCountTable_.size() > topCounts_) {
        topCountTable_.erase(topCountTable_.begin());
    }

}

/**
 * Returns background highlight colour for an instruction with the given
 * exec count.
 *
 * @param execCount Execution count of the instruciton.
 * @return Background highlight colour for the instruction.
 */
wxColour
DisasmTopCountAttrProvider::bgColour(ClockCycleCount execCount) {

    ExecutionCountTable::reverse_iterator iter = topCountTable_.rbegin();
    size_t pos = 0;
    while (iter != topCountTable_.rend()) {
        if ((*iter).first == execCount) {
            break;
        }
        pos++;
        iter++;
    }

    if (pos < topCounts_) { 
        int value = pos * (255 / topCounts_);
        if (pos < (topCounts_ / 2)) {
            return wxColour(255 , 2 * value, 0);
        } else {
            return wxColour(255 - (2 * (value-128)), 255, 0);
        }
    } else {
        return wxColour(255, 255, 255);
    }

}

