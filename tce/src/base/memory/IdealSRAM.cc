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
IdealSRAM::IdealSRAM(Word start, Word end, Word MAUSize, bool littleEndian) :
    Memory(start, end, MAUSize, littleEndian), start_(start), end_(end), 
    MAUSize_(MAUSize) {
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


