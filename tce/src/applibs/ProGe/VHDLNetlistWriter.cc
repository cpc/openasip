/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file VHDLNetlistWriter.cc
 *
 * Implementation of VHDLNetlistWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cctype>
#include <algorithm>

#include "VHDLNetlistWriter.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"

#include "FileSystem.hh"
#include "Conversion.hh"
#include "ContainerTools.hh"
#include "AssocTools.hh"
#include "StringTools.hh"

using std::string;
using std::endl;
using std::ofstream;
using std::set;
using std::pair;

const std::string GROUND_SIGNAL = "ground_signal";
const std::string PARAM_STRING = "string";

namespace ProGe {

/**
 * Constructor. Records the input netlist for which it can generate VHDL.
 *
 * @param netlist The input netlist.
 */
VHDLNetlistWriter::VHDLNetlistWriter(const Netlist& netlist) :
    NetlistWriter(netlist), groundWidth_(0) {
}


/**
 * The destructor.
 */
VHDLNetlistWriter::~VHDLNetlistWriter() {
}


/**
 * Generates the VHDL files and writes them to the given directory.
 *
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 * @exception InvalidData If the netlist is invalid.
 */
void
VHDLNetlistWriter::write(const std::string& dstDirectory)
    throw (IOException, InvalidData) {

    const Netlist& netlist = this->netlist();
    if (netlist.isEmpty()) {
        string errorMsg = "Empty input netlist.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    writeNetlistParameterPackage(dstDirectory);
    NetlistBlock& topLevelBlock = netlist.topLevelBlock();
    writeBlock(topLevelBlock, dstDirectory);
}


/**
 * Writes the package that defines parameters of the netlist.
 *
 * @param dstDirectory The destination directory.
 */
void
VHDLNetlistWriter::writeNetlistParameterPackage(
    const std::string& dstDirectory) const {

    string fileName = dstDirectory + FileSystem::DIRECTORY_SEPARATOR + 
        netlistParameterPkgName() + "_pkg.vhdl";
    ofstream outFile;
    outFile.open(fileName.c_str(), ofstream::out);

    outFile << "package " << netlistParameterPkgName() << " is" << endl;
    for (int i = 0; i < netlist().parameterCount(); i++) {
        Netlist::Parameter param = netlist().parameter(i);
        outFile << indentation(1) << "constant " << param.name << " : "
                << param.type << " := " << param.value << ";" << endl;
    }
    outFile << "end " << netlistParameterPkgName() << ";" << endl;
}
    

/**
 * Returns the name of the netlist parameter package.
 *
 * @return The name.
 */
std::string
VHDLNetlistWriter::netlistParameterPkgName() const {
//    return netlist().coreEntityName() + "_params";
    return netlist().topLevelBlock().moduleName() + "_params";
}


/**
 * Writes the given block of the netlist to the given destination directory.
 *
 * @param block The netlist block.
 * @param dstDirectory The destination directory.
 * @exception IOException If the file cannot be created.
 */
void
VHDLNetlistWriter::writeBlock(
    const NetlistBlock& block,
    const std::string& dstDirectory)
    throw (IOException) {

    string fileName = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        block.moduleName() + ".vhdl";
    if (!FileSystem::fileIsCreatable(fileName) && 
        !(FileSystem::fileExists(fileName) && 
          FileSystem::fileIsWritable(fileName))) {

        string errorMsg = "Unable to create file: " + fileName;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    const string entityName = block.moduleName();

    ofstream outFile;
    outFile.open(fileName.c_str(), ofstream::out);

    outFile << "library IEEE;" << endl;
    outFile << "use IEEE.std_logic_1164.all;" << endl;
    outFile << "use IEEE.std_logic_arith.all;" << endl;
    outFile << "use work.tce_util.all;" << endl;
    outFile << "use work." << netlist().coreEntityName()
            << "_globals.all;" << endl;
    outFile << "use work." << netlist().coreEntityName()
            << "_imem_mau.all;" << endl;

    for (size_t i = 0; i < block.packageCount(); i++) {
        outFile << "use work." << block.package(i) << ".all;" << endl;
    }


    if (netlist().parameterCount() > 0) {
        outFile << "use work." << netlistParameterPkgName() << ".all;"
                << endl;
    }

    outFile << endl;

    // create entity
    outFile << "entity " + entityName + " is" << endl;

    // create generics
    writeGenericDeclaration(block, 1, indentation(1), outFile);

    // create port declarations
    outFile << endl;
    writePortDeclaration(block, 1, indentation(1), outFile);

    outFile << endl << "end " << entityName << ";" << endl;

    // create architecture
    outFile << endl;
    string architectureName = "structural";
    outFile << "architecture " << architectureName << " of "
            << entityName << " is" << endl << endl;

    writeSignalDeclarations(block, outFile);
    outFile << endl;
    writeComponentDeclarations(block, outFile);
    outFile << endl;
    outFile << "begin" << endl << endl;
    writeSignalAssignments(block, outFile);
    outFile << endl;
    writePortMappings(block, outFile);
    outFile << "end " + architectureName + ";" << endl;
    outFile.close();
}


/**
 * Writes the generic declarations of the given netlist block.
 *
 * @param block The netlist block.
 * @param indentationLevel The indentation level where the generic declaration
 *                         is written.
 * @param indentation The string used as indentation (one level).
 * @param stream The stream to write.
 */
void
VHDLNetlistWriter::writeGenericDeclaration(
    const NetlistBlock& block,
    unsigned int indentationLevel,
    const std::string& indentation,
    std::ostream& stream) {

    if (block.parameterCount() > 0) {
        stream << endl;
        stream << generateIndentation(indentationLevel, indentation)
               << "generic (" << endl;
        for (int i = 0; i < block.parameterCount(); i++) {
            Netlist::Parameter param = block.parameter(i);
            stream << generateIndentation(indentationLevel+1, indentation)
                   << param.name << " : " << param.type;
            if (param.value != "") {
                stream << " := ";
                if (param.type.lower() == PARAM_STRING) {
                    // string literal needs quot. marks
                    if (!param.value.startsWith("\""))
                        stream << "\"";
                    stream << param.value;
                    if (!param.value.endsWith("\""))
                        stream << "\"";
                } else {
                    stream << param.value;
                }
            }
            if (i + 1 == block.parameterCount()) {
                stream << ");";
            } else {
                stream << ";";
            }
            stream << endl;
        }
    }
}


/**
 * Writes the port declaration of the given netlist block.
 *
 * @param block The netlist block.
 * @param indentationLevel The indentation level where the generic declaration
 *                         is written.
 * @param indentation The string used as indentation (one level).
 * @param stream The stream to write.
 */
void
VHDLNetlistWriter::writePortDeclaration(
    const NetlistBlock& block,
    unsigned int indentationLevel,
    const std::string& indentation,
    std::ostream& stream) {

    stream << generateIndentation(indentationLevel, indentation) << "port ("
           << endl;

    for (int i = 0; i < block.portCount(); i++) {
        NetlistPort& port = block.port(i);
        string portName = port.name();
        string direction = directionString(port.direction());
        stream << generateIndentation(indentationLevel+1, indentation)
               << portName << " : " << direction << " ";
        if (port.dataType() == BIT) {
            stream << "std_logic";
        } else {
            stream << "std_logic_vector(";
            // zero width ports as (0 downto 0)
            if (isNumber(port.widthFormula()) && 
                    Conversion::toInt(port.widthFormula()) == 0) {
                stream << "0";
            } else if (isNumber(port.widthFormula())) {
                stream << Conversion::toInt(port.widthFormula()) - 1;
            } else {
                stream << port.widthFormula() << "-1";
            }
            stream << " downto 0)";
        }
        if (i + 1 == block.portCount()) {
            stream << ");";
        } else {
            stream << ";";
        }
        stream << endl;
    }
}


/**
 * Writes the VHDL signal declarations to the given stream.
 *
 * @param block The block of which the signals are written.
 * @param stream The stream to write.
 */
void
VHDLNetlistWriter::writeSignalDeclarations(
    const NetlistBlock& block,
    std::ofstream& stream) {

    // collect all the sub blocks to a set, lexicographical sort.
    typedef std::set<NetlistBlock*, NetlistBlockNameComparator> BlockSet;
    BlockSet subBlocks;
    for (int i = 0; i < block.subBlockCount(); i++) {
        // ports belonging to virtual blocks have static values, thus they are
        // excluded
        if (!block.subBlock(i).isVirtual()) {
            subBlocks.insert(&block.subBlock(i));
        }
    }

    // create a signal for each port in the sub-blocks
    for (BlockSet::const_iterator iter = subBlocks.begin();
         iter != subBlocks.end(); iter++) {
        NetlistBlock* subBlock = *iter;
        
        for (int i = 0; i < subBlock->portCount(); i++) {
            NetlistPort& port = subBlock->port(i);

            size_t vertexDescriptor = netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, netlist());
            
            if (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                vertex_descriptor dstVertex = boost::target(
                    edgeDescriptor, netlist());
                NetlistPort* dstPort = netlist()[dstVertex];
                if (dstPort->parentBlock() != &block ||
                    boost::out_degree(vertexDescriptor, block.netlist()) > 1) {
                    stream << indentation(1) << "signal "
                           << portSignalName(port) << " : "
                           << portSignalType(port) << ";" << endl;
                }
            } else {
                // assume the port is connected to ground if is is
                // unconnected in the netlist
                if (port.realWidthAvailable()) {
                    groundWidth_ = 
                        std::max(port.realWidth(), groundWidth_);
                }
                stream << indentation(1) << "signal "
                       << portSignalName(port) << " : "
                       << portSignalType(port) << ";" << endl;
            }
        }
    }
    
    // create a ground signal
    if (groundWidth_ > 0) {
        stream << indentation(1) << "signal " << GROUND_SIGNAL
               << " : std_logic_vector(" << groundWidth_ - 1
               << " downto 0);" << endl;
    }
}


/**
 * Writes the signal assignments of the given block to the given stream.
 *
 * @param block The netlist block.
 * @param stream The stream.
 */
void
VHDLNetlistWriter::writeSignalAssignments(
    const NetlistBlock& block,
    std::ofstream& stream) const {

    set<NetlistBlock*, NetlistBlockNameComparator> subBlocks;
    for (int i = 0; i < block.subBlockCount(); i++) {
        subBlocks.insert(&block.subBlock(i));
    }

    typedef std::vector<edge_descriptor> EdgeTable;
    EdgeTable handledEdges;
    
    for (int i = 0; i < block.subBlockCount(); i++) {
        NetlistBlock& subBlock = block.subBlock(i);
        for (int i = 0; i < subBlock.portCount(); i++) {
            NetlistPort& port = subBlock.port(i);
            size_t vertexDescriptor = netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, netlist());

            while (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                edges.first++;
                if (!ContainerTools::containsValue(
                        handledEdges, edgeDescriptor)) {
                    vertex_descriptor srcVertex = boost::source(
                        edgeDescriptor, netlist());
                    vertex_descriptor dstVertex = boost::target(
                        edgeDescriptor, netlist());
                    NetlistPort* srcPort = netlist()[srcVertex];
                    NetlistPort* dstPort = netlist()[dstVertex];
                    
                    if (dstPort->parentBlock() == &block) {
                        if (dstPort->direction() == HDB::OUT &&
                            srcPort->dataType() == dstPort->dataType() && 
                            boost::out_degree(vertexDescriptor,
                                block.netlist()) > 1) {
                            // Handle the rare case of multiple outputs
                            // through a wire signal. This isn't done normally
                            // because there would be an ugly wire signal for
                            // every clk, rstx, etc.
                            stream << indentation(1)
                                   << dstPort->name()
                                   << " <= "
                                   << portSignalName(*srcPort) << ";"
                                   << endl;
                        }
                        continue;
                    }

                    assert(srcPort == &port);
                    if (AssocTools::containsKey(
                            subBlocks, srcPort->parentBlock()) &&
                        AssocTools::containsKey(
                            subBlocks, dstPort->parentBlock())) {
                        
                         handledEdges.push_back(edgeDescriptor);
                         // add the opposite edge too
                         std::pair<edge_descriptor, bool> opposite =
                             boost::edge(dstVertex, srcVertex, netlist());
                         assert(opposite.second);
                         assert(opposite.first != edgeDescriptor);
                         handledEdges.push_back(opposite.first);
                         
                         PortConnectionProperty property =
                             netlist()[edgeDescriptor];
                         if (property.fullyConnected()) {
                             if (srcPort->direction() == HDB::OUT) {
                                 stream << indentation(1)
                                        << portSignalName(*dstPort) 
                                        << " <= " 
                                        << portSignalName(*srcPort) << ";"
                                        << endl;
                             } else {
                                 stream << indentation(1)
                                        << portSignalName(*srcPort)
                                        << " <= "
                                        << portSignalName(*dstPort) << ";"
                                        << endl;
                             }
                         } else {
                             string srcPortSignal;
                             if (srcPort->dataType() == BIT) {
                                 srcPortSignal = portSignalName(*srcPort);
                             } else {
                                 if (dstPort->dataType() == BIT) {
                                     srcPortSignal = 
                                         portSignalName(*srcPort) + "(" + 
                                         Conversion::toString(
                                             property.port1FirstBit()) + ")";
                                 } else {
                                     srcPortSignal = 
                                         portSignalName(*srcPort) + "(" + 
                                         Conversion::toString(
                                             property.port1FirstBit() + 
                                             property.width() - 1) + 
                                         " downto " + 
                                         Conversion::toString(
                                             property.port1FirstBit()) + ")";
                                 }
                             }
                             string dstPortSignal;
                             if (dstPort->dataType() == BIT) {
                                 dstPortSignal = portSignalName(*dstPort);
                             } else {
                                 if (srcPort->dataType() == BIT) {
                                     dstPortSignal = 
                                         portSignalName(*dstPort) + "(" + 
                                         Conversion::toString(
                                             property.port2FirstBit()) + ")";
                                 } else {
                                     dstPortSignal = 
                                         portSignalName(*dstPort) + "(" + 
                                         Conversion::toString(
                                             property.port2FirstBit() + 
                                             property.width() - 1) + 
                                         " downto " + 
                                         Conversion::toString(
                                             property.port2FirstBit()) + ")";
                                 }
                             }
                                 
                             if (srcPort->direction() == HDB::OUT) {
                                 stream << indentation(1) << dstPortSignal
                                        << " <= " << srcPortSignal << ";"
                                        << endl;
                             } else {
                                 stream << indentation(1) << srcPortSignal
                                        << " <= " << dstPortSignal << ";"
                                        << endl;
                             }
                         }
                    }
                }
            }
        }
    }

    if (groundWidth_ > 0) {
        stream << indentation(1) << GROUND_SIGNAL << " <= (others => '0');"
               << endl;
    }
}
            

/**
 * Writes the component declarations of the given netlist block to the
 * given stream.
 *
 * @param block The netlist block.
 * @param stream The stream to write.
 */
void
VHDLNetlistWriter::writeComponentDeclarations(
    const NetlistBlock& block,
    std::ofstream& stream) const {

    std::set<string> declaredModules;
    for (int i = 0; i < block.subBlockCount(); i++) {
        NetlistBlock& component = block.subBlock(i);
        if (AssocTools::containsKey(declaredModules, component.moduleName())) {
            continue;
        }
        // virtual NetlistBlocks are omitted
        if (component.isVirtual()) {
            continue;
        }

        declaredModules.insert(component.moduleName());
        stream << indentation(1) << "component " << component.moduleName()
               << endl;
        if (component.parameterCount() > 0) {
            stream << indentation(2) << "generic (" << endl;
            for (int i = 0; i < component.parameterCount(); i++) {
                Netlist::Parameter param = component.parameter(i);
                stream << indentation(3) << param.name << " : "
                       << param.type;
                if (i + 1 == component.parameterCount()) {
                    stream << ");";
                } else {
                    stream << ";";
                }
                stream << endl;
            }
        }
        stream << indentation(2) << "port (" << endl;
        for (int i = 0; i < component.portCount(); i++) {
            NetlistPort& port = component.port(i);
            stream << indentation(3) << port.name() << " : "
                   << directionString(port.direction()) << " ";
            if (port.dataType() == BIT) {
                stream << "std_logic";
            } else { 
                stream << "std_logic_vector(";
                stream << port.widthFormula();
                if ((isNumber(port.widthFormula()) &&
                    (Conversion::toInt(port.widthFormula()) != 0)) ||
                        !isNumber(port.widthFormula())) {
                    stream << "-1";
                } 
                stream << " downto 0)";
            }
            if (i + 1 == component.portCount()) {
                stream << ");";
            } else {
                stream << ";";
            }
            stream << endl;
        }
        stream << indentation(1) << "end component;" << endl << endl;
    }
}


/**
 * Writes the port mappings of the given block to the given stream.
 *
 * @param block The netlist block.
 * @param stream The stream to write.
 */
void
VHDLNetlistWriter::writePortMappings(
    const NetlistBlock& block,
    std::ofstream& stream) const {

    for (int i = 0; i < block.subBlockCount(); i++) {
        NetlistBlock& component = block.subBlock(i);

        // virtual NetlistBlocks are omitted
        if (component.isVirtual()) {
            continue;
        }

        stream << indentation(1) << component.instanceName() << " : "
               << component.moduleName() << endl;

        // create generic map
        if (component.parameterCount() > 0) {
            stream << indentation(2) << "generic map (" << endl;
            for (int i = 0; i < component.parameterCount(); i++) {
                Netlist::Parameter param = component.parameter(i);
                stream << indentation(3) << param.name << " => ";
                if (param.type.lower() == PARAM_STRING) {
                    stream << genericMapStringValue(param.value);
                } else {
                    stream << param.value;
                }
                if (i == component.parameterCount() - 1) {
                    stream << ")" << endl;
                } else {
                    stream << "," << endl;
                }
            }
        }

        // create port map
        stream << indentation(2) << "port map (" << endl;
        for (int i = 0; i < component.portCount(); i++) {
            NetlistPort& port = component.port(i);
            size_t vertexDescriptor = netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, netlist());

            string srcConn = port.name();
            string dstConn = "";
            if (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                vertex_descriptor dstVertex = boost::target(
                    edgeDescriptor, netlist());
                NetlistPort* dstPort = netlist()[dstVertex];
                
                if (dstPort->parentBlock() == &block) {
                    if (port.dataType() != dstPort->dataType()) {
                        if (port.dataType() == BIT) {
                            assert(dstPort->dataType() == BIT_VECTOR);
                            dstConn = dstPort->name() + "(0)";
                        } else {
                            assert(dstPort->dataType() == BIT);
                            srcConn += "(0)";
                            dstConn = dstPort->name();
                        }
                    } else {
                        dstConn = dstPort->name();
                    }
                } else {
                    dstConn = portSignalName(port);
                }
            } else {
                dstConn = portSignalName(port);
            }
            stream << indentation(3) << srcConn << " => " << dstConn;
            if (i+1 < component.portCount()) {
                stream << "," << endl;
            }
        }
        stream << ");" << endl << endl;
    }
}


