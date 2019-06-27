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
 * @file DisassemblyGridTable.cc
 *
 * Implementation of DisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "DisassemblyGridTable.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "Program.hh"
#include "POMDisassembler.hh"
#include "AssocTools.hh"
#include "DisassemblyInstruction.hh"
#include "NullInstruction.hh"
#include "Machine.hh"
#include "GlobalScope.hh"
#include "CodeLabel.hh"
#include "Procedure.hh"

using std::string;

using namespace TTAProgram;


/**
 * The Constructor.
 */
DisassemblyGridTable::DisassemblyGridTable():
    wxGridTableBase(), program_(NULL), disassembler_(NULL) {
	    
}


/**
 * The Destructor.
 */
DisassemblyGridTable::~DisassemblyGridTable() {
    if (disassembler_ != NULL) {
        delete disassembler_;
    }
}


/**
 * Returns row count of the grid.
 *
 * The row count is limited to MAX_ROWS due to limtiations of wxGrid.
 *
 * @return Number of rows in the table.
 */
int
DisassemblyGridTable::GetNumberRows() {
    Word firstAddress = program_->firstInstruction().address().location();
    Word lastAddress = program_->lastInstruction().address().location();
    return lastAddress - firstAddress + 1;
}


/**
 * Returns column count of the grid.
 *
 * @return Number of coulmns in the table.
 */
int
DisassemblyGridTable::GetNumberCols() { 
    int busCount = program_->targetProcessor().busNavigator().count();
    int immSlotCount =
        program_->targetProcessor().immediateSlotNavigator().count();
    return busCount + immSlotCount + 1;
}


/**
 * Returns true, if the given cell is empty, false otherwise.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return True, if the column is empty.
 */
bool
DisassemblyGridTable::IsEmptyCell(int /* row UNUSED */, int /* col UNUSED */) {
    return false;
}


/**
 * Returns cell value as a wxString.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return Cell contents as a wxString.
 */
wxString
DisassemblyGridTable::GetValue(int row, int col) {

    int busCount = program_->targetProcessor().busNavigator().count();

    if (col == 0) {
        wxString labels = WxConversion::toWxString("");
        std::pair<LabelMap::iterator, LabelMap::iterator> range = 
            labels_.equal_range(row);
        LabelMap::iterator firstEqual = range.first;
        LabelMap::iterator lastEqual = range.second;
        while (firstEqual != lastEqual) {
                labels += WxConversion::toWxString(firstEqual->second)
                       + WxConversion::toWxString(" ");
                firstEqual++;
            }
            return labels;
    } else if (col > 0) {
        Word address = (Word) row;
        DisassemblyInstruction* disassembly =
            disassembler_->createInstruction(address);
        
        wxString disasm;
        if (col < (busCount + 1)) {
            disasm = WxConversion::toWxString(
                disassembly->move(col - 1).toString());
        } else {
            int immCount = static_cast<int>(disassembly->longImmediateCount());
            if (col < (busCount + 1 + immCount)) {
                disasm = WxConversion::toWxString(
                    disassembly->longImmediate(col - 1 - busCount).toString());
            }
        }
        delete disassembly;
        return disasm;
    }

    return _T("");
}


/**
 * Returns row label of a grid row.
 *
 * @param row Row number.
 * @return Label for the grid row.
 */
wxString
DisassemblyGridTable::GetRowLabelValue(int row) {
    return WxConversion::toWxString(row);
}


/**
 * Returns column label of a grid column.
 *
 * @param col Column number.
 * @return Label for the grid column.
 */ 
wxString
DisassemblyGridTable::GetColLabelValue(int col) {

    int busCount = program_->targetProcessor().busNavigator().count();

    if (col > 0 && col < (busCount + 1)) {
        const TTAMachine::Machine::BusNavigator& navigator =
            program_->targetProcessor().busNavigator();
    
        string label = Conversion::toString(col - 1);
        label += ": ";
        label += navigator.item(col - 1)->name();
        return WxConversion::toWxString(label);
    } else {
        return _T("");
    }
}


/**
 * Returns true if the grid cells can have attributes.
 *
 * @return Always true.
 */
bool
DisassemblyGridTable::CanHaveAttributes() {
    return true;
}


/**
 * Not implemented, use setCellValue() instead.
 */
void
DisassemblyGridTable::SetValue(int, int, const wxString&) {
    // Do nothing.
}


/**
 * Loads a new program in the grid table.
 *
 * @param program Program to load.
 */
void
DisassemblyGridTable::loadProgram(const Program& program) {

    program_ = &program;

    if (disassembler_ != NULL) {
        delete disassembler_;
    }

    disassembler_ = new POMDisassembler(program);

    labels_.clear();

    const GlobalScope& gScope = program_->globalScopeConst();
    int labelCount = gScope.globalCodeLabelCount();

    for (int i = 0; i < labelCount; i++) {
        const CodeLabel& codeLabel = gScope.globalCodeLabel(i);
        unsigned address = codeLabel.address().location();
        std::string label = codeLabel.name();
        labels_.insert(std::pair<unsigned, string>(address, label));
    }
}

/**
 * Returns address of the instruction on a row.
 *
 * @param row Row of the instruction.
 * @return Address of the instruction.
 */
Word
DisassemblyGridTable::addressOfRow(int row) {
    return (Word)row;
}


/**
 * Returns row of the instruction with given address.
 *
 * @param address Address of the instruction.
 * @return Row of the instruction.
 */
int
DisassemblyGridTable::rowOfAddress(Word address) {
    return (int)address;
}
