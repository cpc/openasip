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
 * @file UnboundedRegisterFile.hh
 *
 * Declaration of UnboundedRegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_REGISTER_FILE_HH
#define TTA_UNIVERSAL_REGISTER_FILE_HH

#include "RegisterFile.hh"

/**
 * UnboundedRegisterFile class represents a register file which has
 * unlimited amount of registers. This kind of register files are used in
 * UniversalMachine.
 */
class UnboundedRegisterFile : public TTAMachine::RegisterFile {
public:
    UnboundedRegisterFile(
        const std::string& name,
        int width,
        RegisterFile::Type type)
        throw (InvalidName, OutOfRange);
    virtual ~UnboundedRegisterFile();

    virtual int numberOfRegisters() const;

    virtual void setMaxReads(int reads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setType(RegisterFile::Type type);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
};

#endif
