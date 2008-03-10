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
    Word start,
    Word end,
    Word MAUSize,
    Word wordSize,
    int align) : Memory(), start_(start), end_(end), MAUSize_(MAUSize),
                 MAUSize3_(MAUSize_ * 3), MAUSize2_(MAUSize_ * 2),
                 wordSize_(wordSize), mask_(~(~0 << MAUSize_)), 
                 alignment_(align) {

    data_ = new MemoryContents(end_ - start_);
    lastReadRequest_.data_ = new MAU[MAX_REQUEST_SIZE];
}


/**
 * Destructor.
 */
DirectAccessMemory::~DirectAccessMemory() {
    delete data_;
    data_ = NULL;
    delete lastReadRequest_.data_;
    lastReadRequest_.data_ = NULL;
}


/**
 * Initiates a load request.
 *
 * If address is out of range, OutOfRange exception is thrown.
 *
 * @param address Address where data is loaded.
 * @param size Size of the data area to be loaded.
 * @param id Id of the request.
 * @exception OutOfRange if the requested address is out of range.
 */
void
DirectAccessMemory::initiateRead(Word address, int size, Memory::URC)
    throw (OutOfRange) {
    lastReadRequest_.address_ = address;
    lastReadRequest_.size_ = size;
    data_->read(
        address - start_, lastReadRequest_.data_, lastReadRequest_.size_);
}

/**
 * Loads data for the request identified by given id.
 *
 * If id is not found, returns [NULL, 0]. Returned MAUTable is not property
 * of the caller, and should NOT be deleted after use.
 *
 * @param id Id of the request.
 * @return The table of MAUs.
 */
std::pair<Memory::MAUTable, std::size_t>
DirectAccessMemory::loadData(URC) {
    return std::make_pair(lastReadRequest_.data_, lastReadRequest_.size_);
}

/**
 * Loads data for the request identified by given id.
 *
 * @param data The storage for the read data.
 * @param id Id of the request.
 */
void
DirectAccessMemory::loadData(Memory::MAUVector& data, Memory::URC id) {
    std::pair<Memory::MAUTable, std::size_t> readData = loadData(id);
    if (data.size() != readData.second) {
        data.resize(readData.second);
    }
    for (size_t i = 0; i < data.size(); ++i) {
        data.at(i) = readData.first[i];
    }
}

/**
 * Returns true if result is ready.
 *
 * For direct access memory, results are immediately readable.
 *
 * @param id The id of the request.
 * @return True if result is ready.
 */
bool
DirectAccessMemory::resultReady(Memory::URC) {
    return true;
}

/**
 * Initiates a write request.
 *
 * Writes are updated directly to the memory.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param size Size of data to be written.
 * @param id Id of the request.
 * @exception OutOfRange if the requested address is out of range.
 */
void
DirectAccessMemory::initiateWrite(
    Word address,
    Memory::MAUTable data,
    std::size_t size,
    Memory::URC)
    throw (OutOfRange) {
    data_->write(address - start_, data, size);
}

/**
 * Reads a block of data from the memory.
 *
 * If address is out of range, it is recalculated.
 *
 * @param address The address which data is read from.
 * @param data Container in which data is read.
 */
void
DirectAccessMemory::readBlock(Word address, Memory::MAUVector& data) {
    int size = data.size();
    data_->read(address - start_, data, size);
}

/**
 * Writes a block of data in memory.
 *
 * If address is out of range, it is recalculated.
 *
 * @param address Address to write to.
 * @param data Data to be written.
 * @exception OutOfRange If memory boundaries are exceeded.
 */
void
DirectAccessMemory::writeBlock(Word address, Memory::MAUVector data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        Memory::MAU temp = data[i];
        data_->write(address - start_ + i, &temp, 1);
    }
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
