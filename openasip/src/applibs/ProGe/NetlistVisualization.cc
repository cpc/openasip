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
 * @file NetlistVisualization.cc
 *
 * Implementation of NetlistVisualization class.
 *
 * Created on: 23.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "NetlistVisualization.hh"

#include <iostream>
#include <string>
#include <set>
#include <utility>
#include <boost/graph/graph_traits.hpp>

#include "BaseNetlistBlock.hh"
#include "NetlistPort.hh"
#include "Netlist.hh"
#include "Parameter.hh"
#include "NetlistTools.hh"

#include "Conversion.hh"
#include "TCEString.hh"

namespace ProGe {

NetlistVisualization::NetlistVisualization() {}

NetlistVisualization::~NetlistVisualization() {}

void
NetlistVisualization::visualizeBlockTree(
    const BaseNetlistBlock& root, std::ostream& streamOut) {
    printBlock(root, "", streamOut);
    printBlockTree(root, "", streamOut);
}

void
NetlistVisualization::listConnections(
    const BaseNetlistBlock& block, std::ostream& streamOut) {
    //    typedef boost::graph_traits<Netlist>::edge_iterator EdgeIterator;
    //    typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
    typedef std::pair<std::string, std::string> PortNamePair;
    typedef std::set<PortNamePair> ConnectionSet;

    ConnectionSet connections;
    //    EdgePair edge_it;

    Netlist::const_iterator edge_it;
    for (edge_it = block.netlist().begin(); edge_it != block.netlist().end();
         edge_it++) {
        const NetlistPort* port_a =
            block.netlist()[boost::source(*edge_it, block.netlist())];
        const NetlistPort* port_b =
            block.netlist()[boost::target(*edge_it, block.netlist())];
        std::string port_a_name(port_a->parentBlock().instanceName());
        port_a_name += ":" + port_a->name();
        std::string port_b_name(port_b->parentBlock().instanceName());
        port_b_name += ":" + port_b->name();

        if (port_a_name < port_b_name) {
            connections.insert(std::make_pair(port_a_name, port_b_name));
        } else {
            connections.insert(std::make_pair(port_b_name, port_a_name));
        }
    }

    ConnectionSet::const_iterator conn_it;
    for (conn_it = connections.begin(); conn_it != connections.end();
         conn_it++) {
        streamOut << conn_it->first << " <-> " << conn_it->second
                  << std::endl;
    }
}

void
NetlistVisualization::listNetlistDescriptors(
    const BaseNetlistBlock& block, std::ostream& streamOut) {
    listNetlistDescriptors(block.netlist(), streamOut);
}

void
NetlistVisualization::listNetlistDescriptors(
    const Netlist& netlist, std::ostream& streamOut) {
    Netlist::const_descriptor_iterator desc_it;
    for (desc_it = netlist.descriptorBegin();
         desc_it != netlist.descriptorEnd(); desc_it++) {
        const NetlistPort* port = desc_it->first;
        const BaseNetlistBlock* parent = &port->parentBlock();
        size_t descriptor = desc_it->second;
        streamOut << parent->instanceName() << ":" << port->name() << ":"
                  << descriptor << std::endl;
    }
}

void
NetlistVisualization::printBlockTree(
    const BaseNetlistBlock& blockNode, std::string prefix,
    std::ostream& streamOut) {
    for (size_t i = 0; i < blockNode.parameterCount(); i++) {
        printParameter(blockNode.parameter(i), prefix + "|- ", streamOut);
    }

    for (size_t i = 0; i < blockNode.portCount(); i++) {
        printPort(blockNode.port(i), prefix + "|- ", streamOut);
    }

    for (size_t i = 0; i < blockNode.subBlockCount(); i++) {
        printBlock(blockNode.subBlock(i), prefix + "+- ", streamOut);
        printBlockTree(
            blockNode.subBlock(i),
            prefix + TCEString::applyIf(
                         i < blockNode.subBlockCount() - 1, "|  ", "   "),
            streamOut);
    }
}

void
NetlistVisualization::printBlock(
    const BaseNetlistBlock& blockNode, const std::string& prefix,
    std::ostream& streamOut) {
    streamOut << prefix << "Blk: " << blockNode.instanceName() << " : "
              << blockNode.moduleName() << std::endl;
}

void
NetlistVisualization::printParameter(
    const Parameter& parameter, const std::string& prefix,
    std::ostream& streamOut) {
    streamOut << prefix << "Prm: " << parameter.name() << " : "
              << parameter.type() << " := " << parameter.value() << std::endl;
}

void
NetlistVisualization::printPort(
    const NetlistPort& port, const std::string& prefix,
    std::ostream& streamOut) {
    streamOut << prefix << "Prt: " << port.name() << " "
              << toString(port.direction()) << " " << portWidthToString(port)
              << std::endl;
}

std::string
NetlistVisualization::toString(Direction dir) {
    switch (dir) {
        case IN:
            return "in";
        case OUT:
            return "out";
        case BIDIR:
            return "bidir";
        default:
            return "N/A";
    }
}

std::string
NetlistVisualization::portWidthToString(const NetlistPort& port) {
    if (port.dataType() == BIT) {
        return "1";
    } else if (port.dataType() == BIT_VECTOR) {
        if (port.realWidthAvailable()) {
            return std::string("[") + Conversion::toString(port.realWidth()) +
                   "-1:0]";
        } else {
            return std::string("[") + port.widthFormula() + "-1:0]";
        }
    } else {
        return "N/A";
    }
}

} /* namespace ProGe */
