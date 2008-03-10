/**
 * @file NullAddressSpace.hh
 *
 * Declaration of NullAddressSpace class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_ADDRESS_SPACE_HH
#define TTA_NULL_ADDRESS_SPACE_HH

#include "AddressSpace.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null AddressSpace.
 *
 * All the methods abort the program.
 */
class NullAddressSpace : public AddressSpace {
public:
    static NullAddressSpace& instance();

    virtual int width() const;
    virtual unsigned int start() const;
    virtual unsigned int end() const;

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setAddressBounds(unsigned int start, unsigned int end)
        throw (OutOfRange);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullAddressSpace();
    virtual ~NullAddressSpace();

    /// The only instance of NullAddressSpace.
    static NullAddressSpace instance_;
    /// Machine to which the null address space is registered.
    static Machine machine_;
};
}

#endif