/**
 * Returns the string that means the same direction as the given one in VHDL.
 *
 * @return The direction string.
 */
std::string
VHDLNetlistWriter::directionString(HDB::Direction direction) {
    switch (direction) {
    case HDB::IN: return "in";
    case HDB::OUT: return "out";
    case HDB::BIDIR: return "inout";
    default: assert(false);
    }

    // dummy return
    assert(false);
    return "";
}


/**
 * Tells whether the given string is a non-negative integer number.
 *
 * @param formula The string.
 * @return True if the given string is a non-negative integer number.
 */
bool
VHDLNetlistWriter::isNumber(const std::string& formula) {
    int length = formula.length();
    for (int i = 0; i < length; i++) {
        if (!isdigit(formula[i])) {
            return false;
        }
    }

    return true;
}


/**
 * Returns a string which makes indetation of the given level.
 *
 * @param level The indentation level.
 */
std::string
VHDLNetlistWriter::indentation(unsigned int level) const {
    return StringTools::indent(level);
}

/**
 * Generates an indentation string with the given parameters.
 *
 * @param indentationLevel The level of indentation.
 * @param indentation The string used as indentation (one level).
 * @return The indentation of the given level.
 */
std::string
VHDLNetlistWriter::generateIndentation(
    unsigned int indentationLevel,
    const std::string& indentation) {

    string generatedInd("");
    for (unsigned int i = 0; i < indentationLevel; i++) {
        generatedInd += indentation;
    }
    return generatedInd;
}


