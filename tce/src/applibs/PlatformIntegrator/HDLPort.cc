/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file HDLPort.cc
 *
 * Implementation of HDLPort class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include "HDLPort.hh"
#include "NetlistPort.hh"
#include "NetlistBlock.hh"
using ProGe::NetlistPort;

HDLPort::HDLPort(
    const std::string name,
    const std::string& widthFormula,
    ProGe::DataType type,
    HDB::Direction direction,
    bool needsInversion): name_(name), widthFormula_(widthFormula),
                          type_(type), direction_(direction),
                          needsInversion_(needsInversion), hasWidth_(false),
                          width_(0), hasStaticValue_(false),
                          staticValue_(ProGe::GND) {
}

HDLPort::HDLPort(
    const std::string name,
    const std::string& widthFormula,
    ProGe::DataType type,
    HDB::Direction direction,
    bool needsInversion,
    int width): name_(name), widthFormula_(widthFormula), type_(type),
                direction_(direction), needsInversion_(needsInversion),
                hasWidth_(true), width_(width), hasStaticValue_(false),
                staticValue_(ProGe::GND) {
}

HDLPort::HDLPort(const HDLPort& old):
    name_(old.name_), widthFormula_(old.widthFormula_), type_(old.type_),
    direction_(old.direction_), needsInversion_(old.needsInversion_),
    hasWidth_(old.hasWidth_), width_(old.width_),
    hasStaticValue_(old.hasStaticValue_), staticValue_(old.staticValue_) {
}


HDLPort::HDLPort(const ProGe::NetlistPort& port):
    name_(port.name()), widthFormula_(port.widthFormula()),
    type_(port.dataType()), direction_(port.direction()),
    needsInversion_(false), hasWidth_(port.realWidthAvailable()), width_(0),
    hasStaticValue_(false), staticValue_(ProGe::GND) {
    
    if (port.realWidthAvailable()) {
        width_ = port.realWidth();
    }
}


std::string 
HDLPort::name() const {
    return name_;
}

std::string 
HDLPort::widthFormula() const {
    return widthFormula_;
}

bool 
HDLPort::hasRealWidth() const {
    return hasWidth_;
}

int 
HDLPort::realWidth() const {
    return width_;
}

HDB::Direction 
HDLPort::direction() const {
    return direction_;
}

ProGe::DataType 
HDLPort::type() const {
    return type_;
}

bool 
HDLPort::needsInversion() const {
    return needsInversion_;
}

ProGe::NetlistPort*
HDLPort::convertToNetlistPort(ProGe::NetlistBlock& block) const {

    NetlistPort* port;
    if (hasWidth_) {
        port = new NetlistPort(name_, widthFormula_, width_, type_,
                               direction_, block);
    } else {
        port = new NetlistPort(name_, widthFormula_, type_, direction_,
                               block);
    }
    if (hasStaticValue_) {
        port->setToStatic(staticValue_);
    }
    return port;
}


void
HDLPort::setToStatic(ProGe::StaticSignal value) {
    
    hasStaticValue_ = true;
    staticValue_ = value;
}


bool
HDLPort::hasStaticValue() const {

    return hasStaticValue_;
}


ProGe::StaticSignal
HDLPort::staticValue() const {
    
    return staticValue_;
}
