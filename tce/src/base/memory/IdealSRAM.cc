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
 * @file IdealSRAM.cc
 *
 * Definition of IdealSRAM class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <utility>

#include "IdealSRAM.hh"
#include "MemoryContents.hh"
#include "Conversion.hh"
#include "Application.hh"

using std::string;


/**
 * Constructor. Create a model for a given memory.
 *
 * The created memory model is empty. No data is allocated for its contents.
 *
 * @param start First address of the memory.
 * @param end Last address of the memory.
 * @param MAUSize Bit width of the minimum addressable unit of the memory.
 * @param wordSize Number of MAUs that make up a natural word.
 * @param align Alignment of natural words, expressed in number of MAUs.
 */
IdealSRAM::IdealSRAM(Word start, Word end, Word MAUSize) :
    Memory(start, end, MAUSize), start_(start), end_(end), MAUSize_(MAUSize) {
    data_ = new MemoryContents(end_ - start_);
}


/**
 * Destructor.
 *
 * The storage reserved for the memory contents is deallocated. Any data
 * about pending access requests is freed, too.
 */
IdealSRAM::~IdealSRAM() {
    delete data_;
    data_ = NULL;
}

/**
 * Writes a single memory location.
 *
 * The fastest way to write to the memory.
 *
 * @param address The target address.
 * @param data The data to write.
 */
void
IdealSRAM::write(Word address, MAU data) {
    data_->writeData(address - start_, data);
}

/**
 * Reads a single memory location.
 *
 * The fastest way to read the memory.
 *
 * @param address The address to read.
 * @return The data read.
 */
Memory::MAU
IdealSRAM::read(Word address) {
    return data_->readData(address - start_);
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
IdealSRAM::fillWithZeros() {
    data_->clear();
}


