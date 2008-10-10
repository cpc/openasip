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
 * @file Unit.hh
 *
 * Declaration of Unit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 22 Jun 2004 by ao, ml, vpj, ll
 */

#ifndef TTA_UNIT_HH
#define TTA_UNIT_HH

#include <string>
#include <vector>
#include <set>

#include "MachinePart.hh"
#include "Exception.hh"
#include "Port.hh"

namespace TTAMachine {

/**
 * An Abstract base class for the different units in the machine.
 */
class Unit : public Component {
public:
    virtual ~Unit();

    virtual bool hasPort(const std::string& name) const;
    virtual Port* port(const std::string& name) const
        throw (InstanceNotFound);
    virtual Port* port(int index) const
        throw (OutOfRange);
    virtual int portCount() const;

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for Unit.
    static const std::string OSNAME_UNIT;

protected:
    Unit(const std::string& name)
        throw (InvalidName);
    Unit(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual void removePort(Port& port);

private:
    /// Container for ports.
    typedef std::vector<Port*> PortTable;
    /// Set type for strings.
    typedef std::set<std::string> NameSet;

    /// Copying forbidden.
    Unit(const Unit&);
    /// Assingment forbidden.
    Unit& operator=(const Unit&);

    void addPort(Port& port)
        throw (ComponentAlreadyExists);
    void deleteAllPorts();
    void deleteOtherPorts(const NameSet& portsToLeave);
    static NameSet portNames(const ObjectState* state)
        throw (KeyNotFound);
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Contains all the ports of the unit.
    PortTable ports_;

    // Port is a friend class to be able add and remove itself from the unit
    friend class Port;
};
}

#endif
