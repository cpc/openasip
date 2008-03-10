/**
 * @file TargetMemory.cc
 *
 * Definition of TargetMemory class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <algorithm>
#include <iomanip>

#include "TargetMemory.hh"
#include "Application.hh"
#include "config.h"
#include "Conversion.hh"

using std::cout;
using std::endl;

/**
 * Constructor.
 *
 * @param memory Memory instance.
 * @param bigEndian True if the endianess of the target machine is big endian.
 * @param MAUSize The size of the MAU in bits.
 * @exception OutOfRange if MAU size is too large.
 */
TargetMemory::TargetMemory(Memory& memory, bool bigEndian, int MAUSize) :
    memory_(memory), bigEndian_(bigEndian), sizeOfMAU_(MAUSize) {

    const int maxMAUSize =
        static_cast<int>(sizeof(MinimumAddressableUnit) * BYTE_BITWIDTH);

    if (sizeOfMAU_ == maxMAUSize) {
        mask_ = ~0;
    } else if (sizeOfMAU_ > maxMAUSize) {
        std::string msg = "Maximum supported MAU width is ";
        msg += Conversion::toString(maxMAUSize);
        msg += " bits.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    } else {
        mask_ = ~0 << sizeOfMAU_;
        mask_ = ~mask_;
    }
}

/**
 * Destructor.
 */
TargetMemory::~TargetMemory() {
}

/**
 * Returns the width of the minimum addressable unit of the memory.
 *
 * @return The width in bits.
 */
int
TargetMemory::mauWidth() const {
    return sizeOfMAU_;
}

/**
 * Initiates read request.
 *
 * @param address Address in which read request is done.
 * @param size The size of the read request.
 * @param id Id of the request.
 */
void
TargetMemory::initiateRead(Word address, int size, Memory::URC id)
    throw (OutOfRange) {
    memory_.initiateRead(address, size, id);
}

/**
 * Initiates write request.
 *
 * Swaps the word from host to big endian if necessary.
 *
 * @param address The address to which data is written.
 * @param size The size of MAUs to be written.
 * @param data Data to be written.
 * @param id Id of the request.
 */
void
TargetMemory::initiateWrite(
    Word address,
    int size,
    UIntWord data,
    Memory::URC id)
    throw (OutOfRange) {

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpack(data, size, MAUData);
    memory_.initiateWrite(address, MAUData, size, id);
}

/**
 * Initiates write request.
 *
 * @param address The address to which data is written.
 * @param size The size of MAUs to be written.
 * @param data Data to be written.
 * @param id Id of the request.
 */
void
TargetMemory::initiateWrite(
    Word address,
    int size,
    FloatWord data,
    Memory::URC id)
    throw (OutOfRange) {

    Memory::MAU MAUData[MAX_ACCESS_SIZE];
    unpack(*reinterpret_cast<UIntWord*>(&data), size, MAUData);
    memory_.initiateWrite(address, MAUData, size, id);
}

/**
 * Initiates write request.
 *
 * @param address The address to which data is written.
 * @param size The size of MAUs to be written.
 * @param data Data to be written.
 * @param id Id of the request.
 */
void
TargetMemory::initiateWrite(
    Word address,
    int size,
    DoubleWord data,
    Memory::URC id)
    throw (OutOfRange) {

    Memory::MAU MAUData[MAX_ACCESS_SIZE];

    union castUnion {
        double d;
        unsigned char bytes[sizeof(DoubleWord)];
    };

    castUnion cast;
    cast.d = data;

    for (std::size_t i = 0; i < sizeof(DoubleWord); ++i) {
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        MAUData[i] = cast.bytes[i];
        #else
        MAUData[sizeof(DoubleWord) - 1 - i] = cast.bytes[i];
        #endif
    }

    memory_.initiateWrite(address, MAUData, size, id);
}

/**
 * Reads data from the memory.
 *
 * @param data Data to be read.
 * @param id Request id.
 */
void
TargetMemory::readData(UIntWord& data, Memory::URC id) {
    std::pair<Memory::MAUTable, std::size_t> readData = memory_.loadData(id);
    pack(readData.first, readData.second, data);
}

/**
 * Reads data from the memory.
 *
 * @param data Data to be read.
 * @param id Request id.
 */
void
TargetMemory::readData(FloatWord& data, Memory::URC id) {
    std::pair<Memory::MAUTable, std::size_t> readData = memory_.loadData(id);
    pack(readData.first, readData.second, data);
}

