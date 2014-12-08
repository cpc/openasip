/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file Memory.hh
 *
 * Declaration of the base interface for the memory model (Memory class).
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */


#ifndef TTA_MEMORY_MODEL_HH
#define TTA_MEMORY_MODEL_HH

#include "BaseType.hh"

struct WriteRequest;
struct RequestQueue;

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

    Memory(Word start, Word end, Word MAUSize);
    virtual ~Memory();

    virtual void advanceClock();

    virtual void write(Word address, MAU data) = 0;
    virtual Memory::MAU read(Word address) = 0;

    virtual void write(Word address, int size, UIntWord data);
    virtual void write(Word address, FloatWord data);
    virtual void write(Word address, DoubleWord data);
    virtual void read(Word address, int size, UIntWord& data);
    virtual void read(Word address, FloatWord& data);
    virtual void read(Word address, DoubleWord& data);

    virtual void reset();
    virtual void fillWithZeros();

    virtual Word start() { return start_; }
    virtual Word end() { return end_; }
    virtual Word MAUSize() { return MAUSize_; }

protected:

    void pack(const Memory::MAUTable data, int size, UIntWord& value);
    void unpack(const UIntWord& value, int size, Memory::MAUTable data);

private:
    /// Copying not allowed.
    Memory(const Memory&);
    /// Assignment not allowed.
    Memory& operator=(const Memory&);

    void checkRange(Word startAddress, int numberOfMAUs);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;

    /// The uncommited write requests.
    RequestQueue* writeRequests_;
    /// Mask bit pattern for unpacking IntWord to MAUs.
    int mask_;

};

/// Maximum number of MAUs in a single request supported by the interface.
#define MAX_ACCESS_SIZE 64

#include "Memory.icc"

#endif
