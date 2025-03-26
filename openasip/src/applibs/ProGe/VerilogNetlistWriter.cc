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
 * @file VerilogNetlistWriter.cc
 *
 * Implementation of VerilogNetlistWriter class based on VHDLNetlistWriter.cc
 *
 * @author Vinogradov Viacheslav 2012
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cctype>
#include <algorithm>

#include "VerilogNetlistWriter.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "Parameter.hh"

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
const std::string PARAM_BOOLEAN = "boolean";

namespace ProGe {

/**
 * Constructor. Records the input netlist for which it can generate Verilog.
 *
 * @param netlist The input netlist.
 */
VerilogNetlistWriter::VerilogNetlistWriter(
    const BaseNetlistBlock& targetBlock)
    : NetlistWriter(targetBlock), groundWidth_(0) {}

/**
 * The destructor.
 */
VerilogNetlistWriter::~VerilogNetlistWriter() {
}

/**
 * Generates the Verilog files and writes them to the given directory.
 *
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 * @exception InvalidData If the netlist is invalid.
 */
void
VerilogNetlistWriter::write(const std::string& dstDirectory) {
    const BaseNetlistBlock& block = this->targetNetlistBlock();
    if (block.netlist().isEmpty()) {
        string errorMsg = "Empty input netlist block.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    writeNetlistParameterPackage(dstDirectory);
    writeBlock(block, dstDirectory);
}

/**
 * Writes the package(include files for verilog) that defines parameters of
 * the netlist.
 *
 * @param dstDirectory The destination directory.
 */
void
VerilogNetlistWriter::writeNetlistParameterPackage(
    const std::string& dstDirectory) const {
    string fileName = dstDirectory + FileSystem::DIRECTORY_SEPARATOR + 
        netlistParameterPkgName() + "_pkg.vh";
    ofstream outFile;
    outFile.open(fileName.c_str(), ofstream::out);
    if (targetNetlistBlock().netlist().parameterCount() == 0) {
        outFile << "parameter " << targetNetlistBlock().moduleName() 
                << "_DUMMY " << " = 0" << endl;
    } else {
        for (size_t i = 0; i < targetNetlistBlock().netlist().parameterCount();
             i++) {
            Parameter param = targetNetlistBlock().netlist().parameter(i);
            outFile << "parameter " << param.name() << " = " << param.value();
            if (i != targetNetlistBlock().netlist().parameterCount() - 1)
                outFile << ",";
            outFile << endl;
         }
    }
}

/**
 * Returns the name of the netlist parameter package(include file).
 *
 * @return The name.
 */
std::string
VerilogNetlistWriter::netlistParameterPkgName() const {
    return targetNetlistBlock().moduleName() + "_params";
}


/**
 * Writes the given block of the netlist to the given destination directory.
 *
 * @param block The netlist block.
 * @param dstDirectory The destination directory.
 * @exception IOException If the file cannot be created.
 */
void
VerilogNetlistWriter::writeBlock(
    const BaseNetlistBlock& block, const std::string& dstDirectory) {
    string fileName = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        block.moduleName() + ".v";
    if (!FileSystem::fileIsCreatable(fileName) && 
        !(FileSystem::fileExists(fileName) && 
          FileSystem::fileIsWritable(fileName))) {

        string errorMsg = "Unable to create file: " + fileName;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    const string entityName = block.moduleName();

    ofstream outFile;
    outFile.open(fileName.c_str(), ofstream::out);

    // create module
    outFile << "module " + entityName << endl;

    //create include
    string separator;
    outFile << "#(" << endl;
    if (block.netlist().parameterCount() > 0) {
        outFile << "`include \"" << netlistParameterPkgName() << "_pkg.vh\""
                << endl;
        separator = ",";
    }

    for (size_t i = 0; i < block.packageCount(); i++) {
        outFile << separator << endl;
        outFile << "`include \"" << block.package(i) << "_pkg.vh\"" << endl;
        separator = ",";
    }

    // create generics
    writeGenericDeclaration(block, 1, indentation(1), outFile);

    outFile << ")" << endl;

    // create port declarations
    writePortDeclaration(block, 1, indentation(1), outFile);
    outFile << endl;
    // create architecture
    writeSignalDeclarations(block, outFile);
    outFile << endl;
    writeSignalAssignments(block, outFile);
    outFile << endl;
    writePortMappings(block, outFile);
    outFile << "endmodule" << endl;
    outFile << endl;
    outFile.close();
}

/**
 * Writes the generic(parameter) declarations of the given netlist block.
 *
 * @param block The netlist block.
 * @param indentationLevel The indentation level where the generic declaration
 *                         is written.
 * @param indentation The string used as indentation (one level).
 * @param stream The stream to write.
 */
void
VerilogNetlistWriter::writeGenericDeclaration(
    const BaseNetlistBlock& block, unsigned int indentationLevel,
    const std::string& indentation, std::ostream& stream) {
    if (block.parameterCount() > 0) {
        if (block.netlist().parameterCount() > 0 || block.packageCount() != 0) {
            stream << ",";
        }
        stream << endl;
        for (size_t i = 0; i < block.parameterCount(); i++) {
            stream << generateIndentation(indentationLevel, indentation)
                   << "parameter ";
            Parameter param = block.parameter(i);
            stream << generateIndentation(indentationLevel + 1, indentation)
                   << param.name();
            if (param.defaultValue() != "") {
                stream << " = ";
                if (param.type().lower() == PARAM_BOOLEAN) {
                    if (param.defaultValue() == "false") {
                        stream << "0";
                    } else if (param.defaultValue() == "true") {
                        stream << "1";
                    } else {
                        string errorMsg = "VerilogNetlistWriter: invalid value for boolean parameter";
                        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
                    }
                } else if (param.type().lower() == PARAM_STRING) {
                    // string literal needs quot. marks
                    if (!param.defaultValue().startsWith("\""))
                        stream << "\"";
                    stream << param.defaultValue();
                    if (!param.defaultValue().endsWith("\"")) stream << "\"";
                } else {
                    stream << param.defaultValue();
                }
            }
            if (i != block.parameterCount()-1) {
                stream << "," << endl;
            }
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
VerilogNetlistWriter::writePortDeclaration(
    const BaseNetlistBlock& block, unsigned int indentationLevel,
    const std::string& indentation, std::ostream& stream) {
    stream << generateIndentation(indentationLevel, indentation) << "("
           << endl;

    for (size_t i = 0; i < block.portCount(); i++) {
        const NetlistPort& port = block.port(i);
        string portName = port.name();
        string direction = directionString(port.direction());
        stream << generateIndentation(indentationLevel+1, indentation)
               << direction;

        if (port.dataType() == BIT) {
           //nothing to do
        } else {
            stream << "[";
            // zero width ports as (0: 0 
            if (isNumber(port.widthFormula()) && 
                    Conversion::toInt(port.widthFormula()) == 0) {
                stream << "0";
            } else if (isNumber(port.widthFormula())) {
                stream << Conversion::toInt(port.widthFormula()) - 1;
            } else {
                stream << port.widthFormula() << "-1";
            }
            stream << ":0]";
        }
        stream << " " << portName;
        if (i + 1 == block.portCount()) {
            stream << ");";
        } else {
            stream << ",";
        }
        stream << endl;
    }
}

/**
 * Writes the Verilog signal declarations to the given stream.
 *
 * @param block The block of which the signals are written.
 * @param stream The stream to write.
 */
void
VerilogNetlistWriter::writeSignalDeclarations(
    const BaseNetlistBlock& block, std::ofstream& stream) {
    // collect all the sub blocks to a set
    typedef std::set<const BaseNetlistBlock*, NetlistBlockNameComparator>
        BlockSet;
    BlockSet subBlocks;
    for (size_t i = 0; i < block.subBlockCount(); i++) {
        // ports belonging to virtual blocks have static values, thus they are
        // excluded
        if (!block.subBlock(i).isVirtual()) {
            subBlocks.insert(&block.subBlock(i));
        }
    }

    // create a signal for each port in the sub-blocks
    for (BlockSet::const_iterator iter = subBlocks.begin();
         iter != subBlocks.end(); iter++) {
        const BaseNetlistBlock* subBlock = *iter;

        for (size_t i = 0; i < subBlock->portCount(); i++) {
            const NetlistPort& port = subBlock->port(i);

            size_t vertexDescriptor = block.netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, block.netlist());

            if (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                vertex_descriptor dstVertex =
                    boost::target(edgeDescriptor, block.netlist());
                NetlistPort* dstPort = block.netlist()[dstVertex];
                if (&dstPort->parentBlock() != &block) {
                    stream << indentation(1) << "wire"
                           << portSignalType(port) << " "
                           << portSignalName(port) << ";" << endl;
                }
            } else if (!port.hasStaticValue()) {
                // assume the port is connected to ground if is is
                // unconnected in the netlist
                if (port.realWidthAvailable()) {
                    groundWidth_ = 
                        std::max(port.realWidth(), groundWidth_);
                }
                stream << indentation(1) << "wire"
                       << portSignalType(port) << " "
                       << portSignalName(port) << ";" << endl;
            }
        }
    }
    
    // create a ground signal
    if (groundWidth_ > 0) {
        stream << indentation(1) << "wire[" << groundWidth_ - 1 << ":0]"
               << GROUND_SIGNAL << ";" << endl;
    }
}

/**
 * Writes the signal assignments of the given block to the given stream.
 *
 * @param block The netlist block.
 * @param stream The stream.
 */
void
VerilogNetlistWriter::writeSignalAssignments(
    const BaseNetlistBlock& block, std::ofstream& stream) const {
    set<const BaseNetlistBlock*, NetlistBlockNameComparator> subBlocks;
    for (size_t i = 0; i < block.subBlockCount(); i++) {
        subBlocks.insert(&block.subBlock(i));
    }

    typedef std::vector<edge_descriptor> EdgeTable;
    EdgeTable handledEdges;

    for (size_t i = 0; i < block.subBlockCount(); i++) {
        const BaseNetlistBlock& subBlock = block.subBlock(i);
        for (size_t i = 0; i < subBlock.portCount(); i++) {
            const NetlistPort& port = subBlock.port(i);
            size_t vertexDescriptor = block.netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, block.netlist());

            while (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                edges.first++;
                if (!ContainerTools::containsValue(
                        handledEdges, edgeDescriptor)) {
                    vertex_descriptor srcVertex =
                        boost::source(edgeDescriptor, block.netlist());
                    vertex_descriptor dstVertex =
                        boost::target(edgeDescriptor, block.netlist());
                    NetlistPort* srcPort = block.netlist()[srcVertex];
                    NetlistPort* dstPort = block.netlist()[dstVertex];

                    if (&dstPort->parentBlock() == &block) {
                        continue;
                    }

                    assert(srcPort == &port);
                    if (AssocTools::containsKey(
                            subBlocks, &srcPort->parentBlock()) &&
                        AssocTools::containsKey(
                            subBlocks, &dstPort->parentBlock())) {
                        handledEdges.push_back(edgeDescriptor);
                        // add the opposite edge too
                        std::pair<edge_descriptor, bool> opposite =
                            boost::edge(
                                dstVertex, srcVertex, block.netlist());
                        assert(opposite.second);
                        assert(opposite.first != edgeDescriptor);
                        handledEdges.push_back(opposite.first);

                        PortConnectionProperty property =
                            block.netlist()[edgeDescriptor];
                        if (property.fullyConnected()) {
                            if (srcPort->direction() == OUT) {
                                stream << indentation(1) << "assign "
                                       << portSignalName(*dstPort) << " = "
                                       << portSignalName(*srcPort) << ";"
                                       << endl;
                            } else {
                                stream << indentation(1) << "assign "
                                       << portSignalName(*srcPort) << " = "
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
                                        portSignalName(*srcPort) + "[" +
                                        Conversion::toString(
                                            property.port1FirstBit()) +
                                        "]";
                                } else {
                                    srcPortSignal =
                                        portSignalName(*srcPort) + "[" +
                                        Conversion::toString(
                                            property.port1FirstBit() +
                                            property.width() - 1) +
                                        ":" +
                                        Conversion::toString(
                                            property.port1FirstBit()) +
                                        "]";
                                }
                            }
                            string dstPortSignal;
                            if (dstPort->dataType() == BIT) {
                                dstPortSignal = portSignalName(*dstPort);
                            } else {
                                if (srcPort->dataType() == BIT) {
                                    dstPortSignal =
                                        portSignalName(*dstPort) + "[" +
                                        Conversion::toString(
                                            property.port2FirstBit()) +
                                        "]";
                                } else {
                                    dstPortSignal =
                                        portSignalName(*dstPort) + "[" +
                                        Conversion::toString(
                                            property.port2FirstBit() +
                                            property.width() - 1) +
                                        ":" +
                                        Conversion::toString(
                                            property.port2FirstBit()) +
                                        "]";
                                }
                            }

                            if (srcPort->direction() == OUT) {
                                stream << indentation(1) << "assign "
                                       << dstPortSignal << " = "
                                       << srcPortSignal << ";" << endl;
                            } else {
                                stream << indentation(1) << "assign "
                                       << srcPortSignal << " = "
                                       << dstPortSignal << ";" << endl;
                            }
                        }
                    }
                }
            }
        }
    }

    if (groundWidth_ > 0) {
        stream << indentation(1) << "assign "
               << GROUND_SIGNAL << " = {" << groundWidth_ <<"{1'b0}};"
               << endl;
    }
}

/**
 * Writes the port mappings of the given block to the given stream.
 *
 * @param block The netlist block.
 * @param stream The stream to write.
 */
void
VerilogNetlistWriter::writePortMappings(
    const BaseNetlistBlock& block, std::ofstream& stream) const {
    for (size_t i = 0; i < block.subBlockCount(); i++) {
        const BaseNetlistBlock& component = block.subBlock(i);

        // virtual NetlistBlocks are omitted
        if (component.isVirtual()) {
            continue;
        }

        stream << indentation(1) << component.moduleName()<< endl;

        // create generic map(parameters)
        if (component.parameterCount() > 0) {
            stream << indentation(1) << "#(" << endl;
            for (size_t i = 0; i < component.parameterCount(); i++) {
                Parameter param = component.parameter(i);
                stream << indentation(2) << "." << param.name() << "(";
                if (param.type().lower() == PARAM_BOOLEAN) {
                    if (param.defaultValue() == "false") {
                        stream << "0";
                    } else if (param.defaultValue() == "true") {
                        stream << "1";
                    } else {
                        string errorMsg = "VerilogNetlistWriter: invalid value for boolean parameter";
                        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
                    }
                } else if (param.type().lower() == PARAM_STRING) {
                    stream << genericMapStringValue(param.value());
                } else {
                    stream << param.value();
                }
                if (i == component.parameterCount() - 1) {
                    stream << ")";
                } else {
                    stream << "),";
                }
                stream << endl;
            }
            stream << indentation(1) << ")" << endl;
        }
        // create port map on unique(!) instance
        stream << indentation(1) << component.instanceName()
               << "_" << i << endl
               << indentation(2) << "(" << endl;
        for (size_t i = 0; i < component.portCount(); i++) {
            const NetlistPort& port = component.port(i);
            size_t vertexDescriptor = block.netlist().descriptor(port);
            std::pair<out_edge_iterator, out_edge_iterator> edges =
                boost::out_edges(vertexDescriptor, block.netlist());

            string srcConn = port.name();
            string dstConn = "";
            if (edges.first != edges.second) {
                edge_descriptor edgeDescriptor = *edges.first;
                vertex_descriptor dstVertex =
                    boost::target(edgeDescriptor, block.netlist());
                const NetlistPort* dstPort = block.netlist()[dstVertex];
                PortConnectionProperty property =
                    block.netlist()[edgeDescriptor];
                if (&dstPort->parentBlock() == &block) {
                    if (port.dataType() != dstPort->dataType()) {
                        int index = 0;
                        if (!property.fullyConnected() &&
                            dstPort->dataType() == BIT_VECTOR &&
                            port.dataType() == BIT) {

                            index = property.port2FirstBit();
                        }

                        if (port.dataType() == BIT) {

                            assert(dstPort->dataType() == BIT_VECTOR);
                            dstConn = dstPort->name() + "[" + 
                                Conversion::toString(index) + "]";
                        } else {

                            assert(dstPort->dataType() == BIT);
                            if (port.widthFormula() == "1") {
                                srcConn += "[0]";
                                dstConn = dstPort->name();
                            } else {
                                dstConn = portSignalName(port);
                            }
                        }
                    } else {

                        if ((!property.fullyConnected() ||
                             dstPort->direction() == OUT) &&
                            boost::out_degree(
                                vertexDescriptor, block.netlist()) > 1) {

                            dstConn = portSignalName(port);
                        } else {

                            dstConn = dstPort->name();
                        }
                    }
                } else {

                    dstConn = portSignalName(port);
                }
            } else {

                dstConn = portSignalName(port);
            }
            stream << indentation(3) << "." << srcConn << "(" << dstConn << ")";
            if (i+1 < component.portCount()) {
                stream << "," << endl;
            }
        }
        stream << ");" << endl << endl;
    }
}

/**
 * Returns the string that means the same direction as the given one in Verilog.
 *
 * @return The direction string.
 */
std::string
VerilogNetlistWriter::directionString(Direction direction) {
    switch (direction) {
        case IN:
            return "input";
        case OUT:
            return "output";
        case BIDIR:
            return "inout";
    }
    assert(false);
    // dummy return
    return "";
}

/**
 * Tells whether the given string is a non-negative integer number.
 *
 * @param formula The string.
 * @return True if the given string is a non-negative integer number.
 */
bool
VerilogNetlistWriter::isNumber(const std::string& formula) {
    int length = formula.length();
    for (int i = 0; i < length; i++) {
        if (!isdigit(formula[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Returns true if port uses single parameter of its parent block as port
 * width.
 */
bool
VerilogNetlistWriter::usesParameterWidth(const NetlistPort& port) {
    const BaseNetlistBlock& parent = port.parentBlock();
    return parent.hasParameter(port.widthFormula());
}

/**
 * Returns a string which makes indetation of the given level.
 *
 * @param level The indentation level.
 */
std::string
VerilogNetlistWriter::indentation(unsigned int level) const {
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
VerilogNetlistWriter::generateIndentation(
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
VerilogNetlistWriter::portSignalName(const NetlistPort& port) {
    const BaseNetlistBlock* parentBlock = &port.parentBlock();
    if (port.hasStaticValue()) {
        std::string portWidth;
        if (port.realWidthAvailable()) {
            portWidth = Conversion::toString(port.realWidth());
        } else {
            portWidth = port.widthFormula();
        }
        return ((port.staticValue().is(StaticSignal::VCC)) ? "'1" : "'0");
    }
    return parentBlock->instanceName() + "_" + port.name() +"_wire";
}


/**
 * Returns the type of the signal mapped to the given port.
 *
 * @param port The port.
 */
std::string
VerilogNetlistWriter::portSignalType(const NetlistPort& port) {
    if (port.dataType() == BIT) {
        return "";
    } else {
        if (port.realWidthAvailable()) {
            int width = port.realWidth();
            return " [" + Conversion::toString(width?width-1:0) + ":0]";
        } else if (isNumber(port.widthFormula()) && 
                   (Conversion::toInt(port.widthFormula()) == 0)) {
            return " [0:0]";
        } else if (usesParameterWidth(port)) {
            return " [" + parameterWidthValue(port) +
                   "-1 :0]";
        } else {
            return " [ " + port.widthFormula()+"-1: 0]";
        }
    }
}

TCEString
VerilogNetlistWriter::genericMapStringValue(const TCEString& generic) const {

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

/**
 * Returns port width value of port that uses parameter as width.
 */
TCEString
VerilogNetlistWriter::parameterWidthValue(const NetlistPort& port) {
    return port.parentBlock().parameter(port.widthFormula()).value();
}


}
