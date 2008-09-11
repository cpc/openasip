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
 * @file ProximDisassemblyGridTable.hh
 *
 * Declaration of ProximDisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
