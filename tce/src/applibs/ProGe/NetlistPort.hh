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
 * @file NetlistPort.hh
 *
 * Declaration of NetlistPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
