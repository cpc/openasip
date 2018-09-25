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
 * @file DirectAccessMemory.hh
 *
 * Declaration of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_DIRECT_ACCESS_MEMORY_HH
#define TTA_DIRECT_ACCESS_MEMORY_HH

#include "Memory.hh"
#include "BaseType.hh"

class MemoryContents;

/**
 * Class that models an "ideal" memory to which updates are visible
 * immediately.
 *
 * This model is used in compiled simulation. It does not require an
 * advance clock call: all writes to it are visible immediately. Thus,
 * one has to make sure that all reads in the same cycle are executed
 * before writes in order for the reads to read the old values.
 *
 * Note that all range checking is disabled for fastest possible simulation
 * model. In case you are unsure of your simulated input correctness, use
 * the old simulation engine for verification.
 */
class DirectAccessMemory : public Memory {
public:
    DirectAccessMemory(
        ULongWord start,
        ULongWord end,
        Word MAUSize, bool littleEndian);

    virtual ~DirectAccessMemory();

    void write(ULongWord address, Memory::MAU data) override;
    
    void fastWriteMAU(
        ULongWord address,
        ULongWord data);
    
    void fastWrite2MAUsBE(
        ULongWord address,
        ULongWord data);
    
    void fastWrite4MAUsBE(
        ULongWord address,
        ULongWord data);

    void fastWrite2MAUsLE(
        ULongWord address,
        ULongWord data);
    
    void fastWrite4MAUsLE(
        ULongWord address,
        ULongWord data);

    Memory::MAU read(ULongWord address) override;
    
    void fastReadMAU(
        ULongWord address,
        ULongWord& data);
    
    void fastRead2MAUsBE(
        ULongWord address,
        ULongWord& data);
    
    void fastRead4MAUsBE(
        ULongWord address,
        ULongWord& data);

    void fastRead2MAUsLE(
        ULongWord address,
        ULongWord& data);
    
    void fastRead4MAUsLE(
        ULongWord address,
        ULongWord& data);

    virtual void advanceClock() {}
    virtual void reset() {}
    virtual void fillWithZeros();

    void writeBE(ULongWord address, int count, ULongWord data) override;


    using Memory::write;
    using Memory::read;

private:
    /// Copying not allowed.
    DirectAccessMemory(const DirectAccessMemory&);
    /// Assignment not allowed.
    DirectAccessMemory& operator=(const DirectAccessMemory&);

    /// Starting point of the address space.
    Word start_;
    /// End point of the address space.
    Word end_;
    /// Size of the minimum adressable unit.
    Word MAUSize_;
    /// precalculated MAUSize_ * 3
    Word MAUSize3_;
    /// precalculated MAUSize_ * 2
    Word MAUSize2_;    
    /// Size of the natural word as MAUs.
    Word wordSize_;
    /// Mask bit pattern for unpacking IntWord to MAUs.
    Word mask_;
    /// Contains MAUs of the memory model, that is, the actual data of the
    /// memory.
    MemoryContents* data_;
};

#endif