/**
 * Reads data from the memory.
 *
 * @param data Data to be read.
 * @param id Request id.
 */
void
TargetMemory::readData(DoubleWord& data, Memory::URC id) {
    std::pair<Memory::MAUTable, std::size_t> readData = memory_.loadData(id);
    pack(readData.first, readData.second, data);
}

/**
 * Returns true if a new result is ready from the given request id.
 *
 * @param id Id of the request.
 * @return True if a new result is ready, false otherwise.
 */
bool
TargetMemory::isResultReady(Memory::URC id) {
    return memory_.resultReady(id);
}

/**
 * Advances clock cycle of memory by one.
 */
void
TargetMemory::advanceClock() {
    memory_.advanceClock();
}

/**
 * Reads a memory block from the memory.
 *
 * @param address Address of the memory block.
 * @param data Data to be read.
 * @param width The bit width of the word.
 */
void
TargetMemory::readBlock(Word address, UIntWordVector& data, int width) {

    Memory::MAUVector maus;
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;
    if (width % sizeOfMAU_ != 0) {
        numberOfMAUs++;
    }
    maus.resize(numberOfMAUs);
    for (size_t i = 0; i < data.size(); i++) {
        memory_.readBlock(currentAddress, maus);
        pack(maus, maus.size(), data[i]);
        currentAddress += numberOfMAUs;
    }
}

/**
 * Reads a memory block from the memory.
 *
 * @param address Address of the memory block.
 * @param data Data to be read.
 * @param width The bit width of the word.
 */
void
TargetMemory::readBlock(Word address, FloatWordVector& data, int width) {

    Memory::MAUVector maus;
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;
    if (width % sizeOfMAU_ != 0) {
        numberOfMAUs++;
    }
    maus.resize(numberOfMAUs);
    for (size_t i = 0; i < data.size(); i++) {
        memory_.readBlock(currentAddress, maus);
        UIntWord temp = 0;
        pack(maus, maus.size(), temp);
        data[i] = *reinterpret_cast<FloatWord*>(&temp);
        currentAddress += numberOfMAUs;
    }
}

/**
 * Reads a memory block from the memory.
 *
 * @param address Address of the memory block.
 * @param data Data to be read.
 * @param width The bit width of the word.
 */
void
TargetMemory::readBlock(Word address, DoubleWordVector& data, int width) {

    Memory::MAUVector maus;
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;
    if (width % sizeOfMAU_ != 0) {
        numberOfMAUs++;
    }
    maus.resize(numberOfMAUs);
    for (size_t i = 0; i < data.size(); i++) {

        DoubleWord dWord = 0;
        memory_.readBlock(currentAddress, maus);
        pack(maus, numberOfMAUs, dWord);
        currentAddress += numberOfMAUs;

        data[i] = dWord;
    }
}

/**
 * Writes a block of data in to memory.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param width Bit width of the word.
 */
void
TargetMemory::writeBlock(Word address, UIntWordVector data, int width) {

    Memory::MAUVector maus;
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;
    if (width % sizeOfMAU_ != 0) {
        numberOfMAUs++;
    }
    for (size_t i = 0; i < data.size(); i++) {
        unpack(data[i], numberOfMAUs, maus);
        memory_.writeBlock(currentAddress, maus);
        currentAddress += numberOfMAUs;
    }
}

/**
 * Writes a block of data in to memory.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param width Bit width of the word.
 */
void
TargetMemory::writeBlock(Word address, FloatWordVector data, int width) {

    Memory::MAUVector maus;
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;
    if (width % sizeOfMAU_ != 0) {
        numberOfMAUs++;
    }
    for (size_t i = 0; i < data.size(); i++) {
        unpack(*reinterpret_cast<UIntWord*>(&data[i]), numberOfMAUs, maus);
        memory_.writeBlock(currentAddress, maus);
        currentAddress += numberOfMAUs;
    }
}

/**
 * Writes a block of data in to memory.
 *
 * @param address Address to be written.
 * @param data Data to be written.
 * @param width Bit width of the word.
 */
