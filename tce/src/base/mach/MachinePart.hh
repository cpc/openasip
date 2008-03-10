/**
 * @file MachinePart.hh
 *
 * Declaration of MachinePart class and derived Component and SubComponent
 * classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#ifndef TTA_MACHINE_PART_HH
#define TTA_MACHINE_PART_HH

#include <string>

#include "ObjectState.hh"
#include "Serializable.hh"
#include "Exception.hh"

namespace TTAMachine {

class Machine;

////////////////////////////////////////////////////////////////////////////
// MachinePart
////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for all the machine components.
 */
class MachinePart : public Serializable {
protected:
    MachinePart();
    virtual ~MachinePart();

private:
    /// Copying forbidden.
    MachinePart(const MachinePart&);
    /// Assingment forbidden.
    MachinePart& operator=(const MachinePart&);
};


////////////////////////////////////////////////////////////////////////////
// Component
////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for top-level machine components.
 *
 * Top-level machine components, like Bus and Socket, are independently
 * defined parts of a target machine. Components can be registered to (or
 * unregistered from) a Machine.
 */
class Component : public MachinePart {
public:
    virtual ~Component();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    /**
     * Registers component into given Machine.
     *
     * @param machine A Machine.
     */
    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists) = 0;

    /**
     * Removes component from its current Machine.
     */
    virtual void unsetMachine() = 0;

    virtual Machine* machine() const;
    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    // methods inherited from Serializable interface
    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for component.
    static const std::string OSNAME_COMPONENT;
    /// ObjectState attribute key for the name of the component.
    static const std::string OSKEY_NAME;

protected:
    Component(const std::string& name)
        throw (InvalidName);
    Component(const ObjectState* state)
        throw (ObjectStateLoadingException);

    void internalSetMachine(Machine& machine);
    void internalUnsetMachine();

private:
    /// Copying forbidden.
    Component(const Component&);
    /// Assingment forbidden.
    Component& operator=(const Component&);

    /// Name of the component.
    std::string name_;
    /// Machine to which the component is registered.
    Machine* machine_;
};

////////////////////////////////////////////////////////////////////////////
// SubComponent
////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for non-top-level machine components.
 *
 * Subcomponents belong to and are directly managed by one Component (not by
 * a Machine).
 */
class SubComponent : public MachinePart {
protected:
    SubComponent();
    virtual ~SubComponent();

private:
    /// Copying forbidden.
    SubComponent(const SubComponent&);
    /// Assingment forbidden.
    SubComponent& operator=(const SubComponent&);
};
}

#include "MachinePart.icc"

#endif
