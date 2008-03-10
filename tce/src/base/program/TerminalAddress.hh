/**
 * @file TerminalAddress.hh
 *
 * Declaration of TerminalAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_ADDRESS_HH
#define TTA_TERMINAL_ADDRESS_HH

#include "TerminalImmediate.hh"
#include "Address.hh"
#include "AddressSpace.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

/**
 * Represents an inline immediate that refers to an address.
 */
class TerminalAddress : public TerminalImmediate {
public:
    TerminalAddress(SimValue value, const TTAMachine::AddressSpace& space);
    virtual ~TerminalAddress();

    virtual bool isAddress() const;
    virtual Address address() const throw (WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
private:
    /// Assignment not allowed.
    TerminalAddress& operator=(const TerminalAddress&);
    /// Address space of the address.
    const TTAMachine::AddressSpace& space_;
};

}

#endif
