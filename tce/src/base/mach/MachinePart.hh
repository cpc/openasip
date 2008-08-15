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
public:
    struct Comparator {
        bool operator() (const MachinePart* mp1, const MachinePart* mp2) const;
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
