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
 * @file NetlistPortGroup.hh
 *
 * Declaration of NetlistPortGroup class.
 *
 * Created on: 24.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef NETLISTPORTGROUP_HH
#define NETLISTPORTGROUP_HH

#include <vector>
#include <cstddef> // size_t

#include "NetlistPort.hh"

#include "Exception.hh"
#include "SignalGroup.hh"

namespace ProGe {

class BaseNetlistBlock;
class NetlistPort;

/*
 * Presents a group of NetlistPorts belonging together.
 */
class NetlistPortGroup {
public:

    friend class BaseNetlistBlock; // For registration management.

    typedef std::vector<NetlistPort*> PortContainerType;

    NetlistPortGroup(SignalGroup signalGroup = SignalGroup());

    template <typename... PortType>
    NetlistPortGroup(SignalGroup groupType, PortType... ts);

    virtual ~NetlistPortGroup();

    size_t portCount() const;
    const NetlistPort& portAt(size_t index) const;
    NetlistPort& portAt(size_t index);
    void addPort(NetlistPort& port);
    bool hasPortBySignal(SignalType type) const;
    const NetlistPort& portBySignal(SignalType type) const;

    void clear();

    bool hasParent() const;
    const BaseNetlistBlock& parent() const;
    BaseNetlistBlock& parent();

    void assignSignalGroup(SignalGroup signalGroup);
    SignalGroup assignedSignalGroup() const;

    virtual NetlistPortGroup* clone(bool asMirrored = false) const;

    typedef PortContainerType::iterator iterator;
    typedef PortContainerType::const_iterator const_iterator;
    typedef PortContainerType::reverse_iterator reverse_iterator;
    typedef PortContainerType::const_reverse_iterator const_reverse_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

protected:
    NetlistPortGroup(const NetlistPortGroup&, bool asMirrored = false);

private:
    NetlistPortGroup();

    NetlistPortGroup& operator=(const NetlistPortGroup&);

    void setParent(BaseNetlistBlock* newParent);

    /// The parent block where the group belongs to.
    BaseNetlistBlock* parent_;
    /// The ports belonging to this group by reference.
    PortContainerType ports_;
    /// The usage/implemented interface of the group.
    SignalGroup signalGroup_;
};



} /* namespace ProGe */
#include "NetlistPortGroup.icc"

#endif /* NETLISTPORTGROUP_HH_ */
