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
 * the old simulation engine for verification. The size of a single access
 * is limited to 64 MAUs.
 */
#define MAX_REQUEST_SIZE 64

class DirectAccessMemory : public Memory {
public:
    DirectAccessMemory(
        Word start,
        Word end,
        Word MAUSize,
        Word wordSize,
        int align);

    virtual ~DirectAccessMemory();

    virtual void initiateRead(Word address, int size, Memory::URC id)
        throw (OutOfRange);

    virtual void loadData(Memory::MAUVector& data, URC id);
    virtual std::pair<Memory::MAUTable, std::size_t> loadData(URC id);

    virtual bool resultReady(Memory::URC id);

    virtual void initiateWrite(
        Word address,
        Memory::MAUTable value,
        std::size_t size,
        Memory::URC)
        throw (OutOfRange);
    
    void inline fastWriteMAU(
        Word address,
        UIntWord data);
    
    void inline fastWrite2MAUs(
        Word address,
        UIntWord data);
    
    void inline fastWrite4MAUs(
        Word address,
        UIntWord data);
    
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
    virtual bool isAvailable() { return true; }
    virtual void readBlock(Word address, Memory::MAUVector& data);
    virtual void writeBlock(Word address, Memory::MAUVector data);
    virtual void reset() {}
    virtual void fillWithZeros();

private:
    /// Copying not allowed.
    DirectAccessMemory(const DirectAccessMemory&);
    /// Assignment not allowed.
    DirectAccessMemory& operator=(const DirectAccessMemory&);

    /**
     * Models an access request.
     *
     * There can be only one read request pending in DirectAccessMemory.
     * Writes are updated immediately.
     */
    struct Request {
        Request() :
            data_(NULL), dataSize_(0), address_(0), size_(0) {}
        /// Data to be written.
        Memory::MAUTable data_;
        /// Data table size.
        std::size_t dataSize_;
        /// Address to be written to.
        Word address_;
        /// Size of the data to be read.
        int size_;
    };
    /// The write request
    Request lastReadRequest_;
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
    /// The supported maximum access size (with initiate{Read,Write}) in MAUs.
    static const std::size_t MAX_ACCESS_SIZE = 16;
};

#include "DirectAccessMemory.icc"

#endif
