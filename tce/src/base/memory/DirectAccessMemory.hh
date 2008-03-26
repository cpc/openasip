/**
 * @file DirectAccessMemory.hh
 *
 * Declaration of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DIRECT_ACCESS_MEMORY_HH
#define TTA_DIRECT_ACCESS_MEMORY_HH

#include <vector>
#include <map>

#include "Memory.hh"
#include "MemoryContents.hh"
#include "BaseType.hh"

/**
 * Class that models an "ideal" memory to which updates are visible
 * immediately.
 *
 * This model is used in compiled simulation. It does not require an
 * advance clock call: all writes to it are visible immediately. Thus,
 * one has to make sure that all reads in the same cycle are executed
 * before writes in order for the reads to read the old values.
 *
 * Note that all range checking is disabled for fastest possible simulation
 * model. In case you are unsure of your simulated input correctness, use
 * the old simulation engine for verification.
 */
class DirectAccessMemory : public Memory {
public:
    DirectAccessMemory(
        Word start,
        Word end,
        Word MAUSize);

    virtual ~DirectAccessMemory();

    void write(Word address, Memory::MAU data);
    
    void inline fastWriteMAU(
        Word address,
        UIntWord data);
    
    void inline fastWrite2MAUs(
        Word address,
        UIntWord data);
    
    void inline fastWrite4MAUs(
        Word address,
        UIntWord data);

    Memory::MAU read(Word address);
    
    void inline fastReadMAU(
        Word address,
        UIntWord& data);
    
    void inline fastRead2MAUs(
        Word address,
        UIntWord& data);
    
    void inline fastRead4MAUs(
        Word address,
        UIntWord& data);

    virtual void advanceClock() {}
    virtual void reset() {}
    virtual void fillWithZeros();

    using Memory::write;
    using Memory::read;

private:
    /// Copying not allowed.
    DirectAccessMemory(const DirectAccessMemory&);
    /// Assignment not allowed.
    DirectAccessMemory& operator=(const DirectAccessMemory&);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;
    /// precalculated MAUSize_ * 3
    Word MAUSize3_;
    /// precalculated MAUSize_ * 2
    Word MAUSize2_;    
    /// Size of the natural word as MAUs.
    Word wordSize_;
    /// Mask bit pattern for unpacking IntWord to MAUs.
    int mask_;
    /// Alignment contstraint of the natural word.
    int alignment_;
    /// Contains MAUs of the memory model, that is, the actual data of the
    /// memory.
    MemoryContents* data_;
};

#include "DirectAccessMemory.icc"

#endif
