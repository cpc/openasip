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
 * @file VHDLNetlistWriter.hh
 *
 * Declaration of VHDLNetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam.tut.fi)
 * @note rating: red
 */

#ifndef TTA_VHDL_NETLIST_WRITER_HH
#define TTA_VHDL_NETLIST_WRITER_HH

#include <map>
#include <string>
#include <boost/graph/graph_traits.hpp>

#include "NetlistWriter.hh"
#include "Netlist.hh"
#include "ProGeTypes.hh"

namespace ProGe {

class BaseNetlistBlock;

/**
 * Writes VHDL files which implement the given netlist block.
 */
class VHDLNetlistWriter : public NetlistWriter {
public:
    VHDLNetlistWriter(const BaseNetlistBlock& targetBlock);
    virtual ~VHDLNetlistWriter();

    virtual void write(const std::string& dstDirectory);

    static void writeGenericDeclaration(
        const BaseNetlistBlock& block,
        unsigned int indentationLevel,
        const std::string& indentation,
        std::ostream& stream);
    static void writePortDeclaration(
        const BaseNetlistBlock& block,
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
        const BaseNetlistBlock& block, const std::string& dstDirectory);
    void writeSignalDeclarations(
        const BaseNetlistBlock& block,
        std::ofstream& stream);
    void writeSignalAssignments(
        const BaseNetlistBlock& block,
        std::ofstream& stream) const;

    void writeConnection(
        const BaseNetlistBlock& block,
        std::ofstream& stream,
        edge_descriptor edgeDescriptor,
        NetlistPort* srcPort,
        NetlistPort* dstPort) const;

    void writeComponentDeclarations(
        const BaseNetlistBlock& block,
        std::ofstream& stream) const;
    void writePortMappings(
        const BaseNetlistBlock& block,
        std::ofstream& stream) const;
    std::string indentation(unsigned int level) const;

    TCEString genericMapStringValue(const TCEString& generic) const;

    static std::string directionString(Direction direction);
    static std::string generateIndentation(
        unsigned int level, const std::string& indentation);
    static bool isNumber(const std::string& formula);
    static bool usesParameterWidth(const NetlistPort& port);
    static std::string portSignalName(const NetlistPort& port);
    static std::string portSignalType(const NetlistPort& port);
    static TCEString signalRange(
        int high,
        int low,
        bool allowShort = false);
    static TCEString parameterWidthValue(const NetlistPort& port);
    static std::string signalAssignment(
        const NetlistPort& dst, const NetlistPort& src);
    
    /// Width of the ground signal.
    int groundWidth_;
    
};
}

#endif
