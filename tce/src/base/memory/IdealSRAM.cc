/**
 * @file IdealSRAM.cc
 *
 * Definition of IdealSRAM class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <utility>

#include "IdealSRAM.hh"
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
IdealSRAM::IdealSRAM(Word start, Word end, Word MAUSize) : 
    Memory(start, end, MAUSize), start_(start), end_(end), MAUSize_(MAUSize) {
    data_ = new MemoryContents(end_ - start_);
}


/**
 * Destructor.
 *
 * The storage reserved for the memory contents is deallocated. Any data
 * about pending access requests is freed, too.
 */
IdealSRAM::~IdealSRAM() {
    delete data_;
    data_ = NULL;
}

/**
 * Writes a single memory location.
 *
 * The fastest way to write to the memory.
 *
 * @param address The target address.
 * @param data The data to write.
 */
void
IdealSRAM::write(Word address, MAU data) {
    data_->writeData(address - start_, data);
}

/**
 * Reads a single memory location.
 *
 * The fastest way to read the memory.
 *
 * @param address The address to read.
 * @return The data read.
 */
Memory::MAU
IdealSRAM::read(Word address) {
    return data_->readData(address - start_);
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
IdealSRAM::fillWithZeros() {
    data_->clear();
}


