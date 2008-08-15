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
 * @file DisassemblyGridTable.hh
 *
 * Declaration of DisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_GRID_TABLE_HH
#define TTA_DISASSEMBLY_GRID_TABLE_HH

#include <map>
#include <string>
#include <wx/wx.h>
#include <wx/grid.h>
#include "BaseType.hh"

namespace TTAProgram {
    class Program;
}

class POMDisassembler;

/**
 * Grid table class which allows wxGrid to display disassembly of a tta program
 * by disassembling only the currently visible instructions. Purpose of the
 * 'lazy' disasembling is to conserve memory and processing time when the
 * program is very large.
 */
class DisassemblyGridTable : public wxGridTableBase {
public:
    DisassemblyGridTable();
    virtual ~DisassemblyGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual wxString GetRowLabelValue(int row);
    virtual wxString GetColLabelValue(int col);
    virtual void SetValue(int row, int col, const wxString& value);
    virtual bool CanHaveAttributes();
    int rowOfAddress(Word address);
    Word addressOfRow(int row);
    void loadProgram(const TTAProgram::Program& program);
protected:
    /// Program loaded in the table.
    const TTAProgram::Program* program_;
private:
    /// Disassembler for generating instruction disassemblies.
    POMDisassembler*  disassembler_;

    /// Type for the labels multimap
    typedef std::multimap<int, std::string> LabelMap;
    
    /// Program labels.
    LabelMap labels_;
};

#endif
