/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file AddressSpace.hh
 *
 * Declaration of AddressSpace class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_ADDRESS_SPACE_HH
#define TTA_ADDRESS_SPACE_HH

#include <string>

#include "MachinePart.hh"
#include "Machine.hh"
#include "ObjectState.hh"

namespace TTAMachine {

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
