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
 * @file BaseRegisterFile.hh
 *
 * Declaration of abstract BaseRegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#ifndef TTA_BASE_REGISTER_FILE_HH
#define TTA_BASE_REGISTER_FILE_HH

#include <string>

#include "Unit.hh"
#include "RFPort.hh"

namespace TTAMachine {

/**
 * An abstract base class for register files as ImmediateUnit and
 * RegisterFile.
 */
class BaseRegisterFile : public Unit {
public:
    virtual ~BaseRegisterFile();

    virtual int numberOfRegisters() const;
    virtual int width() const;
    virtual int size() const;

    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);

    virtual RFPort* port(const std::string& name) const
        throw (InstanceNotFound);
    virtual RFPort* port(int index) const
        throw (OutOfRange);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for BaseRegisterFile.
    static const std::string OSNAME_BASE_REGISTER_FILE;
    /// ObjectState attribute key for the number of registers.
    static const std::string OSKEY_SIZE;
    /// ObjectState attribute key for bit width of the registers.
    static const std::string OSKEY_WIDTH;

protected:
    BaseRegisterFile(const std::string& name, int size, int width)
        throw (OutOfRange, InvalidName);
    BaseRegisterFile(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Number of registers in the register file.
    int size_;
    /// Bit width of the registers in the register file.
    int width_;
};
}

#include "BaseRegisterFile.icc"

#endif
