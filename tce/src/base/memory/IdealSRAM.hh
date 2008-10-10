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
 * @file IdealSRAM.hh
 *
 * Declaration of IdealSRAM class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IDEAL_SRAM_HH
#define TTA_IDEAL_SRAM_HH

#include <vector>
#include <map>

#include "Memory.hh"

class MemoryContents;

/**
 * Class that models an "ideal" memory.
 *
 * An ideal memory is defined as a memory with read latency zero. 
 * The data is available at the same cycle in which the load is initiated. 
 * Also, after a store is initiated, data is written into memory as soon as
 * the clock advances.
 *
 * This implementation uses a "paged array" as the storage structure which
 * avoids unnecessary allocation while providing O(1) access time. See
 * PagedArray for more details.
 */
class IdealSRAM : public Memory {
public:
    IdealSRAM(Word start, Word end, Word MAUSize);
    virtual ~IdealSRAM();

    virtual void write(Word address, MAU data);
    virtual Memory::MAU read(Word address);

    using Memory::write;
    using Memory::read;

    virtual void fillWithZeros();

private:
    /// Copying not allowed.
    IdealSRAM(const IdealSRAM&);
    /// Assignment not allowed.
    IdealSRAM& operator=(const IdealSRAM&);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;
    /// Container for holding read/write requests.
    MemoryContents* data_;
};

#endif
