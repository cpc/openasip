/**
 * @file SequentialMemory.hh
 *
 * Declaration of SequentialMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_SRAM_HH
#define TTA_SEQUENTIAL_SRAM_HH

#include <vector>
#include <map>

#include "Memory.hh"
#include "MemoryContents.hh"

/**
 * Class that models a memory which is accessed with sequential code.
 *
 * Sequential access allows several optimizations for the implementation
 * of the Memory interface. For example, memory can be written directly
 * without waiting for the clock cycle change, because it's known that
 * there is not going to be a simultaneous read.
 */
class SequentialMemory : public Memory {
public:
    SequentialMemory(
        Word start,
        Word end,
        Word MAUSize,
        Word wordSize,
        int align);

    virtual ~SequentialMemory();

    virtual void initiateRead(Word address, int size, Memory::URC id)
        throw (OutOfRange);

    virtual void loadData(Memory::MAUVector& data, URC id);
    virtual std::pair<Memory::MAUTable, std::size_t> loadData(URC id);

    virtual bool resultReady(Memory::URC id);

    virtual void initiateWrite(
        Word address,
        Memory::MAUTable data,
        std::size_t size,
        Memory::URC id)
        throw (OutOfRange);

    virtual void advanceClock();
    virtual bool isAvailable();
    virtual void readBlock(Word address, Memory::MAUVector& data);
    virtual void writeBlock(Word address, Memory::MAUVector data);
    virtual void reset();
    virtual void fillWithZeros();

private:
    /// Copying not allowed.
    SequentialMemory(const SequentialMemory&);
    /// Assignment not allowed.
    SequentialMemory& operator=(const SequentialMemory&);
    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;
    /// Size of the natural word as MAUs.
    Word wordSize_;
    /// Alignment contstraint of the natural word.
    int alignment_;
    /// Contains MAUs of the memory model, that is, the actual data of the
    /// memory.
    MemoryContents* data_;
    /// The data of the latest read request.
    Memory::MAUTable readData_;
    /// The size of the latest read request.
    std::size_t lastReadSize_;
};

/// The maximum access size (with initiate{Read,Write}) in MAUs. It's known
/// that in sequential code the biggest accessed chunk is a double, which
/// is 8 bytes.
const std::size_t MAX_ACCESS_SIZE = 8;

#endif
