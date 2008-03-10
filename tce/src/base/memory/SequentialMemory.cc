/**
 * @file SequentialMemory.cc
 *
 * Definition of SequentialMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <utility>

#include "SequentialMemory.hh"
#include "MemoryContents.hh"
#include "Conversion.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 */
SequentialMemory::SequentialMemory(
    Word start,
    Word end,
    Word MAUSize,
    Word wordSize,
    int align) : Memory(), start_(start), end_(end), MAUSize_(MAUSize),
                 wordSize_(wordSize), alignment_(align) {

    data_ = new MemoryContents(end_ - start_);
    readData_ = new MAU[MAX_ACCESS_SIZE];
    lastReadSize_ = 0;
}

/**
 * Destructor.
 */
SequentialMemory::~SequentialMemory() {
    delete[] readData_;
    readData_ = NULL;
    delete data_;
    data_ = NULL;
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
SequentialMemory::initiateRead(Word address, int size, Memory::URC)
    throw (OutOfRange) {

    if (address < start_ || address > end_) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            "Tried to read address " + Conversion::toString(address) +
            " which is out of bounds of the address space.");
    }
    assert(static_cast<std::size_t>(size) <= MAX_ACCESS_SIZE);
    lastReadSize_ = size;
    data_->read(address - start_, readData_, size);
}

/**
 * Loads data for the request identified by given id.
 *
 * Always returns the latest read data. URC is not used in the sequential
 * implementation.
 *
 * @param id Id of the request (unused).
 * @return The table of MAUs.
 */
std::pair<Memory::MAUTable, std::size_t>
SequentialMemory::loadData(URC) {
    return std::make_pair(readData_, lastReadSize_);
}

/**
 * Loads data for the request identified by given id.
 *
 * @param data The storage for the read data.
 * @param id Id of the request.
 */
void
SequentialMemory::loadData(Memory::MAUVector& data, Memory::URC id) {
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
 * Sequential version always returns true.
 *
 * @param id The id of the request (unused).
 * @return True if result is ready.
 */
bool
SequentialMemory::resultReady(Memory::URC) {
    return true;
}

/**
 * Initiates a write request.
 *
 * In the sequential version, write is commited immediately to the memory
 * array.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param size Size of data to be written.
 * @param id Id of the request (unused).
 * @exception OutOfRange if the requested address is out of range.
 */
void
SequentialMemory::initiateWrite(
    Word address,
    Memory::MAUTable data,
    std::size_t size,
    Memory::URC)
    throw (OutOfRange) {

    if (address < start_ || address > end_) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            "Tried to write address " + Conversion::toString(address) +
            " which is out of bounds of the address space.");
    }
    assert(size <= MAX_ACCESS_SIZE);
    data_->write(address, data, size);
}

/**
 * Advances clock for one cycle.
 *
 * Sequential implementation does nothing. This should not even be called
 * in the simulation loop because the memory appears to be always idle to
 * the client.
 */
void
SequentialMemory::advanceClock() {
    return;
}

/**
 * Returns true if memory is available.
 *
 * Sequential memory is always available.
 *
 * @return Always true.
 */
bool
SequentialMemory::isAvailable() {
    return true;
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
SequentialMemory::readBlock(Word address, Memory::MAUVector& data) {
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
SequentialMemory::writeBlock(Word address, Memory::MAUVector data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        Memory::MAU temp = data[i];
        data_->write(address - start_ + i, &temp, 1);
    }
}

/**
 * Resets the memory.
 *
 * Clears any request queues. Sequential implementation does nothing, because
 * there is no queue.
 */
void
SequentialMemory::reset() {
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
SequentialMemory::fillWithZeros() {
    data_->clear();
}
