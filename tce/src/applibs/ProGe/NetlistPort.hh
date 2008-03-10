/**
 * @file NetlistPort.hh
 *
 * Declaration of NetlistPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_PORT_HH
#define TTA_NETLIST_PORT_HH

#include <string>
#include "HDBTypes.hh"
#include "ProGeTypes.hh"
#include "Exception.hh"

namespace ProGe {

class NetlistBlock;

/**
 * Represents a port in the netlist. Ports are the vertices of the graph
 * that represents the netlist. From the ports, it is possible to reach the
 * parent netlist blocks.
 */
class NetlistPort {
public:
    NetlistPort(
        const std::string& name,
        const std::string& widthFormula,
        int realWidth,
        DataType dataType,
        HDB::Direction direction,
        NetlistBlock& parent);
    NetlistPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType,
        HDB::Direction direction,
        NetlistBlock& parent);

    virtual ~NetlistPort();

    std::string name() const;
    std::string widthFormula() const;
    bool realWidthAvailable() const;
    int realWidth() const
        throw (NotAvailable);
    DataType dataType() const;
    HDB::Direction direction() const;

    NetlistBlock* parentBlock() const;

private:
    /// Name of the port.
    std::string name_;
    /// Formula for the width of the port.
    std::string widthFormula_;
    /// Real width of the port.
    int realWidth_;
    /// Data type of the port.
    DataType dataType_;
    /// Direction of the port.
    HDB::Direction direction_;
    /// The parent netlist block.
    NetlistBlock* parent_;
};
}

#endif