/**
 * Returns the name of the signal mapped to the given port.
 *
 * @param port The port.
 */
std::string
VHDLNetlistWriter::portSignalName(const NetlistPort& port) {

    NetlistBlock* parentBlock = port.parentBlock();
    string signalName = "";
    if (port.hasStaticValue()) {
        string bit = "";
        if (port.staticValue() == ProGe::VCC) {
            bit = "1";
        } else {
            bit = "0";
        }
        if (port.dataType() == BIT) {
            signalName = "'" + bit + "'";
        } else {
            signalName = "(others => '" + bit + "')";
        }
    } else {
        signalName = parentBlock->instanceName() + "_" + port.name() +
            "_wire";
    }
    return signalName;
}


/**
 * Returns the type of the signal mapped to the given port.
 *
 * @param port The port.
 */
std::string
VHDLNetlistWriter::portSignalType(const NetlistPort& port) {
    if (port.dataType() == BIT) {
        return "std_logic";
    } else {
        if (port.realWidthAvailable()) {
            int width = port.realWidth();
            return "std_logic_vector(" + Conversion::toString(width?width-1:0) +
                " downto 0)";
        } else if (isNumber(port.widthFormula()) && 
                   (Conversion::toInt(port.widthFormula()) == 0)) {
            return "std_logic_vector(0 downto 0)";
        } else {
            return "std_logic_vector(" + port.widthFormula() +
                "-1 downto 0)";
        }
    }
}

TCEString
VHDLNetlistWriter::genericMapStringValue(const TCEString& generic) const {

    if (generic.startsWith("\"") && generic.endsWith("\"")) {
        return generic;
    }
    std::vector<TCEString> unallowed;
    unallowed.push_back(".");
    unallowed.push_back("__");
    for (unsigned int i = 0; i < unallowed.size(); i++) {
        if (generic.find(unallowed.at(i)) != TCEString::npos) {
            TCEString quoted;
            quoted << "\"" << generic << "\"";
            return quoted;
        }
    }
    return generic;
}
}
