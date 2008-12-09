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
 * @file MemoryGridTable.hh
 *
 * Declaration of MemoryGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_GRID_TABLE_HH
#define TTA_MEMORY_GRID_TABLE_HH

#include <string>
#include <wx/wx.h>
#include <wx/grid.h>
#include "BaseType.hh"

class Memory;

/**
 * Grid table class which allows wxGrid to access TargetMemory contents
 * without copying the memory contents to separate table.
 */
class MemoryGridTable : public wxGridTableBase {
public:
    MemoryGridTable(Memory& memory);
    virtual ~MemoryGridTable();
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual wxString GetRowLabelValue(int row);
    virtual wxString GetColLabelValue(int col);
    virtual void SetValue(int row, int col, const wxString& value);

    void writeValue(int row, int column, UIntWord memoryValue);
    void writeValue(int row, int column, DoubleWord memoryValue);
    void findAddress(Word addr, int& row, int& col);
    void setNumberOfColumns(unsigned columns);

    enum SizeMode {
	SIZE_MAU,
	SIZE_TWO_MAUS,
	SIZE_FOUR_MAUS,
	SIZE_EIGHT_MAUS
    };

    enum DataMode {
	DATA_BIN,
	DATA_HEX,
	DATA_SIGNED_INT,
	DATA_UNSIGNED_INT,
	DATA_DOUBLE,
	DATA_FLOAT
    };

    void setSizeMode(SizeMode mode);
    void setDataMode(DataMode mode);
    unsigned sizeOfCell() const;


private:
    Word cellAddress(int row, int column) const;
    wxString memoryContents(Word addr);

    /// Memory to access.
    Memory& memory_;
    /// Start address of the memory range to display.
    Word start_;
    /// End address of the memory range to display.
    Word end_;

    /// String that is displayed in a cell that is not in the current AS.
    static const std::string NOT_AVAILABLE;
    static const int COLUMNS;
    /// Maximum number of rows to display in the window.
    static const Word MAX_ROWS;
    /// Current data mode of the window (hex/binary/int...).
    DataMode dataMode_;
    /// Current size mode of the window (1/2/4... MAUs per cell).
    SizeMode sizeMode_;
    /// Size of MAU in bits.
    int mauSize_;
    /// Current number of columns in the grid.
    unsigned numberOfColumns_;
};

#endif
