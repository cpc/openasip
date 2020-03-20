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
 * @file TerminalAddress.hh
 *
 * Declaration of TerminalAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_ADDRESS_HH
#define TTA_TERMINAL_ADDRESS_HH

#include "TerminalImmediate.hh"
#include "Address.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {

/**
 * Represents an inline immediate that refers to a data address.
 */
class TerminalAddress : public TerminalImmediate {
public:
    TerminalAddress(SimValue value, const TTAMachine::AddressSpace& space);
    virtual ~TerminalAddress();

    virtual bool isAddress() const;
    virtual Address address() const;
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
private:
    /// Assignment not allowed.
    TerminalAddress& operator=(const TerminalAddress&);
    /// Address space of the address.
    const TTAMachine::AddressSpace& space_;
};

}

#endif
