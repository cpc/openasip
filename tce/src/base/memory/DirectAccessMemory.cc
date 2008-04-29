/**
 * @file DirectAccessMemory.cc
 *
 * Definition of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <string>
#include <utility>

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
    MAUSize3_(MAUSize_ * 3), MAUSize2_(MAUSize_ * 2),
    mask_(~(~0 << MAUSize_)) {

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
