/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file Memory.cc
 *
 * Non-inline definitions of Memory class.
 *
 * @author Pekka J��skel�inen 2004,2014 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstddef>
#include <ios>

#include <boost/format.hpp>
#include "Memory.hh"
#include "MemoryContents.hh"
#include "Application.hh"
#include "Conversion.hh"
#include "WriteRequest.hh"

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
Memory::Memory(Word start, Word end, Word MAUSize, bool littleEndian) : 
    littleEndian_(littleEndian), 
    start_(start), end_(end), MAUSize_(MAUSize),
    writeRequests_(new RequestQueue()) {

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
        mask_ = ~0u << MAUSize_;
        mask_ = ~mask_;
    }
}

/**
 * Destructor.
 */
Memory::~Memory() {
    delete writeRequests_;
    writeRequests_ = NULL;
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
 * Writes a single memory location directly without waiting to
 * the end of the clock.
 *
 * This is used in case it's important for potential other
 * memory operations in the same cycle to see the write.
 * This is used (at the moment solely) to simulate Compare-and-swap (CAS).
 *
 * @param address The target address.
 * @param count Number of MAUs to write.
 * @param data The data to write.
 */
void
Memory::writeDirectlyBE(Word address, int count, UIntWord data) {

    checkRange(address, count);

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpackBE(data, count, MAUData);

    for (int i = 0; i < count; ++i) {
        write(address + i, MAUData[i]);
    }
}

/**
 * Writes a single memory location directly without waiting to
 * the end of the clock.
 *
 * This is used in case it's important for potential other
 * memory operations in the same cycle to see the write.
 * This is used (at the moment solely) to simulate Compare-and-swap (CAS).
 *
 * @param address The target address.
 * @param count Number of MAUs to write.
 * @param data The data to write.
 */
void
Memory::writeDirectlyLE(Word address, int count, UIntWord data) {

    checkRange(address, count);

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpackLE(data, count, MAUData);

    for (int i = 0; i < count; ++i) {
        write(address + i, MAUData[i]);
    }
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
Memory::read(Word /*address*/) {
    return 0;
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
Memory::write(Word address, int count, UIntWord data) {
    if (littleEndian_) {
        writeLE(address, count, data);
    } else {
        writeBE(address, count, data);
    }
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
Memory::writeBE(Word address, int count, UIntWord data) {

    checkRange(address, count);

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpackBE(data, count, MAUData);

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[count];
    std::memcpy(request->data_, MAUData, count*sizeof(MAU));
    request->size_ = count;
    request->address_ = address;
    writeRequests_->push_back(request);
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
Memory::writeLE(Word address, int count, UIntWord data) {

    checkRange(address, count);

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpackLE(data, count, MAUData);

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[count];
    std::memcpy(request->data_, MAUData, count*sizeof(MAU));
    request->size_ = count;
    request->address_ = address;
    writeRequests_->push_back(request);
}

/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a FloatWord in Big Endian Format
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readBE(Word address, FloatWord& data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "Loading FloatWords works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 4;

    checkRange(address, MAUS);

    union castUnion {
        FloatWord d;
        Byte maus[MAUS];
    };

    castUnion cast;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // one byte so endian does not matter
        readBE(address + i, 1, data);
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        cast.maus[i] = data;
        #else
        cast.maus[MAUS - 1 - i] = data;
        #endif        
    }
    data = cast.d;
}

/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a FloatWord in Little Endian Format
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readLE(Word address, FloatWord& data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "Loading FloatWords works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 4;

    checkRange(address, MAUS);

    union castUnion {
        FloatWord d;
        Byte maus[MAUS];
    };

    castUnion cast;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // one bytes so endian does not matter
        readLE(address + i, 1, data);
        // Byte order must be reversed if host is not little endian.
        #if WORDS_BIGENDIAN == 0
        cast.maus[i] = data;
        #else
        cast.maus[MAUS - 1 - i] = data;
        #endif        
    }
    data = cast.d;
}

/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a FloatWord in order set by the memory.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::read(Word address, FloatWord& data) {
    if (littleEndian_) {
        readLE(address, data);
    } else {
        readBE(address, data);
    }
}

/**
 * A convenience method for writing a FloatWord to the memory in Big Endian.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::writeBE(Word address, FloatWord data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "Writing FloatWords works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 4;

    checkRange(address, MAUS);

    union castUnion {
        FloatWord d;
        Byte maus[MAUS];
    };

    castUnion cast;
    cast.d = data;

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[MAUS];
    request->size_ = MAUS;
    request->address_ = address;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        data = cast.maus[i];
        #else
        data = cast.maus[MAUS - 1 - i];
        #endif
        request->data_[i] = data;
    }
    writeRequests_->push_back(request);
}

/**
 * A convenience method for writing a FloatWord to the memory in Little Endian.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::writeLE(Word address, FloatWord data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "Writing FloatWords works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 4;

    checkRange(address, MAUS);

    union castUnion {
        FloatWord d;
        Byte maus[MAUS];
    };

    castUnion cast;
    cast.d = data;

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[MAUS];
    request->size_ = MAUS;
    request->address_ = address;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 0
        data = cast.maus[i];
        #else
        data = cast.maus[MAUS - 1 - i];
        #endif
        request->data_[i] = data;
    }
    writeRequests_->push_back(request);
}


/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a DoubleWord in Big Endian.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readBE(Word address, DoubleWord& data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "LDD works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 8;
    union castUnion {
        DoubleWord d;
        Byte maus[MAUS];
    };

    castUnion cast;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        readBE(address + i, 1, data);
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        cast.maus[i] = data;
        #else
        cast.maus[MAUS - 1 - i] = data;
        #endif        
    }
    data = cast.d;
}

/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a DoubleWord in Little Endian.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readLE(Word address, DoubleWord& data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "LDD works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 8;
    union castUnion {
        DoubleWord d;
        Byte maus[MAUS];
    };

    castUnion cast;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        readLE(address + i, 1, data);
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 0
        cast.maus[i] = data;
        #else
        cast.maus[MAUS - 1 - i] = data;
        #endif        
    }
    data = cast.d;
}

/**
 * A convenience method for reading data from the memory and 
 * interpreting it as a DoubleWord in order set by endian bit of memory.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::read(Word address, DoubleWord& data) {
    if (littleEndian_) {
        readLE(address, data);
    } else {
        readBE(address, data);
    }
}

/**
 * A convenience method for writing a DoubleWord to the memory.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::write(Word address, DoubleWord data) {
    if (littleEndian_) {
        writeLE(address, data);
    } else {
        writeBE(address, data);
    }
}

/**
 * A convenience method for writing a DoubleWord to the memory in Big Endian.
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::writeBE(Word address, DoubleWord data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "LDD works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 8;

    checkRange(address, MAUS);

    union castUnion {
        DoubleWord d;
        Byte maus[MAUS];
    };

    castUnion cast;
    cast.d = data;

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[MAUS];
    request->size_ = MAUS;
    request->address_ = address;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        data = cast.maus[i];
        #else
        data = cast.maus[MAUS - 1 - i];
        #endif
        request->data_[i] = data;
    }
    writeRequests_->push_back(request);
}

/**
 * A convenience method for writing a DoubleWord to the memory in Litle Endian
 *
 * @note Currently works only if MAU == 8 bits. asserts otherwise.
 *
 * @param address The address to write.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::writeLE(Word address, DoubleWord data) {

    assert(MAUSize() == sizeof(Byte)*8 && 
           "LDD works only with byte sized MAU at the moment.");

    const std::size_t MAUS = 8;

    checkRange(address, MAUS);

    union castUnion {
        DoubleWord d;
        Byte maus[MAUS];
    };

    castUnion cast;
    cast.d = data;

    WriteRequest* request = new WriteRequest();
    request->data_ = new MAU[MAUS];
    request->size_ = MAUS;
    request->address_ = address;

    for (std::size_t i = 0; i < MAUS; ++i) {
        UIntWord data;
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 0
        data = cast.maus[i];
        #else
        data = cast.maus[MAUS - 1 - i];
        #endif
        request->data_[i] = data;
    }
    writeRequests_->push_back(request);
}

/**
 * A convenience method for reading units of data from the memory in Big Endian
 *
 * The data is written to an UIntWord. 
 *
 * @param address The address to read.
 * @param count Number of MAUs to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readBE(Word address, int size, UIntWord& data) {

    checkRange(address, size);

    data = 0;
    int shiftCount = MAUSize_ * (size - 1);
    for (int i = 0; i < size; i++) {
        data = data | (read(address + i) << shiftCount);
        shiftCount -= MAUSize_;
    }
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
Memory::read(Word address, int size, UIntWord& data) {
    if (littleEndian_) {
        readLE(address, size, data);
    } else {
        readBE(address, size, data);
    }
}

/**
 * A convenience method for reading units of data from the memory in Little Endian
 *
 * The data is written to an UIntWord. 
 *
 * @param address The address to read.
 * @param count Number of MAUs to read.
 * @param data The data to write.
 * @exception OutOfRange in case the address is out of range of the memory.
 */
void
Memory::readLE(Word address, int size, UIntWord& data) {

    checkRange(address, size);

    data = 0;
    int shiftCount = 0;
    for (int i = 0; i < size; i++) {
        data = data | (read(address + i) << shiftCount);
        shiftCount += MAUSize_;
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
    RequestQueue::iterator iter = writeRequests_->begin();
    while (iter != writeRequests_->end()) {
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }
    writeRequests_->clear();
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
void
Memory::packBE(const Memory::MAUTable data, int size, UIntWord& value) {

    value = 0;
    int shiftCount = MAUSize_ * (size - 1);
    for (int i = 0; i < size; i++) {
        value = value | (data[i] << shiftCount);
        shiftCount -= MAUSize_;
    }
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
void
Memory::packLE(const Memory::MAUTable data, int size, UIntWord& value) {

    value = 0;
    int shiftCount = 0;
    for (int i = 0; i < size; i++) {
        value = value | (data[i] << shiftCount);
        shiftCount += MAUSize_;
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
void
Memory::unpackBE(
    const UIntWord& value,
    int size,
    Memory::MAUTable data) {
    int shiftCount = MAUSize_ * (size - 1);
    for(int i = 0; i < size; ++i) {
        data[i] = ((value >> shiftCount) & mask_);
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
void
Memory::unpackLE(
    const UIntWord& value,
    int size,
    Memory::MAUTable data) {
    int shiftCount = 0;
    for(int i = 0; i < size; ++i) {
        data[i] = ((value >> shiftCount) & mask_);
        shiftCount += MAUSize_;
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

    RequestQueue::iterator iter = writeRequests_->begin();
    while (iter != writeRequests_->end()) {
        WriteRequest* req = (*iter);
        for (int i = 0; i < req->size_; ++i) {
            write(req->address_ + i, req->data_[i]);
        }
        delete[] (*iter)->data_;
        (*iter)->data_ = NULL;
        delete (*iter);
        ++iter;
    }

    writeRequests_->clear();
}

/**
 * Helper for checking the legality of the memory access address range.
 *
 * Does nothing in case the address range is legal, throws otherwise.
 *
 * @exception OutOfRange in case the range is illegal.
 */
void
Memory::checkRange(Word startAddress, int numberOfMAUs) {

    unsigned int low = start(); 
    unsigned int high = end(); 

    if ((startAddress < low) || (startAddress > high - numberOfMAUs + 1)) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            (boost::format(
                "Memory access at %d of size %d is out of the address space.")
             % startAddress % numberOfMAUs).str());
    }
}
