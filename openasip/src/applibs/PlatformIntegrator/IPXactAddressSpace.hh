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
 * @file IPXactAddressSpace.hh
 *
 * Declaration of IPXactAddressSpace class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_ADDRESS_SPACE_HH
#define TTA_IP_XACT_ADDRESS_SPACE_HH

#include "TCEString.hh"
#include "MemoryGenerator.hh"

class IPXactAddressSpace {
public:

    IPXactAddressSpace(
        const TCEString& name,
        int range,
        int memLocationWidth,
        int mauWidth);

    IPXactAddressSpace(const MemInfo& memory);

    TCEString name() const;
    
    int memRange() const;

    int memLocationWidth() const;

    int mauWidth() const;

    
    

private:

    IPXactAddressSpace();

    TCEString name_;
    int range_;
    int memLocationWidth_;
    int mauWidth_;
};

#endif
