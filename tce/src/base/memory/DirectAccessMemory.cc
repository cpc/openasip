/**
 * @file DirectAccessMemory.cc
 *
 * Definition of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
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
 * @param wordSize Number of MAUs that make up a natural word.
 * @param align Alignment of natural words, expressed in number of MAUs.
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
