/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file MemorySystem.cc
 *
 * Definition of MemorySystem class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2009-2012
 * @note rating: red
 */

#include <string>
#include <algorithm>

#include "MemorySystem.hh"
#include "Machine.hh"
#include "AddressSpace.hh"
#include "Memory.hh"
#include "MapTools.hh"
#include "Application.hh"
#include "SequenceTools.hh"
#include "Conversion.hh"

using std::string;
using namespace TTAMachine;

/**
 * Constructor.
 *
 * @param machine Machine in which MemorySystem belongs to.
 */
MemorySystem::MemorySystem(const Machine& machine) : 
    machine_(&machine) {
}

/**
 * Deletes all shared memory instances.
 *
 * Must be called only once for all MemorySystems sharing the memories.
 */
void
MemorySystem::deleteSharedMemories() {
    sharedMemories_.clear();
}

/**
 * Destructor.
 *
 * Deletes all the Memory instances.
 */
MemorySystem::~MemorySystem() {
    localMemories_.clear();
    replacedSharedMemories_.clear();
}

/**
 * Adds AddressSpace and corresponding memory to data structure.
 *
 * @param as AddressSpace to be added.
 * @param mem Memory to be added. Becomes property of the MemorySystem,
 *            that is, MemorySystems is responsible for deallocating it.
 * @param shared If the given Memory instace is shared by multiple 
 *               MemorySystems (cores).
 * @exception IllegalRegistration If the AddressSpace does not belong to the
 *                                target machine.
 */
void
MemorySystem::addAddressSpace(
    const AddressSpace& as, MemoryPtr mem, bool shared) {
    Machine::AddressSpaceNavigator nav = machine_->addressSpaceNavigator();
    if (!nav.hasItem(as.name())) {
        string msg = "Address space doesn't belong to the target machine";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }

    memories_[&as] = mem;

    if (shared) {
        sharedMemories_.push_back(mem);
    } else {
        localMemories_.push_back(mem);
    }
    memoryList_.push_back(mem);
}

bool
MemorySystem::hasMemory(const TCEString& aSpaceName) const {
    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end()) {
        const AddressSpace& space = *((*iter).first);
        if (space.name() == aSpaceName) {
            return true;
        }
        ++iter;
    }
    return false;
}

/**
 * In case two TTA simulation models share memories in an heterogeneous
 * multicore simulation, this method should be called after initializing
 * the simulation frontends between all such  pairs to fix the memory model 
 * references to point to the same memory model.
 *
 * The matching is done by the address space name. The shared address
 * space must have the 'shared' attribute set.
 *
 * @fixme An untested method.
 */
void
MemorySystem::shareMemoriesWith(MemorySystem& other) {
    for (std::size_t i = 0; i < other.memoryCount(); ++i) {
        const AddressSpace& as = other.addressSpace(i);
        if (!as.isShared() || !hasMemory(as.name())) continue;

        Machine::AddressSpaceNavigator nav = machine_->addressSpaceNavigator();
        AddressSpace* thisAS = nav.item(as.name());
        /// remove the replaced memory as it should not be controlled
        /// by this MemorySystem anymore
        memoryList_.erase(
            std::find(
                memoryList_.begin(), memoryList_.end(), 
                memories_[thisAS]));

        sharedMemories_.erase(
            std::find(
                sharedMemories_.begin(), sharedMemories_.end(),
                memories_[thisAS]));

        replacedSharedMemories_.push_back(memories_[thisAS]);

        memories_[thisAS] = other.memory(i);

    }
}

/**
 * Returns Memory instance bound to the given AddressSpace.
 *
 * The returned memory instance is writable.
 *
 * @param as AddressSpace in which memory is asked.
 * @return Memory bound to AddressSpace.
 * @exception InstanceNotFound If no memory is bound to the given 
 *                             AddressSpace.
 */
MemorySystem::MemoryPtr
MemorySystem::memory(const AddressSpace& as) {
    return memory(as.name());
}

/**
 * Returns Memory instance bound to an address space with the given name.
 *
 * The returned memory instance is writable.
 *
 * @param addressSpaceName The name of the address space.
 * @return Memory bound to AddressSpace.
 * @exception InstanceNotFound If no memory is found with the name.
 */
MemorySystem::MemoryPtr
MemorySystem::memory(const std::string& addressSpaceName) {
    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end()) {
        const AddressSpace& space = *((*iter).first);
        MemoryPtr mem = ((*iter).second);
        if (space.name() == addressSpaceName) {
            return mem;
        }
        ++iter;
    }
    string msg = "No memory model found for address space " + addressSpaceName;
    throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Returns Memory instance bound to the given AddressSpace.
 *
 * The returned memory instance is read-only.
 *
 * @param as AddressSpace in which memory is asked.
 * @return Memory bound to AddressSpace.
 * @exception InstanceNotFound If no memory is bound to the given 
 *                             AddressSpace.
 * @todo These methods need to be changed to compare with the AS attributes
 *       not with AS pointer!
 */
const MemorySystem::MemoryPtr
MemorySystem::memoryConst(const AddressSpace& as) const {
    MemoryMap::const_iterator iter = memories_.find(&as);
    if (iter == memories_.end()) {
        string msg = "No memory found for address space " + as.name();
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return ((*iter).second);
}

/**
 * Returns the count of Memory instances in this memory system.
 *
 * @return The count of Memory instances.
 */
unsigned int
MemorySystem::memoryCount() const {
    return memories_.size();
}

/**
 * Returns the ith Memory instance in the memory system.
 *
 * @return The Memory instance.
 * @exception OutOfRange if i is out of bounds.
 */
MemorySystem::MemoryPtr
MemorySystem::memory(unsigned int i) {
    if (i > memories_.size() - 1) {
        const string msg = 
	    "Index out of bounds. Count of memories is " + 
	    Conversion::toString(memoryCount());
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    unsigned int foundCount = 0;
    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end() && foundCount < i) {
        ++iter;
        ++foundCount;
    }
    return ((*iter).second);
}

/**
 * Returns the address space of the ith Memory instance in the memory system.
 *
 * @return Address Space of the memory instance.
 * @exception OutOfRange if i is out of bounds.
 */
const AddressSpace&
MemorySystem::addressSpace(unsigned int i) {
    if (i > memories_.size() - 1) {
        const string msg = 
	    "Index out of bounds. Count of memories is " + 
	    Conversion::toString(memoryCount());
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    unsigned int foundCount = 0;
    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end() && foundCount < i) {
        ++iter;
        ++foundCount;
    }
    return *((*iter).first);
}

/**
 * Returns the address space with the given name.
 *
 * @param name The name of the address space.
 * @return AddressSpace.
 * @exception InstanceNotFound if the address space is not found.
 */
const AddressSpace&
MemorySystem::addressSpace(const std::string& name) {
    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end()) {
        if ((*iter).first->name() == name)
            return *((*iter).first);
        ++iter;
    }

    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, 
        "Address space with the given name not found.");
}
