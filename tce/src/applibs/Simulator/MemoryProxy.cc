/**
 * @file MemoryProxy.cc
 *
 * Implementation of MemoryProxy class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel@cs.tut.fi)
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
    Memory(memory->start(), memory->end(), memory->MAUSize()), 
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
MemoryProxy::read(Word address) {

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
    Word address, MAU data) {

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
MemoryProxy::readAccess(unsigned idx) const
    throw (OutOfRange) {

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
MemoryProxy::writeAccess(unsigned idx) const
    throw (OutOfRange) {

    if (idx > writes_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    return writes_[idx];
}

