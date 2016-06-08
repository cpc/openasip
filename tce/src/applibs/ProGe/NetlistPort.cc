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
 * @file NetlistPort.cc
 *
 * Implementation of NetlistPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cctype>

#include "NetlistPort.hh"
#include "NetlistBlock.hh"
#include "Conversion.hh"

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
    HDB::Direction direction,
    NetlistBlock& parent) :
    name_(name), widthFormula_(widthFormula), realWidth_(realWidth),
    dataType_(dataType), direction_(direction), parent_(NULL),
    hasStaticValue_(false), staticValue_(GND) {

    // TODO: there might still be possible regressions from changing realWidth
    // check to "< 0" from "< 1" 
    // RF (and IU?) opcode ports may have width 0
    if (realWidth_ < 0 ) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    parent.addPort(this);
    parent_ = &parent;
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
    HDB::Direction direction,
    NetlistBlock& parent) :
    name_(name), widthFormula_(widthFormula), realWidth_(-1),
    dataType_(dataType), direction_(direction), parent_(NULL),
    hasStaticValue_(false), staticValue_(GND) {

    parent.addPort(this);
    parent_ = &parent;
}


/**
 * Destructor.
 *
 * Removes itself from the parent netlist block.
 */
NetlistPort::~NetlistPort() {
    NetlistBlock* parent = parent_;
    parent_ = NULL;
    parent->removePort(*this);
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
 * Returns the formula that defines the width of the port.
 *
 * @return The formula.
 */
std::string
NetlistPort::widthFormula() const {
    return widthFormula_;
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
NetlistPort::realWidth() const
    throw (NotAvailable) {

    if (!realWidthAvailable()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
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
HDB::Direction
NetlistPort::direction() const {
    return direction_;
}


/**
 * Returns the parent netlist block.
 *
 * @return The parent netlist block.
 */
NetlistBlock*
NetlistPort::parentBlock() const {
    return parent_;
}


void
NetlistPort::setToStatic(StaticSignal value) {

    hasStaticValue_ = true;
    staticValue_ = value;
}

void
NetlistPort::unsetToStatic() {

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

} // namespace ProGe
