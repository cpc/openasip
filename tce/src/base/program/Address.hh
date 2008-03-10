/**
 * @file Address.hh
 *
 * Declaration of Address class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADDRESS_HH
#define TTA_ADDRESS_HH

#include "BaseType.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

/**
 * An address in the target memory system of the TTA program.
 *
 * An address is a number that identifies a location in an address space. An
 * address can point to a location of data memory, instruction memory or
 * unified memory.
 */
class Address {
public:
    Address(
        InstructionAddress location,
        const TTAMachine::AddressSpace& space);
    ~Address();

    InstructionAddress location() const;
    const TTAMachine::AddressSpace& space() const;

private:
    /// The location identified by the address.
    InstructionAddress location_;
    /// The address space of the address.
    const TTAMachine::AddressSpace* space_;
};

#include "Address.icc"

}

#endif
