/**
 * @file DisasmTopCountAttrProvider.hh
 *
 * Declaration DisasmTopCountAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASM_TOP_COUNT_ATTR_PROVIDER_HH
#define TTA_DISASM_TOP_COUNT_ATTR_PROVIDER_HH

#include <set>
#include <map>
#include <wx/listctrl.h>
#include <wx/minifram.h>
#include "ProximDisasmAttrProvider.hh"
#include "Listener.hh"
#include "SimulatorConstants.hh"

class wxGridCellAttr;
class TracedSimulatorFrontend;
class DisasmExecCountFrame;

/**
 * Attribute provider for disassembly window grid.
 *
 * Highlights instructions with top execution counts with background colour.
 */
class DisasmTopCountAttrProvider : public ProximDisasmAttrProvider {
public:
    DisasmTopCountAttrProvider(
        TracedSimulatorFrontend& simulator, size_t topCounts);
    virtual ~DisasmTopCountAttrProvider();
    virtual void update();
    virtual wxGridCellAttr* moveCellAttr(
        InstructionAddress address, int move);

    typedef std::set<InstructionAddress> AddressSet;
    typedef std::map<ClockCycleCount, AddressSet> ExecutionCountTable;
private:
    void updateTopCountTable();
    void addToTopCountTable(
        InstructionAddress address, ClockCycleCount execCount);

    void createExecCountWindow();
    wxColour bgColour(ClockCycleCount execCount);

    /// Top execution counts and set of instruction addresses with each count.
    ExecutionCountTable topCountTable_;
    /// Simulator frontend used for accessing the instructions and exec counts.
    TracedSimulatorFrontend& simulator_;
    /// Number of top execution counts in the top execution count list.
    size_t topCounts_;
    DisasmExecCountFrame* listWin_;
};


/**
 * Simple list window for displaying list of top execution counts.
 */
class DisasmExecCountFrame : public wxMiniFrame {
public:
    DisasmExecCountFrame(
        wxWindow* parent, wxWindowID id,
        const DisasmTopCountAttrProvider::ExecutionCountTable& table);

    virtual ~DisasmExecCountFrame();
    void update();
    void addExecCount(
        unsigned execCount, unsigned start,
        unsigned end, const wxColour& colour);

private:
    void onClose(wxCloseEvent& event);
    void onSelection(wxListEvent& event);
    wxListCtrl* list_;
    const DisasmTopCountAttrProvider::ExecutionCountTable& topCountTable_;
 
    DECLARE_EVENT_TABLE()
};

#endif
