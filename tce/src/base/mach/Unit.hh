/*
    Copyright (c) 2002-2009 Tampere University.

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

namespace TTAMachine {

class Port;

/**
 * An Abstract base class for the different units in the machine.
 */
class Unit : public Component {
public:
    virtual ~Unit();

    virtual bool hasPort(const std::string& name) const;
    virtual Port* port(const std::string& name) const;
    virtual Port* port(int index) const;
    virtual int portCount() const;
    virtual int outputPortCount(bool countBidir = false) const;
    virtual int inputPortCount(bool countBidir = false) const;
    virtual int bidirPortCount() const;

    virtual void setMachine(Machine& mach);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

    /// ObjectState name for Unit.
    static const std::string OSNAME_UNIT;

protected:
    Unit(const std::string& name);
    Unit(const ObjectState* state);

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

    void addPort(Port& port);
    void deleteAllPorts();
    void deleteOtherPorts(const NameSet& portsToLeave);
    static NameSet portNames(const ObjectState* state);
    void loadStateWithoutReferences(const ObjectState* state);

    /// Contains all the ports of the unit.
    PortTable ports_;

    // Port is a friend class to be able add and remove itself from the unit
    friend class Port;
};
}

#endif
