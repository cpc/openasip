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
 * @file MemoryProxy.cc
 *
 * Implementation of MemoryProxy class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MemoryProxy.hh"
#include "SimulatorToolbox.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorFrontend.hh"


/**
 * The Constructor.
 *
 * MemoryProxy takes ownership of the wrapped Memory instance.
 *
 * @param memory Tracked memory.
 */
MemoryProxy::MemoryProxy(SimulatorFrontend& frontend, Memory* memory) :
    Memory(memory->start(), memory->end(), memory->MAUSize(),
           memory->isLittleEndian()),
    frontend_(frontend), memory_(memory) {
}


/**
 * The Destructor.
 *
 * Deletes the tracked memory instance.
 */
MemoryProxy::~MemoryProxy() {
    delete memory_;
}

/**
 * Tracks read calls.
 *
 * Passes the call to the wrapped memory.
 */
Memory::MAU
MemoryProxy::read(ULongWord address) {

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_MEMORY_ACCESS);

    newReads_.push_back(std::make_pair(address, 1));
    return memory_->read(address);
}

/**
 * Tracks write calls.
 *
 * Passes the call to the wrapped memory.
 */
void
MemoryProxy::write(
    ULongWord address, MAU data) {

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_MEMORY_ACCESS);

    newWrites_.push_back(std::make_pair(address, 1));
    memory_->write(address, data);
}

/**
 * Resets the memory access information for the last cycle when
 * wrapped memory clock is advanced.
 */
void
MemoryProxy::advanceClock() {
    writes_ = newWrites_;
    reads_ = newReads_;
    newWrites_.clear();
    newReads_.clear();
    memory_->advanceClock();
}

/**
 * Resets all memory access information when the wrapped memory is reset.
 */
void
MemoryProxy::reset() {
    reads_.clear();
    writes_.clear();
    newReads_.clear();
    newWrites_.clear();
    memory_->reset();
}

/**
 * Returns number of read accesses on the last cycle.
 */
unsigned int
MemoryProxy::readAccessCount() const {
    return reads_.size();
}

/**
 * Returns information of the given memory read access.
 *
 * @param idx Index of the read access.
 * @exception OutOfRange If the index is invalid.
 */
MemoryProxy::MemoryAccess
MemoryProxy::readAccess(unsigned idx) const {

    if (idx > reads_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    return reads_[idx];
}

/**
 * Returns number of write accesses on the last cycle.
 */
unsigned int
MemoryProxy::writeAccessCount() const {
    return writes_.size();
}


/**
 * Returns information of the given memory write access.
 *
 * @param idx Index of the write access.
 * @exception OutOfRange If the index is invalid.
 */
MemoryProxy::MemoryAccess
MemoryProxy::writeAccess(unsigned idx) const {

    if (idx > writes_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    return writes_[idx];
}

