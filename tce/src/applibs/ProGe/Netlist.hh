/**
 * @file Netlist.hh
 *
 * Declaration of Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
