/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file IPXactAddressSpace.cc
 *
 * Implementation of IPXactAddressSpace class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "IPXactAddressSpace.hh"

IPXactAddressSpace::IPXactAddressSpace(
    const TCEString& name,
    int range,
    int memLocationWidth,
    int mauWidth):
    name_(name), range_(range), memLocationWidth_(memLocationWidth),
    mauWidth_(mauWidth) {
}

IPXactAddressSpace::IPXactAddressSpace(const MemInfo& memory):
    name_(memory.asName), range_(1 << memory.asAddrw),
    memLocationWidth_(memory.mauWidth*memory.widthInMaus),
    mauWidth_(memory.mauWidth) {
}


TCEString
IPXactAddressSpace::name() const{

    return name_;
}
    
int
IPXactAddressSpace::memRange() const {

    return range_;
}

int
IPXactAddressSpace::memLocationWidth() const {
    
    return memLocationWidth_;
}

int
IPXactAddressSpace::mauWidth() const {

    return mauWidth_;
}
