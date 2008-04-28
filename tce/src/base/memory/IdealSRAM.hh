/**
 * @file IdealSRAM.hh
 *
 * Declaration of IdealSRAM class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IDEAL_SRAM_HH
#define TTA_IDEAL_SRAM_HH

#include <vector>
#include <map>

#include "Memory.hh"

class MemoryContents;

/**
 * Class that models an "ideal" memory.
 *
 * An ideal memory is defined as a memory with read latency zero. 
 * The data is available at the same cycle in which the load is initiated. 
 * Also, after a store is initiated, data is written into memory as soon as
 * the clock advances.
 *
 * This implementation uses a "paged array" as the storage structure which
 * avoids unnecessary allocation while providing O(1) access time. See
 * PagedArray for more details.
 */
class IdealSRAM : public Memory {
public:
    IdealSRAM(Word start, Word end, Word MAUSize);
    virtual ~IdealSRAM();

    virtual void write(Word address, MAU data);
    virtual Memory::MAU read(Word address);

    using Memory::write;
    using Memory::read;

    virtual void fillWithZeros();

private:
    /// Copying not allowed.
    IdealSRAM(const IdealSRAM&);
    /// Assignment not allowed.
    IdealSRAM& operator=(const IdealSRAM&);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;
    /// Container for holding read/write requests.
    MemoryContents* data_;
};

#endif
