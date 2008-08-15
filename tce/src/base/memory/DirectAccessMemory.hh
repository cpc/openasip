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
 * @file DirectAccessMemory.hh
 *
 * Declaration of DirectAccessMemory class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
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
        Word start,
        Word end,
        Word MAUSize);

    virtual ~DirectAccessMemory();

    void write(Word address, Memory::MAU data);
    
    void fastWriteMAU(
        Word address,
        UIntWord data);
    
    void fastWrite2MAUs(
        Word address,
        UIntWord data);
    
    void fastWrite4MAUs(
        Word address,
        UIntWord data);

    Memory::MAU read(Word address);
    
    void fastReadMAU(
        Word address,
        UIntWord& data);
    
    void fastRead2MAUs(
        Word address,
        UIntWord& data);
    
    void fastRead4MAUs(
        Word address,
        UIntWord& data);

    virtual void advanceClock() {}
    virtual void reset() {}
    virtual void fillWithZeros();

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
