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
 * @file MemoryGridTable.cc
 *
 * Implementation of MemoryGridTable class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "MemoryGridTable.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "Memory.hh"

using std::string;

const int MemoryGridTable::MAX_ROWS = 4000000;
const string MemoryGridTable::NOT_AVAILABLE = "N/A";

/**
 * The Constructor.
 *
 * @param memory Memory to display in the grid.
 */
MemoryGridTable::MemoryGridTable(Memory& memory):
    wxGridTableBase(),
    memory_(memory),
    dataMode_(DATA_HEX),
    sizeMode_(SIZE_MAU),
    numberOfColumns_(8) {

    start_ = memory.start();
    end_ = memory.end();
    mauSize_ = memory.MAUSize();

    // Kludge to avoid overflow with some calculations when the AS size = 2^32.
    // The grid-widget is not capable of displaying enough rows,
    // so the last address wouldn't be displayed anyway.
    if (start_ == 0 && end_ == 0xffffffff) end_ = 0xfffffffe;
}


/**
 * The Destructor.
 */
MemoryGridTable::~MemoryGridTable() {
}


/**
 * Returns row count of the grid.
 *
 * The row count is limited to MAX_ROWS due to limtiations of wxGrid.
 *
 * @return Number of rows in the table.
 */
int
MemoryGridTable::GetNumberRows() {

    Word size = end_ - start_ + 1;
    int cells = size / sizeOfCell();
    int rows = cells / numberOfColumns_;
    if ((cells % numberOfColumns_) != 0) {
        rows++;
    }
    if (rows > MAX_ROWS) {
	rows = MAX_ROWS;
    }
    return rows;
}

/**
 * Returns column count of the grid.
 *
 * @return Number of coulmns in the table.
 */
int
MemoryGridTable::GetNumberCols() {
    return numberOfColumns_;
}


/**
 * Returns true, if the given cell is empty, false otherwise.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return True, if the column is empty.
 */
bool
MemoryGridTable::IsEmptyCell(int /* row UNUSED */, int /* col UNUSED */) {
    return false;
}


/**
 * Returns cell value as a wxString.
 *
 * Returns memory contents corresponding to the cell coordinates. The
 * memory value is formatted to the string depending on the size and
 * type modes set.
 *
 * @param row Row of the cell.
 * @param col Column of the cell.
 * @return Cell contents as a wxString.
 */
wxString
MemoryGridTable::GetValue(int row, int col) {

    unsigned addr =
	start_ + (row * numberOfColumns_ * sizeOfCell()) +
	(col * sizeOfCell());

    if ((addr + sizeOfCell()) > (end_ + 1)) {
        return WxConversion::toWxString(NOT_AVAILABLE);
    }

    wxString value = memoryContents(addr);
    return value;
}


/**
 * Returns row label of a grid row.
 *
 * The label is the memory address of the first cell in the row.
 *
 * @param row Row number.
 * @return Label for the grid row.
 */
wxString
MemoryGridTable::GetRowLabelValue(int row) {

    string address = Conversion::toHexString(
        start_ + row * numberOfColumns_ * sizeOfCell());

    return WxConversion::toWxString(address);
}


/**
 * Returns column label of a grid column.
 *
 * The label is the offset of the column compared to the first cell in the
 * row.
 *
 * @param col Column number.
 * @return Label for the grid column.
 */
wxString
MemoryGridTable::GetColLabelValue(int col) {
    string offset = Conversion::toHexString(col * sizeOfCell());
    return WxConversion::toWxString(offset);
}


/**
 * Not implemented, use setCellValue() instead.
 */
void
MemoryGridTable::SetValue(int, int, const wxString&) {
    // Do nothing.
}


/**
 * Returns contents of the given memory contents as a wxString.
 *
 * The string formatting depends on the current sizeMode_ and dataMode_ set.
 *
 * @param addr Memory address to return.
 * @return Memory contents as a wxString.
 */
