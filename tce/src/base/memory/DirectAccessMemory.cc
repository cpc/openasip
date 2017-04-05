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
    Word start, Word end, Word MAUSize, bool littleEndian) : 
    Memory(start, end, MAUSize, littleEndian), 
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
 * A convenience method for writing units of data to the memory.
 *
 * The data is stored in an UIntWord. 
 *
 * @param address The address to write.
 * @param count Number of MAUs to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
DirectAccessMemory::writeBE(Word address, int count, UIntWord data) {
    Memory::writeBE(address, count,  data);
    // compiled simulator does not call advance clock of
    // memories at every cycle for efficiency, so we have
    // to "flush" the writes right away
    Memory::advanceClock();
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
DirectAccessMemory::fastWrite2MAUsBE(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index + 1, (int)(data & mask_));
}

/**
 * Writes 2 MAUs to the memory as fast as possible in little Endian
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWrite2MAUsLE(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index + 1, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index, (int)(data & mask_));
}

/**
 * Writes 4 MAUs to the memory as fast as possible in BE.
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWrite4MAUsBE(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index, (int)((data >> MAUSize3_) & mask_));
    data_->writeData(index + 1, (int)((data >> MAUSize2_) & mask_));
    data_->writeData(index + 2, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index + 3, (int)(data & mask_));
}

/**
 * Writes 4 MAUs to the memory as fast as possible in LE
 * 
 * @param address address to write
 * @param data data to be written
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastWrite4MAUsLE(Word address, UIntWord data) {  
    const Word index = address - start_;
    data_->writeData(index + 3, (int)((data >> MAUSize3_) & mask_));
    data_->writeData(index + 2, (int)((data >> MAUSize2_) & mask_));
    data_->writeData(index + 1, (int)((data >> MAUSize_) & mask_));
    data_->writeData(index, (int)(data & mask_));
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
 * Reads 2 MAUs from the memory as fast as possible in BE
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead2MAUsBE(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index) << MAUSize_;
    data |= data_->readData(index + 1);
}

/**
 * Reads 2 MAUs from the memory as fast as possible in LE
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead2MAUsLE(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index +1) << MAUSize_;
    data |= data_->readData(index);
}

/**
 * Reads 4 MAUs from the memory as fast as possible in BE
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead4MAUsBE(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index) << MAUSize3_;
    data |= data_->readData(index + 1) << MAUSize2_;
    data |= data_->readData(index + 2) << MAUSize_;
    data |= data_->readData(index + 3);
}

/**
 * Reads 4 MAUs from the memory as fast as possible in LE
 * 
 * @param address address to read
 * @param data reference to the read data
 * @note No bounds checking is made so the address is assumed to be in range.
 * @note On a cycle with read and write, make sure the read is done *first* !
 */
void 
DirectAccessMemory::fastRead4MAUsLE(Word address, UIntWord& data) {
    const Word index = address - start_;
    data = data_->readData(index + 3) << MAUSize3_;
    data |= data_->readData(index + 2) << MAUSize2_;
    data |= data_->readData(index + 1) << MAUSize_;
    data |= data_->readData(index);
}
