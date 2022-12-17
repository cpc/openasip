/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file NetlistFactories.hh
 *
 * Declaration of NetlistFactories class.
 *
 * Collection of factories for creating netlist ports, port groups and blocks.
 *
 * Created on: 25.5.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef NETLISTFACTORIES_HH
#define NETLISTFACTORIES_HH

#include <map>
#include <utility>

#include "Machine.hh"
#include "MachineImplementation.hh"

#include "SignalTypes.hh"
#include "SignalGroupTypes.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"

namespace ProGe {

/*
 * Factory that creates prototype ports by given signal (group) types.
 */
class PortFactory {
public:
    PortFactory(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& impl);
    virtual ~PortFactory();

    NetlistPort* createPort(SignalType type, Direction direction = IN) const;
    NetlistPortGroup* createPortGroup(SignalGroupType type) const;

    static NetlistPort* create(SignalType type, Direction direction = IN);
    static NetlistPortGroup* create(SignalGroupType type);
    static void initializeContext(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& impl);
    static PortFactory* instance();

    static NetlistPort* clockPort(Direction direction = IN);
    static NetlistPort* resetPort(Direction direction = IN);

private:
    PortFactory();

    void registerPort(SignalType, const NetlistPort* port);
    void registerPort(const NetlistPort* port);
    void registerPorts();
    void registerPortGroup(
        SignalGroupType type,
        const NetlistPortGroup* portGroup);
    void registerPortGroup(const NetlistPortGroup* portGroup);
    void registerPortGroups();

    typedef std::map<SignalType, const NetlistPort*>
        PortPrototypeContainer;
    typedef std::map<SignalGroupType, const NetlistPortGroup*>
        PortGroupPrototypeContainer;

    /// The creation context.
    /*const TTAMachine::Machine* machine_;*/
    /// The creation context.
    /*const IDF::MachineImplementation* implementation_;*/
    /// Registered NetlistPort prototypes.
    PortPrototypeContainer portPrototypes_;
    /// Registered NetlistPortGroup prototypes.
    PortGroupPrototypeContainer portGroupPrototypes_;

    /// Singleton instance of the factory.
    static PortFactory* instance_;
    /// The creation context for singleton instance.
    static const TTAMachine::Machine* staticMachine_;
    /// The creation context for singleton instance.
    static const IDF::MachineImplementation* staticImplementation_;
};

} /* namespace ProGe */



#endif /* NETLISTFACTORIES_HH */
