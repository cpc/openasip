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
 * @file MemorySystem.hh
 *
 * Declaration of MemorySystem class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2009 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_SYSTEM_HH
#define TTA_MEMORY_SYSTEM_HH

#include <map>
#include <vector>
#include <string>

#include "Exception.hh"

class Memory;
class TCEString;

namespace TTAMachine {
    class Machine;
    class AddressSpace;
}

/**
 * The collection of memory simulation models seen by a single core.
 */
class MemorySystem {
public:
    explicit MemorySystem(const TTAMachine::Machine& machine);
    virtual ~MemorySystem();

    void addAddressSpace(
        const TTAMachine::AddressSpace& as, 
        Memory* mem,
        bool shared=true)
        throw (IllegalRegistration);

    Memory& memory(const TTAMachine::AddressSpace& as)
        throw (InstanceNotFound);
    const Memory& memoryConst(const TTAMachine::AddressSpace& as) const
        throw (InstanceNotFound);

    Memory& memory(const std::string& addressSpaceName)
        throw (InstanceNotFound);

    unsigned int memoryCount() const;
    Memory& memory(unsigned int i)
        throw (OutOfRange);
    const TTAMachine::AddressSpace& addressSpace(unsigned int i)        
        throw (OutOfRange);
    const TTAMachine::AddressSpace& addressSpace(const std::string& name)
        throw (InstanceNotFound);

    void shareMemoriesWith(MemorySystem& other);

    void advanceClockOfLocalMemories();
    void advanceClockOfSharedMemories();
    void resetAllMemories();
    void fillAllMemoriesWithZero();
    void deleteSharedMemories();

    bool hasMemory(const TCEString& aSpaceName) const;

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
    /// List of all the memories for faster traversal.
    MemoryContainer memoryList_;
    /// All memories shared between multiple cores.
    MemoryContainer sharedMemories_;
    /// All private/local memories used by a single core only.
    MemoryContainer localMemories_;
    /// Shared memories which have been replaced with a shared memory
    /// from another core. Just for garbage removal.
    MemoryContainer replacedSharedMemories_;
};
#include "MemorySystem.icc"

#endif
