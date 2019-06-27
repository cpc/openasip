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
 * @file ProximDisassemblyGridTable.cc
 *
 * Implementation of ProximDisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <math.h>
#include "ProximDisassemblyGridTable.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"
#include "WxConversion.hh"
#include "ProximToolbox.hh"
#include "TracedSimulatorFrontend.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "ProximDisasmAttrProvider.hh"

/**
 * The Constructor.
 */
ProximDisassemblyGridTable::ProximDisassemblyGridTable():
    DisassemblyGridTable(), bpManager_(NULL),
    attrProvider_(NULL) {
        
}


/**
 * The Destructor.
 */
ProximDisassemblyGridTable::~ProximDisassemblyGridTable() {
    if (attrProvider_ != NULL) {
        delete attrProvider_;
    }
}

/**
 * Sets the breakpoint manager containing program breakpoints to display.
 *
 * @param bpManager Breakpoint manager of the program.
 */
void
ProximDisassemblyGridTable::setStopPointManager(
    StopPointManager& bpManager) {

    bpManager_ = &bpManager;
}


/**
 * Returns text value of a cell.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return Value of the cell as wxString.
 */
wxString
ProximDisassemblyGridTable::GetValue(int row, int col) {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return _T("");
    }

    wxString value;

    if (col == 0 && bpManager_ != NULL) {
        for (unsigned i = 0; i < bpManager_->stopPointCount(); i++) {
            unsigned handle = bpManager_->stopPointHandle(i);

            const Breakpoint* bp =
                dynamic_cast<const Breakpoint*>(
                      &bpManager_->stopPointWithHandleConst(handle));

            if (bp != NULL) {
                Word address = bp->address();
                if (address == (Word)row) {
                    value.Append(WxConversion::toWxString(handle));
                    value.Append(_T(" "));
                }
            }
        }
    }
    
    if (col == 0 && showPCArrow_ && (Word)row == currentInstruction_) {
        value.Append(_T("next>"));
    }

    if (col > 0) {
        value = DisassemblyGridTable::GetValue(row, col - 1);
    }
    return value;
}

/**
 * Returns number of columns in the grid.
 *
 * @return Number of columns.
 */
int
ProximDisassemblyGridTable::GetNumberCols() {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return 0;
    }

    return DisassemblyGridTable::GetNumberCols() + 1;
}


/**
 * Returns number of rows in the grid.
 *
 * @return Number of rows.
 */
int
ProximDisassemblyGridTable::GetNumberRows() {

    if (!ProximToolbox::frontend()->isProgramLoaded()) {
        return 0;
    }

    return DisassemblyGridTable::GetNumberRows();
}

/**
 * Returns label for a column.
 *
 * @param col Column index.
 * @return Column label.
 */
wxString
ProximDisassemblyGridTable::GetColLabelValue(int col) {
    if (col > 1) {
        return DisassemblyGridTable::GetColLabelValue(col - 1);
    } else {
        return _T("");
    }
}

/**
 * Turns on showing of the arrow displaying the current instruction.
 */
void
ProximDisassemblyGridTable::showPCArrow() {
    showPCArrow_ = true;
}

/**
 * Turns off showing of the arrow displaying the current instruction.
 */
void
ProximDisassemblyGridTable::hidePCArrow() {
    showPCArrow_ = false;
}


/**
 * Sets address of the current instruction.
 *
 * @param address Address of the current instruction.
 */
void
ProximDisassemblyGridTable::setCurrentInstruction(Word address) {
    currentInstruction_ = address;
}

/**
 * Sets the cell attribute provider for cell's containing move disassembly.
 *
 * ProximDisassemblyGridTable take ownership of the pointer and deletes the
 * object when it's no longer needed.
 *
 * @param attrProvider New move cell attribute provider.
 */
void
ProximDisassemblyGridTable::setMoveCellAttrProvider(
    ProximDisasmAttrProvider* attrProvider) {

    if (attrProvider_ != NULL) {
        delete attrProvider_;
    }
    attrProvider_ = attrProvider;
}

/**
 * Returns the move cell attribute provider.
 *
 * @return Move cell attribute provider, or NULL is it's not set.
 */
ProximDisasmAttrProvider*
ProximDisassemblyGridTable::moveCellAttrProvider() const {
    return attrProvider_;
}


/**
 * Returns cell style attributes for a cell with given row and column.
 *
 * @paran row Row of the grid cell.
 * @param col Column of the grid cell.
 * @return Cell's style attribute.
 */
wxGridCellAttr*
ProximDisassemblyGridTable::GetAttr(
    int row, int col, wxGridCellAttr::wxAttrKind  /* kind */) {

    wxGridCellAttr* attr = new wxGridCellAttr();

    if (ProximToolbox::frontend()->isProgramLoaded() &&  row >= 0 && col > 1) {
        if (attrProvider_ != NULL) {
            return attrProvider_->moveCellAttr(addressOfRow(row), col - 2);
        }
    } else {
        attr->SetBackgroundColour(wxColour(180, 180, 180));
    }

    return attr;
}
