/**
 * @file Memory.cc
 *
 * Non-inline definitions of Memory class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <cstddef>

#include "Memory.hh"
#include "MemoryContents.hh"
#include "Application.hh"
#include "Conversion.hh"

//////////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the memory model.
 *
 * The created memory model is empty. No data is allocated for its contents.
 *
 * @param start First address of the memory.
 * @param end Last address of the memory.
 * @param MAUSize Bit width of the minimum addressable unit of the memory.
 */
Memory::Memory(Word start, Word end, Word MAUSize) : 
    start_(start), end_(end), MAUSize_(MAUSize) {

    const std::size_t maxMAUSize =
        static_cast<int>(sizeof(MinimumAddressableUnit) * BYTE_BITWIDTH);

    if (MAUSize_ == maxMAUSize) {
        mask_ = ~0;
    } else if (MAUSize_ > maxMAUSize) {
        std::string msg = "Maximum supported MAU width is ";
        msg += Conversion::toString(maxMAUSize);
        msg += " bits.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    } else {
        mask_ = ~0 << MAUSize_;
        mask_ = ~mask_;
    }

}

/**
 * Destructor.
 */
Memory::~Memory() {
}

/**
 * Writes a single memory location.
 *
 * Must be implemented in the derived class. No range checking.
 * The fastest way to write to the memory.
 *
 * @param address The target address.
 * @param data The data to write.
 */
void
Memory::write(Word, MAU) {
    abortWithError("Must be implemented in the derived class.");
}

/**
 * Reads a single memory location.
 *
 * Must be implemented in the derived class. No range checking.
 * The fastest way to read the memory.
 *
 * @param address The address to read.
 * @return The data read.
 */
Memory::MAU
Memory::read(Word address) {
    address = address;
    return 0;
}

/**
 * A convenience method for writing units of data to
 * the memory.
 *
 * The data is stored in an UIntWord. 
 *
 * @param address The address to write.
 * @param count Number of MAUs to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::write(Word address, int count, UIntWord data)
    throw (OutOfRange) {

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpack(data, count, MAUData);

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[count];
    std::memcpy(request->data_, MAUData, count*sizeof(MAU));
    request->size_ = count;
    request->address_ = address;
    writeRequests_.push_back(request);
}

/**
 * A convenience method for reading units of data from the memory.
 *
 * The data is written to an UIntWord. 
 *
 * @param address The address to read.
 * @param count Number of MAUs to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::read(Word address, int size, UIntWord& data)
    throw (OutOfRange) {
    data = 0;
    int shiftCount = MAUSize_ * (size - 1);
    for (int i = 0; i < size; i++) {
        data = data | (read(address + i) << shiftCount);
        shiftCount -= MAUSize_;
    }
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero. The default implementation is a slow
 * for-loop which can be overridden with a faster one depending on the
 * storage used.
 */
void
Memory::fillWithZeros() {
    for (std::size_t addr = start_; addr <= end_; ++addr) {
        write(addr, MAU(0));
    }
}

/**
 * Resets the memory.
 *
 * Clears any pending write requests.
 */
void
Memory::reset() {
    RequestQueue::iterator iter = writeRequests_.begin();
    while (iter != writeRequests_.end()) {
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }
    writeRequests_.clear();
}

/**
 * Packs MAUs to UIntWord.
 *
 * Version for static table.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value The target of the packing.
 */
inline void
Memory::pack(const Memory::MAUTable data, int size, UIntWord& value) {

    value = 0;
    int shiftCount = MAUSize_ * (size - 1);
    for (int i = 0; i < size; i++) {
        value = value | (data[i] << shiftCount);
        shiftCount -= MAUSize_;
    }
}

/**
 * Unpack a given UIntWord to MAUs.
 *
 * Version for static table. Table is expected to be correct size, no checking
 * is done!
 *
 * @param value Value to be unpacked.
 * @param size The number of MAUs.
 * @param data The target of unpacking.
 */
inline void
Memory::unpack(
    const UIntWord& value,
    std::size_t size,
    Memory::MAUTable data) {
    int shiftCount = MAUSize_ * (size - 1);
    for(std::size_t i = 0; i < size; ++i) {
        data[i] = ((value >> shiftCount) & mask_);
        shiftCount -= MAUSize_;
    }
}

/**
 * Advances clock for one cycle.
 *
 * Commits all pending write requests to the memory.
 * Cannot throw an exception, request legality is checked when request
 * is initiated.
 */
void
Memory::advanceClock() {

    RequestQueue::iterator iter = writeRequests_.begin();
    while (iter != writeRequests_.end()) {
        WriteRequest* req = (*iter);
        for (int i = 0; i < req->size_; ++i) {
            write(req->address_ + i, req->data_[i]);
        }
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }

    writeRequests_.clear();
}

/**
 * Reads a block of data from the memory.
 *
 * @param address The address which data is read from.
 * @param data Container in which data is read.
 */
void
Memory::readBlock(Word address, Memory::MAUVector& data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = read(address - start_ + i);
    }
}

/**
 * Writes a block of data in memory.
 *
 * @param address Address to write to.
 * @param data Data to be written.
 */
void
Memory::writeBlock(Word address, Memory::MAUVector data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        Memory::MAU temp = data[i];
        write(address - start_ + i, temp);
    }
}

//////////////////////////////////////////////////////////////////////////////
// NullMemory
//////////////////////////////////////////////////////////////////////////////

NullMemory* NullMemory::instance_ = NULL;

/**
 * Constructor.
 */
NullMemory::NullMemory() : Memory(0, 0, 0) {
}

/**
 * Destructor.
 */
NullMemory::~NullMemory() {
}

void
NullMemory::write(Word, MAU) {
    abortWithError("Do not call me!");
}

Memory::MAU
NullMemory::read(Word) {
    abortWithError("Do not call me!");
    return 0;
}


/**
 * Returns an instance of NullMemory class (singleton).
 *
 * @return Singleton instance of NullMemory class.
 */
NullMemory&
NullMemory::instance() {
    if (instance_ == NULL) {
        instance_ = new NullMemory();
    }
    return *instance_;
}

