/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file NetlistPortGroup.cc
 *
 * Implementation of NetlistPortGroup class.
 *
 * Created on: 24.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "NetlistPortGroup.hh"

#include <cassert>
#include <iterator>

#include "BaseNetlistBlock.hh"
#include "NetlistPort.hh"

#include "SequenceTools.hh"

namespace ProGe {

NetlistPortGroup::NetlistPortGroup()
    : parent_(nullptr),
      ports_(),
      signalGroup_() {
}

/**
 * Copy constructor. Copies everything except parent block reference.
 */
NetlistPortGroup::NetlistPortGroup(
    const NetlistPortGroup& other,
    bool asMirrored)
    : parent_(nullptr),
      ports_(),
      signalGroup_(other.signalGroup_) {

    std::insert_iterator<PortContainerType>
        portInserter(ports_, ports_.begin());
    for (size_t i = 0; i < other.portCount(); i++) {
        portInserter = other.portAt(i).clone(asMirrored);
    }
}

/**
 * Constructs empty port group with given SignalGroup.
 */
NetlistPortGroup::NetlistPortGroup(SignalGroup signalGroup)
    : parent_(nullptr),
      ports_(),
      signalGroup_(signalGroup) {
}

NetlistPortGroup::~NetlistPortGroup() {
    if (!hasParent()) {
        SequenceTools::deleteAllItems(ports_);
    }
}

/**
 * Returns number of ports in the group.
 */
size_t
NetlistPortGroup::portCount() const {
    return ports_.size();
}

const NetlistPort&
NetlistPortGroup::portAt(size_t index) const {
    return *ports_.at(index);
}

NetlistPort&
NetlistPortGroup::portAt(size_t index) {
    return *ports_.at(index);
}

void
NetlistPortGroup::addPort(NetlistPort& port) {
    ports_.push_back(&port);
}

bool
NetlistPortGroup::hasPortBySignal(SignalType type) const {
    for (const NetlistPort* p : *this) {
        if (p->assignedSignal() == type) {
            return true;
        }
    }
    return false;
}


/**
 * Returns first found port by given signal type.
 */
const NetlistPort&
NetlistPortGroup::portBySignal(SignalType type) const {
    for (const NetlistPort* p : *this) {
        if (p->assignedSignal() == type) {
            return *p;
        }
    }
    THROW_EXCEPTION(InstanceNotFound,
        "The port group does not have port by given signal type.");
}

/**
 * Clears all ports associated to the port group.
 *
 * The Ports are deleted and detached from the parent block.
 */
void
NetlistPortGroup::clear() {
    SequenceTools::deleteAllItems(ports_);
}

bool
NetlistPortGroup::hasParent() const {
    return parent_ != nullptr;
}

const BaseNetlistBlock&
NetlistPortGroup::parent() const {
    assert(hasParent());
    return *parent_;
}

BaseNetlistBlock&
NetlistPortGroup::parent() {
    assert(hasParent());
    return *parent_;
}

void
NetlistPortGroup::setParent(BaseNetlistBlock* newParent) {
    parent_ = newParent;
}

void
NetlistPortGroup::assignSignalGroup(SignalGroup signalGroup) {
    signalGroup_ = signalGroup;
}

SignalGroup
NetlistPortGroup::assignedSignalGroup() const {
    return signalGroup_;
}

/**
 * Clones the NetlistPort and and its ports without parent block reference
 * since it would break unique port name constraint.
 */
NetlistPortGroup*
NetlistPortGroup::clone(bool asMirrored) const {
    NetlistPortGroup* newGroup = new NetlistPortGroup(*this, asMirrored);
    assert(this->portCount() == newGroup->portCount());
    for (size_t i = 0; i < portCount(); i++) {
        assert(this->portAt(i).assignedSignal().type() ==
            newGroup->portAt(i).assignedSignal().type());
    }
    return newGroup;
}

NetlistPortGroup::iterator
NetlistPortGroup::begin() {
    return ports_.begin();
}

NetlistPortGroup::iterator
NetlistPortGroup::end() {
    return ports_.end();
}

NetlistPortGroup::const_iterator
NetlistPortGroup::begin() const  {
    return ports_.begin();
}

NetlistPortGroup::const_iterator
NetlistPortGroup::end() const {
    return ports_.end();
}

NetlistPortGroup::reverse_iterator
NetlistPortGroup::rbegin() {
    return ports_.rbegin();
}

NetlistPortGroup::reverse_iterator
NetlistPortGroup::rend() {
    return ports_.rend();
}

NetlistPortGroup::const_reverse_iterator
NetlistPortGroup::rbegin() const {
    return ports_.rbegin();
}

NetlistPortGroup::const_reverse_iterator
NetlistPortGroup::rend() const {
    return ports_.rend();
}

} /* namespace ProGe */
