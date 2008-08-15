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
