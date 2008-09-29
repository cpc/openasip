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
 * @file MemorySystem.cc
 *
 * Definition of MemorySystem class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "MemorySystem.hh"
#include "Machine.hh"
#include "AddressSpace.hh"
#include "Memory.hh"
#include "MapTools.hh"
#include "Application.hh"

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
 * Destructor.
 *
 * Deletes all the Memory instances.
 */
MemorySystem::~MemorySystem() {
    MapTools::deleteAllValues(memories_);
}

/**
 * Adds AddressSpace and corresponding memory to data structure.
 *
 * @param as AddressSpace to be added.
 * @param mem Memory to be added. Becomes property of the MemorySystem,
 *            that is, MemorySystems is responsible for deallocating it.
 * @exception IllegalRegistration If the AddressSpace does not belong to the
 *                                target machine.
 */
void
MemorySystem::addAddressSpace(const AddressSpace& as, Memory* mem)
    throw (IllegalRegistration) {

    Machine::AddressSpaceNavigator nav = machine_->addressSpaceNavigator();
    if (!nav.hasItem(as.name())) {
        string msg = "Address space doesn't belong to the target machine";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }

    memories_[&as] = mem;
    memoryList_.push_back(mem);
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
 * @todo These methods need to be changed to compare with the AS attributes
 *       not with AS pointer!
 */
Memory&
MemorySystem::memory(const AddressSpace& as) 
    throw (InstanceNotFound) {

    MemoryMap::const_iterator iter = memories_.find(&as);
    if (iter == memories_.end()) {
        string msg = "No memory found for address space " + as.name();
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return *((*iter).second);
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
Memory& 
MemorySystem::memory(const std::string& addressSpaceName)
    throw (InstanceNotFound) {

    MemoryMap::const_iterator iter = memories_.begin();
    while (iter != memories_.end()) {
        const AddressSpace& space = *((*iter).first);
        Memory& mem = *((*iter).second);
        if (space.name() == addressSpaceName) {
            return mem;
        }
        ++iter;
    }
    string msg = "No memory found for address space " + addressSpaceName;
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
const Memory&
MemorySystem::memoryConst(const AddressSpace& as) const
    throw (InstanceNotFound) {

    MemoryMap::const_iterator iter = memories_.find(&as);
    if (iter == memories_.end()) {
        string msg = "No memory found for address space " + as.name();
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return *((*iter).second);
}

/**
 * Returns the count of Memory instances in the memory system.
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
Memory&
MemorySystem::memory(unsigned int i) 
    throw (OutOfRange) {

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
    return *((*iter).second);
}

/**
 * Returns the address space of the ith Memory instance in the memory system.
 *
 * @return Address Space of the memory instance.
 * @exception OutOfRange if i is out of bounds.
 */
const AddressSpace&
MemorySystem::addressSpace(unsigned int i)
    throw (OutOfRange) {

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
MemorySystem::addressSpace(const std::string& name)
    throw (InstanceNotFound) {

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
