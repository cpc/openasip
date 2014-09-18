/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file NetlistBlock.hh
 *
 * Declaration of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_BLOCK_HH
#define TTA_NETLIST_BLOCK_HH

#include <string>
#include <vector>
#include <map>

#include "Exception.hh"
#include "Netlist.hh"

namespace HDB {
    class HWBlockImplementationParameter;
}

namespace ProGe {

class Netlist;
class NetlistPort;

/**
 * Represents a hardware block in the netlist.
 */
class NetlistBlock {
public:
    NetlistBlock(
        const std::string& moduleName,
        const std::string& instanceName,
        Netlist& netlist);
    virtual ~NetlistBlock();

    std::string instanceName() const;
    std::string moduleName() const;

    void setParameter(
        const std::string& name,
        const std::string& type,
        const std::string& value);
    void setParameter(const Netlist::Parameter& param);

    bool hasParameter(const std::string& name) const;
    Netlist::Parameter parameter(const std::string& name) const
        throw (NotAvailable);
    int parameterCount() const;
    Netlist::Parameter parameter(int index) const
        throw (OutOfRange);

    void addPort(NetlistPort* port);
    void removePort(NetlistPort& port);
    int portCount() const;
    NetlistPort& port(int index) const
        throw (OutOfRange);

    NetlistPort* portByName(const std::string& name) const;

    void addSubBlock(NetlistBlock* block)
        throw (IllegalRegistration);
    int subBlockCount() const;
    NetlistBlock& subBlock(int index) const
        throw (OutOfRange);
    bool isSubBlock(const NetlistBlock& block) const;

    bool hasParentBlock() const;
    NetlistBlock& parentBlock() const
        throw (InstanceNotFound);

    Netlist& netlist() const;

    NetlistBlock* copyToNewNetlist(
        const std::string& instanceName,
        Netlist& destination) const;

    virtual bool isVirtual() const;

private:
    /// Vector type for NetlistBlock.
    typedef std::vector<NetlistBlock*> NetlistBlockTable;
    /// Vector type for NetlistPort.
    typedef std::vector<NetlistPort*> PortTable;
    /// Vector type for parameters.
    typedef std::vector<Netlist::Parameter> ParameterTable;

    bool resolveRealWidth(const NetlistPort* port, int& width) const;

    /// Name of the module.
    std::string moduleName_;
    /// Name of the block instance.
    std::string instanceName_;
    /// The parent NetlistBlock.
    NetlistBlock* parentBlock_;
    /// Child blocks.
    NetlistBlockTable childBlocks_;
    /// The netlist which the block belongs to.
    Netlist& netlist_;
    /// Ports of the block.
    PortTable ports_;
    /// Parameters of the block.
    ParameterTable parameters_;

};

// Lexicographical comparison of 2 strings.
// @return true if a comes before b in dictionary order.
class LexicographicCompare {
public:
    bool operator () (const NetlistBlock* a, const NetlistBlock* b) const {
        return (a->instanceName() + a->moduleName()) <
                (b->instanceName() + b->moduleName());
    }
};

}

#endif
