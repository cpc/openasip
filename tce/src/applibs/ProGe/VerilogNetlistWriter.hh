/*
    Copyright (c) 2012 Vinogradov Viacheslav.

    This file is going to be a part of TTA-Based Codesign Environment (TCE).

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
 * @file VerilogNetlistWriter.hh
 *
 * Declaration of VerilogNetlistWriter class based on VHDLnetlistWriter
 *
 * @author Vinogradov Viacheslav 2012 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_VERILOG_NETLIST_WRITER_HH
#define TTA_VERILOG_NETLIST_WRITER_HH

#include <map>
#include <string>
#include <boost/graph/graph_traits.hpp>

#include "NetlistWriter.hh"
#include "Netlist.hh"
#include "HDBTypes.hh"

namespace ProGe {

class NetlistBlock;

/**
 * Writes Verilog files which implement the given netlist.
 */
class VerilogNetlistWriter : public NetlistWriter {
public:
    VerilogNetlistWriter(const Netlist& netlist);
    virtual ~VerilogNetlistWriter();

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

    /**
     * Tries to determine whether the string generic needs quot marks for
     * generic mapping
     *
     * If string literal contains '.',  or "__" it cannot be a valid
     * Verilog label (i.e. another generic), thus it needs quotation marks.
     *
     * @param generic String generic value
     * @return Generic mapping string
     */
    TCEString genericMapStringValue(const TCEString& generic) const;

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
