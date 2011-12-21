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
 * @file AddressSpace.hh
 *
 * Declaration of AddressSpace class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_ADDRESS_SPACE_HH
#define TTA_ADDRESS_SPACE_HH

#include <string>

#include "MachinePart.hh"

namespace TTAMachine {

class Machine;

/**
 * Represents an address space in the machine.
 */
class AddressSpace : public Component {
public:
    AddressSpace(
        const std::string& name,
        int width,
        unsigned int minAddress,
        unsigned int maxAddress,
        Machine& owner)
        throw (ComponentAlreadyExists, OutOfRange, InvalidName);
    AddressSpace(const ObjectState* state, Machine& owner)
        throw (ObjectStateLoadingException);
    virtual ~AddressSpace();

    virtual int width() const;
    virtual unsigned int start() const;
    virtual unsigned int end() const;

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setAddressBounds(unsigned int start, unsigned int end)
        throw (OutOfRange);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for AddressSpace.
    static const std::string OSNAME_ADDRESS_SPACE;
    /// ObjectState attribute key for the bit width.
    static const std::string OSKEY_WIDTH;
    /// ObjectState attribute key for minimum address.
    static const std::string OSKEY_MIN_ADDRESS;
    /// ObjectState attribute key for maximum address.
    static const std::string OSKEY_MAX_ADDRESS;

private:
    /// Bit width of the minimum addressable word.
    int width_;
    /// Lowest address in the address space.
    unsigned int minAddress_;
    /// Highest address in the address space.
    unsigned int maxAddress_;
};
}

#endif
