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
 * @file Netlist.hh
 *
 * Declaration of Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_HH
#define TTA_NETLIST_HH

#include <boost/graph/adjacency_list.hpp>

#include "PortConnectionProperty.hh"
#include "Exception.hh"

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
        std::string name;
        std::string type;
        std::string value;
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

    bool isEmpty() const;
    NetlistBlock& topLevelBlock() const
        throw (InstanceNotFound);

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
};
}

#endif
