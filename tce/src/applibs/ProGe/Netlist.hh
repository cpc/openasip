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
 * @file Netlist.hh
 *
 * Declaration of Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_HH
#define TTA_NETLIST_HH

// these need to be included before Boost so we include a working
// and warning-free hash_map
#include "hash_set.hh"
#include "hash_map.hh"

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
#include <boost/graph/adjacency_list.hpp>
POP_CLANG_DIAGS

#include "PortConnectionProperty.hh"
#include "Exception.hh"
#include "TCEString.hh"

namespace ProGe {

class NetlistPort;
class NetlistBlock;

/**
 * Represents a netlist of port connections.
 */
class Netlist : public boost::adjacency_list<
    boost::vecS, boost::vecS, boost::bidirectionalS, NetlistPort*,
    PortConnectionProperty> {
public:
    /// Struct for parameter type.
    struct Parameter {
        TCEString name;
        TCEString type;
        TCEString value;
    };

    Netlist();
    virtual ~Netlist();

    void connectPorts(
        NetlistPort& port1,
        NetlistPort& port2,
        int port1FirstBit,
        int port2FirstBit,
        int width);
    void connectPorts(
        NetlistPort& port1,
        NetlistPort& port2);

    void connectPortsInverted(
        NetlistPort& port1,
        NetlistPort& port2);

    bool isPortConnected(const NetlistPort& port) const;

    bool isEmpty() const;
    NetlistBlock& topLevelBlock() const
        throw (InstanceNotFound);

    void setCoreEntityName(TCEString coreEntityName);
    TCEString coreEntityName() const;
    void mapDescriptor(const NetlistPort& port, size_t descriptor);
    size_t descriptor(const NetlistPort& port) const;

    void setParameter(
        const std::string& name,
        const std::string& type,
        const std::string& value);
    void removeParameter(const std::string& name);
    bool hasParameter(const std::string& name) const;
    int parameterCount() const;
    Parameter parameter(int index) const
        throw (OutOfRange);

private:
    /// Map type for vertex descriptors
    typedef std::map<const NetlistPort*, size_t> DescriptorMap;
    /// Vector type for parameters.
    typedef std::vector<Parameter> ParameterTable;

    /// Vertex descriptor map.
    DescriptorMap vertexDescriptorMap_;
    /// Parameters of the netlist.
    ParameterTable parameters_;

    /// This should be set to the entity name of the generate core,
    /// if it's not the same as the toplevel module name.
    TCEString coreEntityName_;

    static const std::string INVERTER_MODULE;
    static const std::string INVERTER_INPUT;
    static const std::string INVERTER_OUTPUT;
};
}

#endif
