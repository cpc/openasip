/**
 * @file NetlistBlock.hh
 *
 * Declaration of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
