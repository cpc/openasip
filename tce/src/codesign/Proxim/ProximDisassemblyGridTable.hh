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
 * @file ProximDisassemblyGridTable.hh
 *
 * Declaration of ProximDisassemblyGridTable class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_DISASSEMBLY_GRID_TABLE_HH
#define TTA_PROXIM_DISASSEMBLY_GRID_TABLE_HH

#include "DisassemblyGridTable.hh"

class StopPointManager;
class InstructionMemory;
class ProximDisasmAttrProvider;

/**
 * A specialized DisassemblyGridTable class, which adds a column with
 * breakpoint labels and arrow displaying the  current instruction
 * to the disassembly grid.
 */
class ProximDisassemblyGridTable : public DisassemblyGridTable {
public:
    ProximDisassemblyGridTable();
    ~ProximDisassemblyGridTable();

    virtual wxString GetValue(int row, int col);
    virtual int GetNumberCols();
    virtual int GetNumberRows();
    virtual wxString GetColLabelValue(int col);
    virtual wxGridCellAttr* GetAttr(
        int row, int col, wxGridCellAttr::wxAttrKind  kind);

    void setStopPointManager(StopPointManager& manager);
    void setCurrentInstruction(Word address);
    void setMoveCellAttrProvider(ProximDisasmAttrProvider* attrProvider); 
    ProximDisasmAttrProvider* moveCellAttrProvider() const;
    void showPCArrow();
    void hidePCArrow();

private:
    /// Program counter value.
    Word currentInstruction_;
    /// Stoppoint manager managing program breakpoints.
    StopPointManager* bpManager_;
    /// True, if the current istrcution arrow should be displayed.
    bool showPCArrow_;
    ///
    ProximDisasmAttrProvider* attrProvider_;
};

#endif
