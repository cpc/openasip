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
 * @file MemorySystem.hh
 *
 * Declaration of MemorySystem class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_SYSTEM_HH
#define TTA_MEMORY_SYSTEM_HH

#include <map>
#include <vector>
#include <string>

#include "Exception.hh"

class Memory;

namespace TTAMachine {
    class Machine;
    class AddressSpace;
}

/**
 * Binds AddressSpace instances to Memory instances.
 */
class MemorySystem {
public:
    explicit MemorySystem(const TTAMachine::Machine& machine);
    virtual ~MemorySystem();

    void addAddressSpace(const TTAMachine::AddressSpace& as, Memory* mem)
        throw (IllegalRegistration);
    Memory& memory(const TTAMachine::AddressSpace& as)
        throw (InstanceNotFound);
    Memory& memory(const std::string& addressSpaceName)
        throw (InstanceNotFound);

    const Memory& memoryConst(const TTAMachine::AddressSpace& as) const
        throw (InstanceNotFound);

    unsigned int memoryCount() const;
    Memory& memory(unsigned int i)
        throw (OutOfRange);
    const TTAMachine::AddressSpace& addressSpace(unsigned int i)        
        throw (OutOfRange);
    const TTAMachine::AddressSpace& addressSpace(const std::string& name)
        throw (InstanceNotFound);

    void advanceClockOfAllMemories();
    void resetAllMemories();
    void fillAllMemoriesWithZero();

private:
    /// Copying not allowed.
    MemorySystem(const MemorySystem&);
    /// Assignment not allowed.
    MemorySystem& operator=(const MemorySystem&);

    /// Maps address spaces to memory instances.
    typedef std::map<const TTAMachine::AddressSpace*, Memory*> MemoryMap;

    /// Container for memory instances for faster traversal.
    typedef std::vector<Memory*> MemoryContainer;

    /// Machine in which MemorySystem belongs to.
    const TTAMachine::Machine* machine_;
    /// Contains all memories indexed by AddressSpaces.
    MemoryMap memories_;

    /// List of the memories for faster traversal.
    MemoryContainer memoryList_;
};
#include "MemorySystem.icc"

#endif
