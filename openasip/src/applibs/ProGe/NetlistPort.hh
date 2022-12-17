/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_PORT_HH
#define TTA_NETLIST_PORT_HH

#include <string>

#include "ProGeTypes.hh"
#include "Exception.hh"
#include "Signal.hh"
#include "NetlistTools.hh"

namespace ProGe {

class StaticSignal {
public:
    enum State {
        GND, ///< All port signals set to low.
        VCC, ///< All port signals set to high.
        MIXED, ///< Port has signals both the low and high states.
        OPEN ///< Port is left open/unused.
    };

    StaticSignal(State state) : state_(state) {}
    bool is(State state) { return state_ == state; }
private:
    State state_;
};

class NetlistBlock;
class BaseNetlistBlock;

/**
 * Represents a port in the netlist. Ports are the vertices of the graph
 * that represents the netlist. From the ports, it is possible to reach the
 * parent netlist blocks.
 */
class NetlistPort {
public:
    friend class BaseNetlistBlock;

    NetlistPort(
        const std::string& name,
        const std::string& widthFormula,
        int realWidth,
        DataType dataType,
        Direction direction,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    NetlistPort(
        const std::string& name,
        int realWidth,
        DataType dataType,
        Direction direction,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    NetlistPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType,
        Direction direction,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    NetlistPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType,
        Direction direction,
        Signal signal = Signal());
    virtual ~NetlistPort();

    bool resolveRealWidth(int& width) const;
    NetlistPort* copyTo(
        BaseNetlistBlock& newParent,
        std::string newName="") const;
    virtual NetlistPort* clone(bool asMirrored = false) const;

    std::string name() const;
    void rename(const std::string& newname);
    std::string widthFormula() const;
    void setWidthFormula(const std::string& newFormula);
    bool realWidthAvailable() const;
    int realWidth() const;
    DataType dataType() const;
    Direction direction() const;
    void setDirection(Direction direction);

    bool hasParentBlock() const;
    const BaseNetlistBlock& parentBlock() const;
    BaseNetlistBlock& parentBlock();

    void setToStatic(StaticSignal value) const;
    void unsetStatic() const;
    bool hasStaticValue() const;
    StaticSignal staticValue() const;

    void assignSignal(Signal signal);
    Signal assignedSignal() const;

private:

    void setParent(BaseNetlistBlock* parent);
    NetlistPort(const NetlistPort& other, bool asMirrored = false);

    /// Name of the port.
    std::string name_;
    /// Formula for the width of the port.
    std::string widthFormula_;
    /// Real width of the port.
    int realWidth_;
    /// Data type of the port.
    DataType dataType_;
    /// Direction of the port.
    Direction direction_;
    /// The parent netlist block.
    BaseNetlistBlock* parentBlock_;
    /// Indicates if port is connected to vcc or gnd
    mutable bool hasStaticValue_;
    /// Static signal value
    mutable StaticSignal staticValue_;
    /// Assigned port usage.
    Signal signal_;
};

/// Convenience class for output ports.
class OutPort : public NetlistPort {
public:
    OutPort(
        const std::string& name,
        const std::string& widthFormula,
        int realWidth,
        DataType dataType,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    OutPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    OutPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType = BIT_VECTOR,
        Signal signal = Signal());
};

/// Convenience class for output bit ports.
class OutBitPort : public NetlistPort {
public:
    OutBitPort(
        const std::string& name,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    OutBitPort(
        const std::string& name,
        Signal signal = Signal());
};

/// Convenience class for input ports.
class InPort : public NetlistPort {
public:
    InPort(
        const std::string& name,
        const std::string& widthFormula,
        int realWidth,
        DataType dataType,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    InPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    InPort(
        const std::string& name,
        const std::string& widthFormula,
        DataType dataType = BIT_VECTOR,
        Signal signal = Signal());
};

/// Convenience class for input bit ports.
class InBitPort : public NetlistPort {
public:
    InBitPort(
        const std::string& name,
        BaseNetlistBlock& parent,
        Signal signal = Signal());
    InBitPort(
        const std::string& name,
        Signal signal = Signal());
};

}

#endif
