/*
 Copyright (c) 2002-2016 Tampere University.

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
 * @file MemoryBusInterface.cc
 *
 * Implementation/Declaration of MemoryBusInterface class.
 *
 * Created on: 16.6.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "MemoryBusInterface.hh"

namespace ProGe {

MemoryBusInterface::MemoryBusInterface(const TCEString& addressSpaceName)
    : NetlistPortGroup(SignalGroup()),
      addressSpaceName_(addressSpaceName) {
}

MemoryBusInterface::MemoryBusInterface(
    SignalGroup groupType,
    const TCEString& addressSpaceName)
    : NetlistPortGroup(groupType),
      addressSpaceName_(addressSpaceName) {
}

MemoryBusInterface::MemoryBusInterface(
    const MemoryBusInterface& otherMemIf,
    bool asMirrored)
    : NetlistPortGroup(otherMemIf, asMirrored),
      addressSpaceName_(otherMemIf.addressSpaceName_) {
}

MemoryBusInterface::~MemoryBusInterface() {
}

MemoryBusInterface*
ProGe::MemoryBusInterface::clone(bool asMirrored) const {
    MemoryBusInterface* newIF =  new MemoryBusInterface(*this, asMirrored);
    return newIF;
}

TCEString
MemoryBusInterface::addressSpace() const {
    return addressSpaceName_;
}




} /* namespace ProGe */

