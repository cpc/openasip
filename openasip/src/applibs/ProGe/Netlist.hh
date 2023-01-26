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
/**
 * @file Netlist.hh
 *
 * Declaration of Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
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
#include "Parameter.hh"

namespace ProGe {

class NetlistPort;
class NetlistPortGroup;
class NetlistBlock;
enum class SignalType;

/**
 * Represents a netlist of port connections.
 */
class Netlist : public boost::adjacency_list<
    boost::vecS, boost::vecS, boost::bidirectionalS, NetlistPort*,
    PortConnectionProperty> {

    /// Map type for vertex descriptors
    typedef std::map<const NetlistPort*, size_t> DescriptorMap;

public:

    Netlist();
    virtual ~Netlist();

    bool connect(
        const NetlistPort& port1,
        const NetlistPort& port2,
        int port1FirstBit,
        int port2FirstBit,
        int width = 1);
    bool connect(
        const NetlistPort& port1,
        const NetlistPort& port2);
    bool connect(
        const NetlistPortGroup& group1,
        const NetlistPortGroup& group2);
    bool connectBy(
        SignalType byType,
        const NetlistPortGroup& group1,
        const NetlistPortGroup& group2);
    bool connect(
        const NetlistPortGroup& group1,
        const NetlistPortGroup& group2,
        std::map<SignalType, SignalType> connectionMap);
    bool connectGroupByName(
        const NetlistPortGroup& group1,
        const NetlistPortGroup& group2);

    void disconnectPorts(
        const NetlistPort& port1,
        const NetlistPort& port2);

    bool isPortConnected(const NetlistPort& port) const;

    bool isEmpty() const;
    bool hasConnections() const;

    size_t registerPort(NetlistPort& port);
    size_t descriptor(const NetlistPort& port) const;
    bool isRegistered(const NetlistPort& port) const;
    void unregisterPort(NetlistPort& port);

    //todo overload boost's operator[]  with arg (const NetlistPort& port)

    void setParameter(
        const std::string& name,
        const std::string& type,
        const std::string& value);
    void setParameter(const Parameter& param);
    void removeParameter(const std::string& name);
    bool hasParameter(const std::string& name) const;
    size_t parameterCount() const;
    Parameter parameter(size_t index) const;

    typedef boost::graph_traits<Netlist>::edge_iterator iterator;
    typedef boost::graph_traits<const Netlist>::edge_iterator const_iterator;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    typedef DescriptorMap::iterator descriptor_iterator;
    typedef DescriptorMap::const_iterator const_descriptor_iterator;
    descriptor_iterator descriptorBegin();
    descriptor_iterator descriptorEnd();
    const_descriptor_iterator descriptorBegin() const;
    const_descriptor_iterator descriptorEnd() const;

    static void connectClocks(NetlistBlock& block);
    static void connectResets(NetlistBlock& block);
private:
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
