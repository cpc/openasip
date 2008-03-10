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
#include "MemoryContents.hh"

/**
 * Class that models an "ideal" memory.
 *
 * An ideal memory is defined as a memory (at least) with read latency zero. 
 * The data is available in the same cycle in which the load is initiated. 
 * Also, after a store is initiated, data is written into memory as soon as
 * the clock advances.
 */
class IdealSRAM : public Memory {
public:
    IdealSRAM(
        Word start,
        Word end,
        Word MAUSize,
        Word wordSize,
        int align);

    virtual ~IdealSRAM();

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
    IdealSRAM(const IdealSRAM&);
    /// Assignment not allowed.
    IdealSRAM& operator=(const IdealSRAM&);

    /**
     * Models write request.
     */
    struct Request {
        Request() :
            data_(NULL), dataSize_(0), address_(0), size_(0), id_(0) {}
        /// Data to be written.
        Memory::MAUTable data_;
        /// Data table size.
        std::size_t dataSize_;
        /// Address to be written to.
        Word address_;
        /// Size of the data to be read/written.
        int size_;
        /// Id of the request.
        Memory::URC id_;
    };
    /// Container for holding read/write requests.
    typedef std::vector<Request*> RequestQueue;
    /// Holds all write requests.
    RequestQueue writeRequests_;
    /// Holds all read requests.
    RequestQueue readRequests_;
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
};

#endif
