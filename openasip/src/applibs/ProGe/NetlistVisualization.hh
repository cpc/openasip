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
 * @file NetlistVisualization.hh
 *
 * Declaration of NetlistVisualization class.
 *
 * Created on: 23.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef NETLISTVISUALIZATION_HH
#define NETLISTVISUALIZATION_HH

#include <iostream>

#include "ProGeTypes.hh"

namespace ProGe {
class BaseNetlistBlock;
class NetlistPort;
class Netlist;
class Parameter;

/*
 * Class for visualizing netlist structure for debug purposes.
 */
class NetlistVisualization {
public:
    NetlistVisualization();
    virtual ~NetlistVisualization();

    static void visualizeBlockTree(
        const BaseNetlistBlock& root,
        std::ostream& streamOut);
    static void listConnections(
        const BaseNetlistBlock& block,
        std::ostream& streamOut);
    static void listNetlistDescriptors(
        const BaseNetlistBlock& block,
        std::ostream& streamOut);
    static void listNetlistDescriptors(
        const Netlist& netlist,
        std::ostream& streamOut);

    static std::string toString(Direction dir);
    static std::string portWidthToString(const NetlistPort& port);

private:

    static void printBlockTree(
        const BaseNetlistBlock& node,
        std::string prefix,
        std::ostream& streamOut);
    static void printBlock(
        const BaseNetlistBlock& root,
        const std::string& prefix,
        std::ostream& streamOut);
    static void printParameter(
        const Parameter& parameter,
        const std::string& prefix,
        std::ostream& streamOut);
    static void printPort(
        const NetlistPort& port,
        const std::string& prefix,
        std::ostream& streamOut);

};

} /* namespace ProGe */

#endif /* NETLISTVISUALIZATION_HH */
