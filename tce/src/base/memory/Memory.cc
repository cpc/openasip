/**
 * @file Memory.cc
 *
 * Non-inline definitions of Memory class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <cstddef>

#include "Memory.hh"
#include "MemoryContents.hh"
#include "Application.hh"

//////////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * A new memory model is in an idle state, that is, it has no pending
 * access requests.
 */
Memory::Memory() : idle_(false) {
}


/**
 * Destructor.
 */
Memory::~Memory() {
}

/**
 * Resets the memory.
 *
 * Clears any request queues.
 */
void
Memory::reset() {
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
Memory::fillWithZeros() {
}

//////////////////////////////////////////////////////////////////////////////
// NullMemory
//////////////////////////////////////////////////////////////////////////////

NullMemory* NullMemory::instance_ = NULL;

/**
 * Constructor.
 */
NullMemory::NullMemory() : Memory() {
}

/**
 * Destructor.
 */
NullMemory::~NullMemory() {
}

/**
 * Returns an instance of NullMemory class (singleton).
 *
 * @return Singleton instance of NullMemory class.
 */
NullMemory&
NullMemory::instance() {
    if (instance_ == NULL) {
        instance_ = new NullMemory();
    }
    return *instance_;
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Doesn't throw.
 */
void
NullMemory::initiateRead(Word, int, URC)
    throw (OutOfRange) {

    abortWithError("initiateRead()");
}

/**
 * Aborts the program with error message.
 */
void
NullMemory::loadData(Memory::MAUVector&, URC) {
    abortWithError("loadData()");
}

/**
 * Aborts the program with error message.
 */
std::pair<Memory::MAUTable, std::size_t>
NullMemory::loadData(URC) {
    abortWithError("loadData()");
    throw 1;
}


/**
 * Aborts the program with error message.
 *
 * @return Nothing.
 */
bool
NullMemory::resultReady(URC) {
    abortWithError("resultReady()");
    return false;
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Doesn't throw.
 */
void
NullMemory::initiateWrite(Word, Memory::MAUTable, std::size_t, URC)
    throw (OutOfRange) {
    abortWithError("initiateWrite()");
}

/**
 * Aborts the program with error message.
 */
void
NullMemory::advanceClock() {
    abortWithError("advanceClock()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullMemory::isAvailable() {
    abortWithError("isAvailable()");
    return false;
}

/**
 * Aborts the program with error message.
 */
void
NullMemory::readBlock(Word, Memory::MAUVector&) {
    abortWithError("readBlock()");
}

/**
 * Aborts the program with error message.
 */
void
NullMemory::writeBlock(Word, Memory::MAUVector) {
    abortWithError("writeBlock()");
}

/**
 * Aborts the program with error message.
 */
void
NullMemory::reset() {
    abortWithError("reset()");
}

/**
 * Fills the whole memory with zeros.
 *
 * This is needed due to some buggy simulated programs which expect
 * uninitialized data to be zero.
 */
void
NullMemory::fillWithZeros() {
    abortWithError("fillWithZeros()");
}


