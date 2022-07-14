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
 * @file NetlistTools.cc
 *
 * Implementation of NetlistTools class.
 *
 * Created on: 5.5.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "NetlistTools.hh"

#include <cstddef>
#include <set>
#include <algorithm>
#include <iterator>

#include "BaseNetlistBlock.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"

namespace ProGe {

/**
 * Finds common parent block of given netlist blocks and maximum distance to
 * the common parent.
 *
 * Distance is calculated how many times parent block pointer is needed to
 * followed to get to the common parent block.
 *
 * @return Returns found common parent and maximum distance to the parent as
 *         pair. The first member is the common parent and the second is the
 *         distance. If common parent has not found return (NULL, 0).
 */
std::pair<const BaseNetlistBlock*, size_t>
NetlistTools::commonParent(
    const BaseNetlistBlock& b1, const BaseNetlistBlock& b2) {
    // Todo check trivial cases: b1 id parent of b2 or vice versa.

    std::set<const BaseNetlistBlock*> parentChain;

    const BaseNetlistBlock* block = &b1;
    do {
        parentChain.insert(block);
    } while ((block = parent(block)));

    const BaseNetlistBlock* found = NULL;
    block = &b2;
    do {
        if (parentChain.count(block)) {
            found = block;
            break;
        }
    } while ((block = parent(block)));

    if (found == NULL) {
        return std::make_pair(found, 0);
    }

    size_t distance = 0;
    block = &b1;
    size_t hops = 0;
    while (block != found) {
        hops++;
        block = parent(block);
    }
    distance = hops;
    block = &b2;
    hops = 0;
    while (block != found) {
        hops++;
        block = parent(block);
    }
    distance = std::max(distance, hops);

    return std::make_pair(found, distance);
}

/**
 * Renames ports by adding prefix into them.
 */
void
NetlistTools::addPrefixToPortNames(
    NetlistPortGroup& portGroup, const std::string& prefix) {
    for (NetlistPort* port : portGroup) {
        addPrefixToPortName(*port, prefix);
    }
}

/**
 * Renames port by adding prefix into it.
 */
void
NetlistTools::addPrefixToPortName(
    NetlistPort& port, const std::string& prefix) {
    port.rename(prefix + port.name());
}

/**
 * Renames ports in netlist port group by given rules.
 *
 * returns number of ports left renamed.
 */
size_t
NetlistTools::renamePorts(
    NetlistPortGroup& portGroup,
    std::map<SignalType, const std::string>&& renameRules) {
    size_t renamedCount = 0;
    for (NetlistPort* port : portGroup) {
        SignalType type = port->assignedSignal().type();
        if (renameRules.count(type)) {
            port->rename(renameRules.at(type));
            renamedCount++;
        }
    }
    return portGroup.portCount() - renamedCount;
}

/**
 * Returns a string that is unique within the given netlist block.
 *
 * That is, the string as instance name does not clash with any of the
 * immediate sub block of the given netlist block.
 *
 * @param within The netlist block.
 * @param basename The string used as base for the instance name.
 * @return The unique instance name. May be the given base name if it itself
 *         is unique within the block. Otherwise the the returned string is
 *         base name postfixed with a running number.
 */
std::string
NetlistTools::getUniqueInstanceName(
    const BaseNetlistBlock& within, const std::string& basename) {
    std::string name(basename);
    while (within.hasSubBlock(name)) {
        int postFixNumber = -1;
        std::string::iterator it;
        std::string::reverse_iterator rit;
        for (rit = name.rbegin(); rit != name.rend(); rit++) {
            std::locale loc;
            if (!std::isdigit(*rit, loc)) {
                it = rit.base();  // Points to first number digit or end().
                break;
            }
        }
        if (it != name.end()) {
            postFixNumber = Conversion::toInt(std::string(it, name.end()));
        }
        postFixNumber++;
        name.replace(it, name.end(), Conversion::toString(postFixNumber));
    }
    return name;
}

/**
 * Returns parent of the given block or NULL if the block does not have one.
 */
const BaseNetlistBlock*
NetlistTools::parent(const BaseNetlistBlock& block) {
    return parent(&block);
}

/**
 * Returns parent of the given block or NULL if the block does not have one.
 */
const BaseNetlistBlock*
NetlistTools::parent(const BaseNetlistBlock* block) {
    if (!block->hasParentBlock()) {
        return NULL;
    } else {
        return &block->parentBlock();
    }
}

/**
 * Returns given direction mirrored.
 */
Direction
NetlistTools::mirror(Direction direction) {
    switch (direction) {
        case IN:
            return OUT;
            break;
        case OUT:
            return IN;
            break;
        case BIDIR:
            return BIDIR;
            break;
        default:
            assert(false && "Unregocnized direction.");
            return IN;
    }
}

/**
 * Mirrors ports direction.
 */
NetlistPort*
NetlistTools::mirror(NetlistPort* port) {
    port->setDirection(mirror(port->direction()));
    return port;
}

} /* namespace ProGe */
