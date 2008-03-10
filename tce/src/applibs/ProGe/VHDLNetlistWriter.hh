/**
 * @file VHDLNetlistWriter.hh
 *
 * Declaration of VHDLNetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_VHDL_NETLIST_WRITER_HH
#define TTA_VHDL_NETLIST_WRITER_HH

#include <map>
#include <string>
#include <boost/graph/graph_traits.hpp>

#include "NetlistWriter.hh"
#include "Netlist.hh"
#include "HDBTypes.hh"

namespace ProGe {

class NetlistBlock;

/**
 * Writes VHDL files which implement the given netlist.
 */
class VHDLNetlistWriter : public NetlistWriter {
public:
    VHDLNetlistWriter(const Netlist& netlist);
    virtual ~VHDLNetlistWriter();

    virtual void write(const std::string& dstDirectory)
        throw (IOException, InvalidData);

    static void writeGenericDeclaration(
        const NetlistBlock& block,
        unsigned int indentationLevel,
        const std::string& indentation,
        std::ostream& stream);
    static void writePortDeclaration(
        const NetlistBlock& block,
        unsigned int indentationLevel,
        const std::string& indentation,
        std::ostream& stream);

private:
    typedef boost::graph_traits<Netlist>::vertex_descriptor
    vertex_descriptor;
    typedef boost::graph_traits<Netlist>::edge_descriptor
    edge_descriptor;
    typedef boost::graph_traits<Netlist>::out_edge_iterator
    out_edge_iterator;

    void writeNetlistParameterPackage(const std::string& dstDirectory) const;
    std::string netlistParameterPkgName() const;
    void writeBlock(
        const NetlistBlock& block,
        const std::string& dstDirectory)
        throw (IOException);
    void writeSignalDeclarations(
        const NetlistBlock& block,
        std::ofstream& stream);
    void writeSignalAssignments(
        const NetlistBlock& block,
        std::ofstream& stream) const;
                                
    void writeComponentDeclarations(
        const NetlistBlock& block,
        std::ofstream& stream) const;
    void writePortMappings(
        const NetlistBlock& block,
        std::ofstream& stream) const;
    std::string indentation(unsigned int level) const;
    static std::string directionString(HDB::Direction direction);
    static std::string generateIndentation(
        unsigned int level, const std::string& indentation);
    static bool isNumber(const std::string& formula);
    static std::string portSignalName(const NetlistPort& port);
    static std::string portSignalType(const NetlistPort& port);
    

    /// Width of the ground signal.
    int groundWidth_;
    
};
}

#endif
