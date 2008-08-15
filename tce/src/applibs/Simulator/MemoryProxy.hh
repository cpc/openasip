/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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

    virtual void write(Word address, MAU data);
    virtual Memory::MAU read(Word address);

    virtual void write(Word address, int size, UIntWord data)
        { memory_->write(address, size, data); }
    virtual void read(Word address, int size, UIntWord& data)
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