wxString
MemoryGridTable::memoryContents(Word addr) {

    unsigned size = sizeOfCell();
    string dataString = NOT_AVAILABLE;
    unsigned int cellSize = sizeOfCell() * mauSize_;

    if ((size * mauSize_) <= sizeof(SIntWord) * BYTE_BITWIDTH) {

        // read one word
        UIntWord data = 0;

        if (addr < start_ || addr > end_) {
            // memory not available
            return WxConversion::toWxString(NOT_AVAILABLE);
        } else {
            memory_.read(addr, size, data);
        }


        if (dataMode_ == DATA_BIN) {
            dataString =
                Conversion::toBinary(static_cast<int>(data), cellSize);
        } else if (dataMode_ == DATA_HEX) {
            unsigned digits = cellSize / 4;
            if ((cellSize % 4) != 0) {
                cellSize++;
            }
            dataString = Conversion::toHexString(data, digits);
        } else if (dataMode_ == DATA_SIGNED_INT) {
            int extendedValue = data;
            extendedValue =
                extendedValue <<
                ((sizeof(extendedValue)*BYTE_BITWIDTH) - cellSize);
            extendedValue =
                extendedValue >>
                ((sizeof(extendedValue)*BYTE_BITWIDTH) - cellSize);
            dataString = Conversion::toString(extendedValue);
        } else if (dataMode_ == DATA_UNSIGNED_INT) {
            dataString = Conversion::toString(data);
        } else if (dataMode_ == DATA_FLOAT &&
                   cellSize == sizeof(FloatWord) * BYTE_BITWIDTH) {

            FloatWord flt;
            memory_.read(addr, flt);
            dataString = Conversion::toString(flt);
        }
    } else if ((size * mauSize_) <= sizeof(DoubleWord) * BYTE_BITWIDTH) {

        // Only double display is available due to the limitations of
        // stringstream hex/bin conversion.
        if (dataMode_ == DATA_DOUBLE
            && cellSize == sizeof(DoubleWord) * BYTE_BITWIDTH) {

            // read one double word
            DoubleWord data = 0;
            if (addr < start_ || addr > end_) {
                // memory not available
                return WxConversion::toWxString(NOT_AVAILABLE);
            } else {
                memory_.read(addr, data);
            }

            dataString = Conversion::toString(data);
        }
    }
    return WxConversion::toWxString(dataString);
}



/**
 * Sets the number of columns to display.
 *
 * @param columns New table width.
 */
void
MemoryGridTable::setNumberOfColumns(unsigned columns) {
    numberOfColumns_ = columns;
}


/**
 * Sets the data display mode of the table.
 *
 * @param mode Data display mode to set.
 */
void
MemoryGridTable::setDataMode(DataMode mode) {
    dataMode_ = mode;
}


/**
 * Sets the memory size per cell.
 *
 * @param mode Size mode to set.
 */
void
MemoryGridTable::setSizeMode(SizeMode mode) {
    sizeMode_ = mode;
}


/**
 * Sets the memory value correspoding to a cell.
 *
 * @param row Row of the cell.
 * @param column Column of the cell.
 * @param memoryValue Value to set.
 */
void
MemoryGridTable::writeValue(int row, int column, UIntWord memoryValue) {
    Word address = start_;
    address += cellAddress(row, column);
    int size = sizeOfCell();
    if (address < end_) {
        memory_.write(address, size, memoryValue);
        memory_.advanceClock();
    }
}


/**
 * Sets the memory value correspoding to a cell.
 *
 * @param row Row of the cell.
 * @param column Column of the cell.
 * @param memoryValue Value to set.
 */
void
MemoryGridTable::writeValue(int row, int column, DoubleWord memoryValue) {
    Word address = start_;
    address += cellAddress(row, column);
    if (address < end_) {
        memory_.write(address, memoryValue);
        memory_.advanceClock();
    }
}


/**
 * Calculates the address of given cell.
 *
 * @param row The selected row.
 * @param colummn The selected column.
 * @return Address of the cell.
 */
Word
MemoryGridTable::cellAddress(int row, int column) const {
    Word address = 0;
    unsigned size = sizeOfCell();
    address += column * size;
    address += row * numberOfColumns_ * size;
    return address;
}


/**
 * Returns row and column nubmer of the address in the table.
 *
 * @param address Memory address to find.
 * @param row Variable to set the row to.
 * @param col Variable to set the column to.
 */
void
MemoryGridTable::findAddress(Word addr, int& row, int& col) {
    unsigned cellSize = sizeOfCell();
    row = addr / ((unsigned)numberOfColumns_ * cellSize);
    col = (addr % ((unsigned)numberOfColumns_ * cellSize)) / cellSize;
}


/**
 * Returns size of memory displayed in a single cell.
 *
 * @return Memory size of a cell.
 */
unsigned
MemoryGridTable::sizeOfCell() const {
    if (sizeMode_ == SIZE_MAU) {
	return 1;
    } else if (sizeMode_ == SIZE_TWO_MAUS) {
	return 2;
    } else if (sizeMode_ == SIZE_FOUR_MAUS) {
	return 4;
    } else if (sizeMode_ == SIZE_EIGHT_MAUS) {
	return 8;
    }
    assert(false);
    return 0;
}
