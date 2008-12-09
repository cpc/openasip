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
