/**
 * @file NullAddress.hh
 *
 * Declaration of NullAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_ADDRESS_HH
#define TTA_NULL_ADDRESS_HH

#include "Address.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null address.
 *
 * Calling any method causes the program to abort.
 */
class NullAddress : public Address {
public:
    virtual ~NullAddress();
    static NullAddress& instance();

    UIntWord location() const;
    const TTAMachine::AddressSpace& space() const;

protected:
    NullAddress();

private:
    /// Copying not allowed.
    NullAddress(const NullAddress&);
    /// Assignment not allowed.
    NullAddress& operator=(const NullAddress&);

    static NullAddress instance_;
};

}

#endif
