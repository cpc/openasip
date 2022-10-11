/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MemoryProxy.hh
 *
 * Declaration of MemoryProxy class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_MEMORY_PROXY_HH
#define TTA_MEMORY_PROXY_HH

#include "Memory.hh"
#include <utility>
#include <vector>

class SimulatorFrontend;

/**
 * MemoryProxy is a memory access tracker for simulator.
 *
 * MemoryProxy tracks calls to reads and writes.
 * Memory accesses on the last clock cycle can be listed
 * with readAccessCount(), readAccess(idx), writeAccessCount()
 * and writeAccess(idx) functions. Memory access information is
 * stored for only one cycle. Tracks only single memory accesses, not
 * those made with the block methods.
 */
class MemoryProxy : public Memory {
public:

    typedef std::pair<Word, int> MemoryAccess;

    MemoryProxy(SimulatorFrontend& frontend, Memory* memory);
    virtual ~MemoryProxy();

    virtual void advanceClock();
    virtual void reset();

    virtual void write(ULongWord address, MAU data) override;
    virtual Memory::MAU read(ULongWord address) override;

    virtual void write(ULongWord address, int size, ULongWord data)
        { memory_->write(address, size, data); }
    virtual void read(ULongWord address, int size, ULongWord& data)
        { memory_->write(address, size, data); }

    virtual void fillWithZeros() { memory_->fillWithZeros(); }

    unsigned int readAccessCount() const;
    unsigned int writeAccessCount() const;

    MemoryAccess readAccess(unsigned int idx) const;

    MemoryAccess writeAccess(unsigned int idx) const;

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
