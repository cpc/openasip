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
 * @file NullAddressSpace.hh
 *
 * Declaration of NullAddressSpace class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_ADDRESS_SPACE_HH
#define TTA_NULL_ADDRESS_SPACE_HH

#include "AddressSpace.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null AddressSpace.
 *
 * All the methods abort the program.
 */
class NullAddressSpace : public AddressSpace {
public:
    static NullAddressSpace& instance();

    virtual int width() const;
    virtual unsigned int start() const;
    virtual unsigned int end() const;

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setAddressBounds(unsigned int start, unsigned int end)
        throw (OutOfRange);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullAddressSpace();
    virtual ~NullAddressSpace();

    /// The only instance of NullAddressSpace.
    static NullAddressSpace instance_;
    /// Machine to which the null address space is registered.
    static Machine machine_;
};
}

#endif
