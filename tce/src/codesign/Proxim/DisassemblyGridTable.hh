/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file DisassemblyGridTable.hh
 *
 * Declaration of DisassemblyGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
