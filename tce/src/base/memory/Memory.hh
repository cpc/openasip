/**
 * @file Memory.hh
 *
 * Declaration of the base interface for the memory model (Memory class).
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */


#ifndef TTA_MEMORY_MODEL_HH
#define TTA_MEMORY_MODEL_HH

#include <vector>
#include <map>

#include "BaseType.hh"
#include "Exception.hh"

//////////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////////

/**
 * Memory Model interface provides methods for emulating memory access.
 *
 * Interface provides methods for emulating the cycle-accurate behaviour of
 * accesses to data memory due to operations of the target architecture such
 * as load and store.
 */
class Memory {
public:

    /// Unique request code for identifying requests.
    typedef unsigned int URC;

    /// Minimum addressable unit of the memory.
    typedef int MAU;
    /// A data area for the memory.
    typedef MAU* MAUTable;
    /// A data area for the memory.
    typedef std::vector<MAU> MAUVector;

    Memory();
    virtual ~Memory();

    virtual void initiateRead(Word address, int size, URC id)
        throw (OutOfRange) = 0;
    virtual void loadData(MAUVector& data, URC id) = 0;
    virtual std::pair<MAUTable, std::size_t> loadData(URC id) = 0;
    virtual bool resultReady(URC id) = 0;

    virtual void initiateWrite(
        Word address,
        Memory::MAUTable data,
        std::size_t size,
        URC id)
        throw (OutOfRange) = 0;

    virtual void advanceClock() = 0;
    virtual bool isAvailable() = 0;
    virtual void readBlock(Word address, MAUVector& data) = 0;
    virtual void writeBlock(Word address, MAUVector data) = 0;
    virtual void reset() = 0;
    virtual void fillWithZeros() = 0;
    bool isIdle() const;

protected:
    /// Flag telling whether tere are pending write requests.
    bool idle_;

private:
    /// Copying not allowed.
    Memory(const Memory&);
    /// Assignment not allowed.
    Memory& operator=(const Memory&);
};

#include "Memory.icc"

//////////////////////////////////////////////////////////////////////////////
// NullMemory
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null Memory.
 *
 * All methods cause program abort with an error log message.
 */
class NullMemory : public Memory {
public:
    virtual ~NullMemory();

    static NullMemory& instance();

    virtual void initiateRead(Word address, int size, URC id)
        throw (OutOfRange);

    virtual void loadData(Memory::MAUVector& data, URC id);
    virtual std::pair<Memory::MAUTable, std::size_t> loadData(URC id);
    virtual bool resultReady(URC id);

    virtual void initiateWrite(
        Word address,
        Memory::MAUTable data,
        std::size_t size,
        URC id)
        throw (OutOfRange);

    virtual void advanceClock();
    virtual bool isAvailable();
    virtual void readBlock(Word address, Memory::MAUVector& data);
    virtual void writeBlock(Word address, Memory::MAUVector data);
    virtual void reset();
    virtual void fillWithZeros();

protected:
    NullMemory();

private:
    /// Unique instance of the class.
    static NullMemory* instance_;
};

#endif
