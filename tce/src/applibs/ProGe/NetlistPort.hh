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
 * @file NetlistPort.hh
 *
 * Declaration of NetlistPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_PORT_HH
#define TTA_NETLIST_PORT_HH

#include <string>
#include "HDBTypes.hh"
#include "ProGeTypes.hh"
#include "Exception.hh"

namespace ProGe {

enum StaticSignal {
    GND,
    VCC
};

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

    void setToStatic(StaticSignal value);
    void unsetToStatic();
    bool hasStaticValue() const;

    StaticSignal staticValue() const;

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
    /// Indicates if port is connected to vcc or gnd
    bool hasStaticValue_;
    /// Static signal value
    StaticSignal staticValue_;
};
}

#endif
