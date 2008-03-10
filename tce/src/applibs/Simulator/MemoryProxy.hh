/**
 * @file MemoryProxy.hh
 *
 * Declaration of MemoryProxy class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_MEMORY_PROXY_HH
#define TTA_MEMORY_PROXY_HH

#include "Memory.hh"

class SimulatorFrontend;

/**
 * MemoryProxy is a memory access tracker for simulator.
 *
 * MemoryProxy tracks calls to initiate reads and writes.
 * Memory accesses on the last clock cycle can be listed
 * with readAccessCount(), readAccess(idx), writeAccessCount()
 * and writeAccess(idx) functions. Memory access information is
 * stored for only one cycle.
 */
class MemoryProxy : public Memory {
public:

    typedef std::pair<Word, int> MemoryAccess;

    MemoryProxy(SimulatorFrontend & frontend, Memory*
memory);
    virtual ~MemoryProxy();

    virtual void initiateRead(Word address, int size, URC id)
        throw (OutOfRange);
    
    virtual void initiateWrite(
        Word address,
        Memory::MAUTable data,
        std::size_t size,
        URC id)
        throw (OutOfRange);

    virtual void advanceClock();
    virtual void reset();

    virtual void loadData(MAUVector& data, URC id);
    virtual std::pair<MAUTable, std::size_t> loadData(URC id);
    virtual bool resultReady(URC id);
    virtual bool isAvailable();
    virtual void readBlock(Word address, MAUVector& data);
    virtual void writeBlock(Word address, MAUVector data);
    virtual void fillWithZeros();

    unsigned int readAccessCount() const;
    unsigned int writeAccessCount() const;

    MemoryAccess readAccess(unsigned int idx) const
        throw (OutOfRange);

    MemoryAccess writeAccess(unsigned int idx) const
        throw (OutOfRange);

private:
    SimulatorFrontend & frontend_;

    /// Wrapped memory.
    Memory* memory_;
    /// Copying not allowed.
    MemoryProxy(const MemoryProxy&);
    /// Assignment not allowed.
    MemoryProxy& operator=(const MemoryProxy&);

    /// List of initiated reads on the last cycle.
    std::vector<MemoryAccess> reads_;
    /// List of initiated writes on the last cycle.
    std::vector<MemoryAccess> writes_;
    /// List of initiated reads.
    std::vector<MemoryAccess> newReads_;
    /// List of initiated writes.
    std::vector<MemoryAccess> newWrites_;

};

#endif
