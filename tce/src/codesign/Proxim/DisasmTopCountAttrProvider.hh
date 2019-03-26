/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file DisasmTopCountAttrProvider.hh
 *
 * Declaration DisasmTopCountAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
