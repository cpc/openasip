/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file NetlistBlock.hh
 *
 * Declaration of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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

private:
    /// Vector type for NetlistBlock.
    typedef std::vector<NetlistBlock*> NetlistBlockTable;
    /// Vector type for NetlistPort.
    typedef std::vector<NetlistPort*> PortTable;
    /// Vector type for parameters.
    typedef std::vector<Netlist::Parameter> ParameterTable;

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
}

#endif
