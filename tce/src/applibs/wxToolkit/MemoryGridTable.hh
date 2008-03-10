/**
 * @file MemoryGridTable.hh
 *
 * Declaration of MemoryGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_GRID_TABLE_HH
#define TTA_MEMORY_GRID_TABLE_HH

#include <string>
#include <wx/wx.h>
#include <wx/grid.h>
#include "BaseType.hh"

class TargetMemory;

/**
 * Grid table class which allows wxGrid to access TargetMemory contents
 * without copying the memory contents to separate table.
 */
class MemoryGridTable : public wxGridTableBase {
public:
    MemoryGridTable(TargetMemory* memory, Word start, Word end, int mauSize);
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
    TargetMemory* memory_;
    /// Start address of the memory range to display.
    Word start_;
    /// End address of the memory range to display.
    Word end_;

    /// String that is displayed in a cell that is not in the current AS.
    static const std::string NOT_AVAILABLE;
    static const int COLUMNS;
    /// Maximum number of rows to display in the window.
    static const int MAX_ROWS;
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
