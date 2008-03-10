/**
 * @file TargetMemory.hh
 *
 * Declaration of TargetMemory class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TARGET_MEMORY_HH
#define TTA_TARGET_MEMORY_HH

#include <vector>

#include "Memory.hh"
#include "Exception.hh"
#include "BaseType.hh"

/**
 * Adaptor class that offers user friendly interface to memory model module.
 */
class TargetMemory {
public:
    /// Vector of IntWords.
    typedef std::vector<UIntWord> UIntWordVector;
    /// Vector of FloatWords.
    typedef std::vector<FloatWord> FloatWordVector;
    /// Vector of DoubleWords.
    typedef std::vector<DoubleWord> DoubleWordVector;

    TargetMemory(Memory& memory, bool bigEndian, int MAUSize);
    virtual ~TargetMemory();

    void initiateRead(Word address, int size, Memory::URC id)
        throw (OutOfRange);

    void initiateWrite(Word address, int size, UIntWord data, Memory::URC id)
        throw (OutOfRange);
    void initiateWrite(Word address, int size, FloatWord data, Memory::URC id)
        throw (OutOfRange);
    void initiateWrite(Word address, int size, DoubleWord data, Memory::URC id)
        throw (OutOfRange);

    void readData(UIntWord& data, Memory::URC id);
    void readData(FloatWord& data, Memory::URC id);
    void readData(DoubleWord& data, Memory::URC id);

    bool isResultReady(Memory::URC id);
    void advanceClock();

    void readBlock(Word address, UIntWordVector& data, int width);
    void readBlock(Word address, FloatWordVector& data, int width);
    void readBlock(Word address, DoubleWordVector& data, int width);

    void writeBlock(Word address, UIntWordVector data, int width);
    void writeBlock(Word address, FloatWordVector data, int width);
    void writeBlock(Word address, DoubleWordVector data, int width);

    int mauWidth() const;

private:
    /// Copying not allowed.
    TargetMemory(const TargetMemory&);
    /// Assignment not allowed.
    TargetMemory& operator=(const TargetMemory&);

    void pack(const Memory::MAUVector& data, int size, UIntWord& value);
    void pack(const Memory::MAUVector& data, int size, FloatWord& value);
    void pack(const Memory::MAUVector& data, int size, DoubleWord& value);
    void pack(const Memory::MAUTable data, int size, UIntWord& value);
    void pack(const Memory::MAUTable data, int size, FloatWord& value);
    void pack(const Memory::MAUTable data, int size, DoubleWord& value);
    void unpack(const UIntWord& value, int size, Memory::MAUVector& data);
    void unpack(const UIntWord& value, std::size_t size, Memory::MAUTable data);

    template<typename source>
    int asInteger(source& src, int index);

    /// Enables access to memory contents.
    Memory& memory_;
    /// True if byte order of target machine is big endian.
    bool bigEndian_;
    /// The size of MAU in bits.
    int sizeOfMAU_;
    /// Mask bit pattern for unpacking IntWord to MAUs.
    int mask_;
    /// The supported maximum access size (with initiate{Read,Write}) in MAUs.
    static const std::size_t MAX_ACCESS_SIZE = 16;
};

#include "TargetMemory.icc"

#endif
