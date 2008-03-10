/**
 * @file Unit.hh
 *
 * Declaration of Unit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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
