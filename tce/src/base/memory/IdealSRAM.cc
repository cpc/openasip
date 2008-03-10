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
IdealSRAM::IdealSRAM(
    Word start,
    Word end,
    Word MAUSize,
    Word wordSize,
    int align) : Memory(), start_(start), end_(end), MAUSize_(MAUSize),
                 wordSize_(wordSize), alignment_(align) {

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
    for (URC i = 0; i < readRequests_.size(); ++i) {
        Request* request = readRequests_[i];
        delete[] request->data_;
        request->data_ = NULL;
        delete request;
    }
    readRequests_.clear();
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
IdealSRAM::initiateRead(Word address, int size, Memory::URC id)
    throw (OutOfRange) {

    if (address < start_ || address > end_) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            "Tried to read address " + Conversion::toString(address) +
            " which is out of bounds of the address space.");
    }
    Request* request = NULL;

    // check if we have already a request slot for the id
    for (URC i = 0; i < readRequests_.size(); ++i) {
        if (readRequests_[i]->id_ == id) {
            request = readRequests_[i];
        }
    }
    if (request == NULL) {
        request = new Request();
        readRequests_.push_back(request);
        request->data_ = new MAU[size];
        request->dataSize_ = size;
    } else if (static_cast<int>(request->dataSize_) < size) {
        delete[] request->data_;
        request->data_ = new MAU[size];
        request->dataSize_ = size;
    } // otherwise we reuse the old data_ array

    request->id_ = id;
    request->size_ = size;

    data_->read(address - start_, request->data_, request->size_);
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
IdealSRAM::loadData(URC id) {
    for (std::size_t i = 0; i < readRequests_.size(); ++i) {
        if (readRequests_[i]->id_ == id) {
            return std::make_pair(
                readRequests_[i]->data_, readRequests_[i]->size_);
        }
    }
    debugLog("Did not find data for the URC.");
    return std::make_pair(static_cast<MAUTable>(NULL), 0);
}

/**
 * Loads data for the request identified by given id.
 *
 * @param data The storage for the read data.
 * @param id Id of the request.
 */
void
IdealSRAM::loadData(Memory::MAUVector& data, Memory::URC id) {
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
 * @param id The id of the request.
 * @return True if result is ready.
 */
bool
IdealSRAM::resultReady(Memory::URC id) {
    for (URC i = 0; i < readRequests_.size(); ++i) {
        if (readRequests_[i]->id_ == id) {
            return true;
        }
    }

    return false;
}

/**
 * Initiates a write request.
 *
 * Write request is put to request queue and is handled when clock
 * advances. If address is out of range, it is recalculated.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param size Size of data to be written.
 * @param id Id of the request.
 * @exception OutOfRange if the requested address is out of range.
 */
void
IdealSRAM::initiateWrite(
    Word address,
    Memory::MAUTable data,
    std::size_t size,
    Memory::URC id)
    throw (OutOfRange) {

    if (address < start_ || address > end_) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            "Tried to write address " + Conversion::toString(address) +
            " which is out of bounds of the address space.");
    }

    /// @todo Optimize: avoid unnecessary dynamic allocations!
    Request* request = new Request();
    request->data_ = new MAU[size];
    std::memcpy(request->data_, data, size*sizeof(MAU));
    request->size_ = size;
    request->address_ = address;
    request->id_ = id;
    writeRequests_.push_back(request);
    idle_ = false;
}

/**
 * Advances clock for one cycle.
 *
 * Cannot throw an exception, request legality is checked when request
 * is initiated.
 *
 * In case of ideal SRAM, all pending requests are fulfilled.
 */
void
IdealSRAM::advanceClock() {

    RequestQueue::iterator iter = writeRequests_.begin();
    while (iter != writeRequests_.end()) {
        data_->write(
            (*iter)->address_ - start_, (*iter)->data_, (*iter)->size_);
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }

    writeRequests_.clear();
    idle_ = true;
}

/**
 * Returns true if memory is available.
 *
 * Ideal SRAM is always available.
 *
 * @return Always true.
 */
bool
IdealSRAM::isAvailable() {
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
IdealSRAM::readBlock(Word address, Memory::MAUVector& data) {
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
IdealSRAM::writeBlock(Word address, Memory::MAUVector data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        Memory::MAU temp = data[i];
        data_->write(address - start_ + i, &temp, 1);
    }
}

/**
 * Resets the memory.
 *
 * Clears any request queues.
 */
void
IdealSRAM::reset() {
    RequestQueue::iterator iter = writeRequests_.begin();
    while (iter != writeRequests_.end()) {
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }
    writeRequests_.clear();
    idle_ = true;
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


