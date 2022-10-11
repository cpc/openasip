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
 * @file NetlistPort.cc
 *
 * Implementation of NetlistPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <cctype>

#include "NetlistPort.hh"

#include "BaseNetlistBlock.hh"
#include "NetlistBlock.hh"
#include "Conversion.hh"
#include "NetlistTools.hh"

namespace ProGe {

/**
 * Constructor. Creates a netlist port with a defined bit width.
 *
 * Creates a port that has a known bit width. Adds the port automatically to
 * the parent block. If a formula for calculating the bit width is given, it
 * should match the actual bit width (an integer number). No check is
 * performed, however, to make sure that the formula is compatible with the
 * actual width. In case of mismatch, the error can be detected only after
 * generation (for example, in a logic synthesis tool).
 *
 * @param name Name of the port.
 * @param widthFormula Formula for calculating the width.
 * @param realWidth Actual width of the port.
 * @param dataType Type of the data.
 * @param direction Direction of the port.
 * @param parent The parent netlist block.
 * @exception OutOfRange If the actual width is not positive ( <0 ).
 */
NetlistPort::NetlistPort(
    const std::string& name,
    const std::string& widthFormula,
    int realWidth,
    DataType dataType,
    Direction direction,
    BaseNetlistBlock& parent,
    Signal signal)
    : name_(name),
      widthFormula_(widthFormula),
      realWidth_(realWidth),
      dataType_(dataType),
      direction_(direction),
      parentBlock_(&parent),
      hasStaticValue_(false),
      staticValue_(StaticSignal::GND),
      signal_(signal) {

    // TODO: there might still be possible regressions from changing realWidth
    // check to "< 0" from "< 1" 
    // RF (and IU?) opcode ports may have width 0
    if (realWidth_ < 0 ) {
        TCEString msg = "Port ";
        msg << name << " has a negative width.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    parent.addPort(this);
}

/**
 * Constructor. Creates a netlist port with a defined bit width,
 * and derive formula from the integer value
 *
 * @param name Name of the port.
 * @param realWidth Actual width of the port.
 * @param dataType Type of the data.
 * @param direction Direction of the port.
 * @param parent The parent netlist block.
 * @exception OutOfRange If the actual width is not positive ( <0 ).
 */
NetlistPort::NetlistPort(
    const std::string& name,
    int realWidth,
    DataType dataType,
    Direction direction,
    BaseNetlistBlock& parent,
    Signal signal)
    : name_(name),
      widthFormula_(Conversion::toString(realWidth)),
      realWidth_(realWidth),
      dataType_(dataType),
      direction_(direction),
      parentBlock_(&parent),
      hasStaticValue_(false),
      staticValue_(StaticSignal::GND),
      signal_(signal) {

    // TODO: there might still be possible regressions from changing realWidth
    // check to "< 0" from "< 1" 
    // RF (and IU?) opcode ports may have width 0
    if (realWidth_ < 0 ) {
        TCEString msg = "Port ";
        msg << name << " has a negative width.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    parent.addPort(this);
}

/**
 * Constructor. Creates a new netlist port.
 *
 * Creates a port that has its bit width defined symbolically, by an
 * expression (formula). Adds the port automatically to the parent block.
 *
 * @param name Name of the port.
 * @param widthFormula Formula for calculating the width.
 * @param dataType Type of the data.
 * @param direction Direction of the port.
 * @param parent The parent netlist block.
 */
NetlistPort::NetlistPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    Direction direction,
    BaseNetlistBlock& parent,
    Signal signal)
    : name_(name),
      widthFormula_(widthFormula),
      realWidth_(-1),
      dataType_(dataType),
      direction_(direction),
      parentBlock_(&parent),
      hasStaticValue_(false),
      staticValue_(StaticSignal::GND),
      signal_(signal) {

    parent.addPort(this);
}

/**
 * Copy constructor. Copies everything except parent block reference since one
 * NetlistBlock may not have ports with identical names.
 */
NetlistPort::NetlistPort(const NetlistPort& other, bool asMirrored)
    : name_(other.name_),
      widthFormula_(other.widthFormula_),
      realWidth_(other.realWidth_),
      dataType_(other.dataType_),
      direction_(other.direction_),
      parentBlock_(nullptr),
      hasStaticValue_(other.hasStaticValue_),
      staticValue_(other.staticValue_),
      signal_(other.signal_) {

    if (asMirrored) {
        direction_ = NetlistTools::mirror(other.direction_);
    }
}

NetlistPort::NetlistPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    Direction direction,
    Signal signal)
    : name_(name),
      widthFormula_(widthFormula),
      realWidth_(-1),
      dataType_(dataType),
      direction_(direction),
      parentBlock_(nullptr),
      hasStaticValue_(false),
      staticValue_(StaticSignal::GND),
      signal_(signal) {
}

bool
NetlistPort::resolveRealWidth(int& width) const {
    
    std::string formula = widthFormula();
    // check if it is a parameter
    for (size_t i = 0; i < parentBlock().netlist().parameterCount(); i++) {
        Parameter param = parentBlock().netlist().parameter(i);
        if (param.name() == formula) {
            width = Conversion::toInt(param.value());
            return true;
        }
    }
    
    // check if formula is a plain number
    bool success = false;
    try {
        width = Conversion::toInt(formula);
        success = true;
    } catch (Exception& e) {
        success = false;
    }
    return success;
}

/**
 * DEPRECATED
 */
NetlistPort*
NetlistPort::copyTo(
    BaseNetlistBlock& newParent,
    std::string newName) const {
    if (newName == "")
        newName = this->name();

    if (realWidthAvailable()) {
        return new NetlistPort(
            newName, widthFormula(), realWidth(), dataType(), direction(), 
            newParent, assignedSignal());
    } else {
        int width = 0;
        if (resolveRealWidth(width)) {
            return new NetlistPort(
                newName, widthFormula(), width, dataType(), direction(), 
                newParent, assignedSignal());
        } else {
            return new NetlistPort(
                newName, widthFormula(), dataType(), direction(), newParent,
                assignedSignal());
        }
    }
    return NULL; 
}


NetlistPort*
NetlistPort::clone(bool asMirrored) const {
    NetlistPort* newPort = new NetlistPort(*this, asMirrored);
    assert(newPort->assignedSignal().type() == this->assignedSignal().type());
    return newPort;
}


/**
 * Destructor.
 *
 * Removes itself from the parent netlist block.
 */
NetlistPort::~NetlistPort() {
    if (hasParentBlock()) {
        parentBlock_->removePort(this);
    }
}


/**
 * Returns the name of the port.
 *
 * @return The name of the port.
 */
std::string
NetlistPort::name() const {
    return name_;
}


/**
 * Sets new name of the port.
 *
 * @return The name of the port.
 */
void
NetlistPort::rename(const std::string& newname) {
    if (hasParentBlock()) {
        if(parentBlock_->port(newname, false) == nullptr ||
            parentBlock_->port(newname, false) == this) {
            name_ = newname;
        } else {
            THROW_EXCEPTION(ObjectAlreadyExists, "Port to be renamed ("
                + name() +") to " + newname +
                " is not unique within the block.");
        }
    } else {
        name_ = newname;
    }
}


/**
 * Returns the formula that defines the width of the port.
 *
 * @return The formula.
 */
std::string
NetlistPort::widthFormula() const {
    return widthFormula_;
}

/**
 * Changes port's width formula.
 */
void
NetlistPort::setWidthFormula(const std::string& newFormula) {
    widthFormula_ = newFormula;
}

/**
 * Tells whether the actual bit width of the port is known.
 *
 * @return True if the width is known, otherwise false.
 */
bool
NetlistPort::realWidthAvailable() const {
    // TODO: there might still be possible regressions from changing realWidth
    // check to allow zero.
    return realWidth_ >= 0;
}


/**
 * Returns the actual bit width of the port.
 *
 * @return The actual bit width.
 * @exception NotAvailable If the actual width is not known.
 */
int
NetlistPort::realWidth() const {
    if (!realWidthAvailable()) {
        throw NotAvailable(__FILE__, __LINE__, __func__, "Port " + name()
                           + " doesn't have actual bit width.");
    }
    return realWidth_;
}

/**
 * Returns the data type of the port.
 *
 * @return The data type of the port.
 */
DataType
NetlistPort::dataType() const {
    return dataType_;
}


/**
 * Returns the direction of the port.
 *
 * @return The direction of the port.
 */
Direction
NetlistPort::direction() const {
    return direction_;
}

/**
 * Sets direction of the port.
 */
void
NetlistPort::setDirection(Direction direction) {
    direction_ = direction;
}

/**
 * Returns true if ports is attached to some netlist block. Otherwise,
 * returns false.
 */
bool
NetlistPort::hasParentBlock() const {
    return parentBlock_ != NULL;
}

/**
 * Returns the parent netlist block.
 *
 * @return The parent netlist block.
 */
const BaseNetlistBlock&
NetlistPort::parentBlock() const {
    return *parentBlock_;
}

BaseNetlistBlock&
NetlistPort::parentBlock() {
    return *parentBlock_;
}

void
NetlistPort::setToStatic(StaticSignal value) const {

    hasStaticValue_ = true;
    staticValue_ = value;
}

void
NetlistPort::unsetStatic() const {

    hasStaticValue_ = false;
}

bool
NetlistPort::hasStaticValue() const {
    return hasStaticValue_;
}


StaticSignal
NetlistPort::staticValue() const {
    return staticValue_;
}

/**
 * Set parent block of this port.
 *
 * @param newParent The new parent. Can be NULL too.
 */
void
NetlistPort::setParent(BaseNetlistBlock* parent) {
    parentBlock_ = parent;
}

/**
 * Assign signal to signify usage of the port.
 */
void
NetlistPort::assignSignal(Signal signal) {
    signal_ = signal;
}

/**
 * Return signal assigned to the port.
 */
Signal
NetlistPort::assignedSignal() const {
    return signal_;
}

OutPort::OutPort(
    const std::string& name,
    const std::string& widthFormula,
    int realWidth,
    DataType dataType,
    BaseNetlistBlock& parent,
    Signal signal)
    : NetlistPort(name, widthFormula, realWidth,
        dataType, OUT, parent, signal) {
}

OutPort::OutPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    BaseNetlistBlock& parent,
    Signal signal)
    : NetlistPort(name, widthFormula, dataType, OUT, parent, signal) {
}

