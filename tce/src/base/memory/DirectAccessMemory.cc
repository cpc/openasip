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
 * @file DirectAccessMemory.cc
 *
 * Definition of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <utility>
#include <limits>

#include "DirectAccessMemory.hh"
#include "MemoryContents.hh"
#include "Conversion.hh"
#include "Application.hh"

using std::string;


/**
 * Constructor. Create a model for a given memory.
 *
 * The created memory model is empty. No data is allocated for its contents.
 *
 * @param start First address of the memory.
 * @param end Last address of the memory.
 * @param MAUSize Bit width of the minimum addressable unit of the memory.
 */
DirectAccessMemory::DirectAccessMemory(
    Word start, Word end, Word MAUSize) : 
    Memory(start, end, MAUSize), 
    start_(start), end_(end), MAUSize_(MAUSize),
    MAUSize3_(MAUSize_ * 3), MAUSize2_(MAUSize_ * 2) {
        
    /// @note In C++, when shifting more bits than there are in integer, the
    /// result is undefined. Thus, we just set the mask to ~0 in this case.
    /// We should probably give user a warning if MAUSize is larger than 
    /// the integer size!
    if (MAUSize_ >= static_cast<Word>(std::numeric_limits<Word>::digits)) {
        mask_ = ~0;
    } else {
        mask_ = ~(~0 << MAUSize_);
    }

    data_ = new MemoryContents(end_ - start_);
}


/**
 * Destructor.
 */
DirectAccessMemory::~DirectAccessMemory() {
    delete data_;
    data_ = NULL;
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
DirectAccessMemory::fillWithZeros() {
    data_->clear();
}

/**
 * Writes a single MAU using the fastest possible method.
 *
 * @param address The address.
 * @param data The data.
 */
void
DirectAccessMemory::write(Word address, Memory::MAU data) {
    fastWriteMAU(address, data);
}

/**
 * Reads a single MAU using the fastest possible method.
 *
 * @param address The address.
 * @return Data.
 */
Memory::MAU 
DirectAccessMemory::read(Word address) {
    UIntWord data = 0;
    fastReadMAU(address, data);
    return data;
}

/**
 * Writes 1 MAU to the memory as fast as possible
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWriteMAU(Word address, UIntWord data) {
    data_->writeData(address - start_, (int)(data & mask_));
}

/**
 * Writes 2 MAUs to the memory as fast as possible
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWrite2MAUs(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index + 1, (int)(data & mask_));
}

/**
 * Writes 4 MAUs to the memory as fast as possible
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWrite4MAUs(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index, (int)((data >> MAUSize3_) & mask_));
    data_->writeData(index + 1, (int)((data >> MAUSize2_) & mask_));
    data_->writeData(index + 2, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index + 3, (int)(data & mask_));
}

/**
 * Reads 1 MAU from the memory as fast as possible
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastReadMAU(Word address, UIntWord& data) {
    data = data_->readData(address - start_);
}

/**
 * Reads 2 MAUs from the memory as fast as possible
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead2MAUs(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index) << MAUSize_;
    data |= data_->readData(index + 1);
}

/**
 * Reads 4 MAUs from the memory as fast as possible
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead4MAUs(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index) << MAUSize3_;
    data |= data_->readData(index + 1) << MAUSize2_;
    data |= data_->readData(index + 2) << MAUSize_;
    data |= data_->readData(index + 3);
}
