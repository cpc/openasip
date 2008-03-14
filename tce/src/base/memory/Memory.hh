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
 * The interface provides methods for emulating the access to data memory by 
 * operations of the target architecture such as load and store. In addition,
 * an interface is implemented for direct access to the memory storage for
 * the debugging user interfaces.
 *
 * The abstract base class Memory implements all functionality except for
 * the actual write() and read() methods which write and read a single unit 
 * to the memory storage as efficiently as possible. That is left for the 
 * derived classes to implement, as it depends on the storage data structure 
 * used, etc.
 *
 * Memory base class implements the correct ordering of loads and stores within
 * the same cycle: loads in the same cycle do not see the values of 
 * the writes in that cycle. That is, the writes are committed to the memory 
 * array at cycleAdvance() call. Derived classes may loosen this behavior
 * and let the client take care of the correct ordering of the memory
 * accesses, as is the case with the compiled simulation engine and the
 * DirectAccessMemory implementation it uses for simulating data memory.
 *
 * The Memory abstraction deals with MAUs (commonly bytes). The client can
 * access the Memory for storing writing doubles and floats in case it 
 * implements floating point memory operations. Interface for those is
 * out of the abstraction level of this interface.
 */
class Memory {
public:
    typedef MinimumAddressableUnit MAU;
    typedef MAU* MAUTable;
    typedef std::vector<MAU> MAUVector;

    Memory(Word start, Word end, Word MAUSize);
    virtual ~Memory();

    virtual void advanceClock();

    virtual void write(Word address, MAU data) = 0;
    virtual Memory::MAU read(Word address) = 0;

    virtual void write(Word address, int size, UIntWord data)
        throw (OutOfRange);
    virtual void write(Word address, FloatWord data)
        throw (OutOfRange);
    virtual void write(Word address, DoubleWord data)
        throw (OutOfRange);
    virtual void read(Word address, int size, UIntWord& data)
        throw (OutOfRange);
    virtual void read(Word address, FloatWord& data)
        throw (OutOfRange);
    virtual void read(Word address, DoubleWord& data)
        throw (OutOfRange);

    virtual void reset();
    virtual void fillWithZeros();

    virtual void writeBlock(Word address, Memory::MAUVector data);
    virtual void readBlock(Word address, Memory::MAUVector& data);

    virtual Word start() { return start_; }
    virtual Word end() { return end_; }
    virtual Word MAUSize() { return MAUSize_; }

private:
    /// Copying not allowed.
    Memory(const Memory&);
    /// Assignment not allowed.
    Memory& operator=(const Memory&);

    void pack(const Memory::MAUTable data, int size, UIntWord& value);
    void unpack(const UIntWord& value, std::size_t size, Memory::MAUTable data);

    void checkRange(Word startAddress, int numberOfMAUs)
        throw (OutOfRange);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;

    /**
     * Models an uncommitted write request.
     */
    struct WriteRequest {
        WriteRequest() :
            data_(NULL), dataSize_(0), address_(0), size_(0) {}
        /// Data to be written.
        Memory::MAUTable data_;
        /// Data table size.
        std::size_t dataSize_;
        /// Address to be written to.
        Word address_;
        /// Size of the data to be read/written.
        int size_;
    };
    typedef std::vector<WriteRequest*> RequestQueue;
    /// The uncommited write requests.
    RequestQueue writeRequests_;
    /// Mask bit pattern for unpacking IntWord to MAUs.
    int mask_;

};

/// Maximum number of MAUs in a single request supported by the interface.
#define MAX_ACCESS_SIZE 64

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

    virtual void write(Word address, MAU data);
    virtual Memory::MAU read(Word address);

protected:
    NullMemory();

private:
    /// Unique instance of the class.
    static NullMemory* instance_;
};

#endif