void
TargetMemory::writeBlock(Word address, DoubleWordVector data, int width) {
    Memory::MAUVector MAUData(sizeof(DoubleWord));
    Word currentAddress = address;
    int numberOfMAUs = width / sizeOfMAU_;

    union castUnion {
        double d;
        unsigned char bytes[sizeof(DoubleWord)];
    };
    castUnion cast;

    for (size_t i = 0; i < data.size(); i++) {

        cast.d = data[i];

        for (std::size_t j = 0; j < sizeof(DoubleWord); ++j) { 
	    // Byte order must be reversed if host is not bigendian.
            #if WORDS_BIGENDIAN == 1
	    MAUData[j] = cast.bytes[j];
            #else
	    MAUData[sizeof(DoubleWord) - 1 - j] = cast.bytes[j];
            #endif
        }
        memory_.writeBlock(currentAddress, MAUData);
        currentAddress += numberOfMAUs;
    }
}

/**
 * Packs MAUs to UIntWord.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value The target of the packing.
 */
inline void
TargetMemory::pack(const Memory::MAUVector& data, int size, UIntWord& value) {
    if (data.size() > 0) {
        value = 0;
        int shiftCount = sizeOfMAU_ * (size - 1);
        for (int i = 0; i < size; i++) {
            value = value | (data[i] << shiftCount);
            shiftCount -= sizeOfMAU_;
        }
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
inline void
TargetMemory::pack(const Memory::MAUTable data, int size, UIntWord& value) {

    value = 0;
    int shiftCount = sizeOfMAU_ * (size - 1);
    for (int i = 0; i < size; i++) {
        value = value | (data[i] << shiftCount);
        shiftCount -= sizeOfMAU_;
    }
}

/**
 * Packs MAUs to a FloatWord.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value To target value.
 */
void
TargetMemory::pack(const Memory::MAUVector& data, int size, FloatWord& value) {
    UIntWord temp = 0;
    pack(data, size, temp);
    value = *(reinterpret_cast<FloatWord*>(&temp));
}

/**
 * Packs MAUs to a FloatWord.
 *
 * Version for static table.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value To target value.
 */
void
TargetMemory::pack(const Memory::MAUTable data, int size, FloatWord& value) {
    UIntWord temp = 0;
    pack(data, size, temp);
    value = *(reinterpret_cast<FloatWord*>(&temp));
}


/**
 * Packs MAUs to DoubleWord.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value The target of packing.
 */
void
TargetMemory::pack(
    const Memory::MAUVector& data, int size, DoubleWord& value) {

    assert(size == sizeof(DoubleWord));
    union castUnion {
        DoubleWord d;
        unsigned char bytes[sizeof(DoubleWord)];
    };

    castUnion cast;

    for (std::size_t i = 0; i < sizeof(DoubleWord); ++i) {
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        cast.bytes[i] = data[i];        
        #else
        cast.bytes[sizeof(DoubleWord) - 1 - i] = data[i];        
        #endif
    }
    value = cast.d;
}

/**
 * Packs MAUs to DoubleWord.
 *
 * Version for static table.
 *
 * @param data Data to be packed.
 * @param size The number of MAUs.
 * @param value The target of packing.
 */
void
TargetMemory::pack(const Memory::MAUTable data, int size, DoubleWord& value) {

    assert(size == sizeof(DoubleWord));
    union castUnion {
        double d;
        unsigned char bytes[sizeof(DoubleWord)];
    };

    castUnion cast;

    for (std::size_t i = 0; i < sizeof(DoubleWord); ++i) {
        // Byte order must be reversed if host is not bigendian.
        #if WORDS_BIGENDIAN == 1
        cast.bytes[i] = data[i];        
        #else
        cast.bytes[sizeof(DoubleWord) - 1 - i] = data[i];        
        #endif
    }
    value = cast.d;
}


/**
 * Unpack a given UIntWord to MAUs.
 *
 * Version for adding the MAUs to the end of an vector.
 *
 * @param value Value to be unpacked.
 * @param size The number of MAUs.
 * @param data The target of unpacking.
 */
inline void
TargetMemory::unpack(const UIntWord& value, int size, Memory::MAUVector& data) {
    int shiftCount = sizeOfMAU_ * (size - 1);
    for(int i = 0; i < size; i++) {
        data.push_back(((value >> shiftCount) & mask_));
        shiftCount -= sizeOfMAU_;
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
TargetMemory::unpack(
    const UIntWord& value,
    std::size_t size,
    Memory::MAUTable data) {
    int shiftCount = sizeOfMAU_ * (size - 1);
    for(std::size_t i = 0; i < size; ++i) {
        data[i] = ((value >> shiftCount) & mask_);
        shiftCount -= sizeOfMAU_;
    }
}