OutPort::OutPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    Signal signal)
    : NetlistPort(name, widthFormula, dataType, OUT, signal) {
}

OutBitPort::OutBitPort(
    const std::string& name,
    BaseNetlistBlock& parent,
    Signal signal)
    : NetlistPort(name, "1", 1, BIT, OUT, parent, signal) {
}

OutBitPort::OutBitPort(
    const std::string& name,
    Signal signal)
    : NetlistPort(name, "1", BIT, OUT, signal) {
}

InPort::InPort(
    const std::string& name,
    const std::string& widthFormula,
    int realWidth,
    DataType dataType,
    BaseNetlistBlock& parent,
    Signal signal)
    : NetlistPort(name, widthFormula, realWidth,
        dataType, IN, parent, signal) {
}

InPort::InPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    BaseNetlistBlock& parent,
    Signal signal)
    : NetlistPort(name, widthFormula, dataType, IN, parent, signal) {
}

InPort::InPort(
    const std::string& name,
    const std::string& widthFormula,
    DataType dataType,
    Signal signal)
    : NetlistPort(name, widthFormula, dataType, IN, signal) {
}

InBitPort::InBitPort(
    const std::string& name,
    BaseNetlistBlock& parent,
    Signal signal)
     : NetlistPort(name, "1", 1, BIT, IN, parent, signal) {
}

InBitPort::InBitPort(
    const std::string& name,
    Signal signal)
    : NetlistPort(name, "1", BIT, IN, signal) {
}

} // namespace ProGe
