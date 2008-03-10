/**
 * @file MemorySystem.hh
 *
 * Declaration of MemorySystem class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
