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
 * @file MachinePart.hh
 *
 * Declaration of MachinePart class and derived Component and SubComponent
 * classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#ifndef TTA_MACHINE_PART_HH
#define TTA_MACHINE_PART_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

class ObjectState;

namespace TTAMachine {

class Machine;

////////////////////////////////////////////////////////////////////////////
// MachinePart
////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for all the machine components.
 */
class MachinePart : public Serializable {
public:
    struct Comparator {
        inline bool operator() (
            const MachinePart* mp1, 
            const MachinePart* mp2) const;
    };
protected:
    MachinePart();
    virtual ~MachinePart();
private:
    /// Copying forbidden.
    MachinePart(const MachinePart&);
    /// Assingment forbidden.
    MachinePart& operator=(const MachinePart&);
    /// Id just for comparison for sets and maps.
    /// More deterministic than pointer to the object.
    static int idCounter_;
    int id_;
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

    /**
     * Compares 2 Component's names lexicographically (dictionary order).
     *
     * Can be used to organize containers of type Component to dictionary
     * order according to their name field.
     * @param a the first Component to compare.
     * @param b the second Component to compare.
     * @return true, if a comes before b in dictionary order.
     */
    class ComponentNameComparator {
        public:
            bool operator () (const Component* a, const Component* b) const {
                return (a->name()) < (b->name());
            }
    };

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
