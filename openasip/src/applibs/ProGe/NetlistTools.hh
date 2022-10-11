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
 * @file NetlistTools.hh
 *
 * Declaration of NetlistTools class.
 *
 * Created on: 5.5.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef NETLISTTOOLS_HH
#define NETLISTTOOLS_HH

#include <cstddef>
#include <utility>
#include <string>
#include <map>

#include "ProGeTypes.hh"
#include "SignalTypes.hh"

namespace ProGe {

class BaseNetlistBlock;
class Netlist;
class NetlistPort;
class NetlistPortGroup;

class NetlistTools {
public:

    static std::pair<const BaseNetlistBlock*, size_t> commonParent(
        const BaseNetlistBlock& b1,
        const BaseNetlistBlock& b2);
    static void addPrefixToPortNames(
        NetlistPortGroup& portGroup, const std::string& prefix);
    static void addPrefixToPortName(
        NetlistPort& port,const std::string& prefix);
    static size_t renamePorts(
        NetlistPortGroup& portGroup,
        std::map<SignalType, const std::string>&& renameRules);
    static std::string getUniqueInstanceName(
        const BaseNetlistBlock& within,
        const std::string& basename);

    static const BaseNetlistBlock* parent(const BaseNetlistBlock& block);
    static const BaseNetlistBlock* parent(const BaseNetlistBlock* block);
    static Direction mirror(Direction direction);
    static NetlistPort* mirror(NetlistPort* port);

};

} /* namespace ProGe */

#endif /* NETLISTTOOLS_HH */
