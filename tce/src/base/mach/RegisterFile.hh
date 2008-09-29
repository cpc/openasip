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
 * @file RegisterFile.hh
 *
 * Declaration of RegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#ifndef TTA_REGISTER_FILE_HH
#define TTA_REGISTER_FILE_HH

#include <string>

#include "MachinePart.hh"
#include "BaseRegisterFile.hh"
#include "ObjectState.hh"

namespace TTAMachine {

/**
 * Represents a register-file in TTA processor.
 */
class RegisterFile : public BaseRegisterFile {
public:
    /// Type of the register file indicates how the RF is used.
    enum Type {
        NORMAL,   ///< Used for general register allocation.
        RESERVED, ///< Used for custom, user controlled register allocation.
        VOLATILE  ///< Used for user-controlled I/O communication.
    };

    RegisterFile(
        const std::string& name,
        unsigned int size,
        unsigned int width,
        unsigned int maxReads,
        unsigned int maxwrites,
        unsigned int guardLatency,
        Type type)
        throw (OutOfRange, InvalidName);
    explicit RegisterFile(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~RegisterFile();
    virtual RegisterFile* copy() const;

    virtual int maxReads() const;
    virtual int maxWrites() const;
    virtual RegisterFile::Type type() const;

    virtual bool isNormal() const;
    virtual bool isVolatile() const;
    virtual bool isReserved() const;

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setMaxReads(int maxReads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setType(RegisterFile::Type type);
    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);

    virtual int guardLatency() const;
    virtual void setGuardLatency(int latency)
        throw (OutOfRange);

    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual bool isArchitectureEqual(const RegisterFile& rf) const;

    virtual bool isUsedAsGuard() const;

    /// ObjectState name for RegisterFile.
    static const std::string OSNAME_REGISTER_FILE;
    /// ObjectState attribute key for register file type.
    static const std::string OSKEY_TYPE;
    /// ObjectState attribute value for normal register file type.
    static const std::string OSVALUE_NORMAL;
    /// ObjectState attribute value for reserved register file type.
    static const std::string OSVALUE_RESERVED;
    /// ObjectState attribute value for volatile register file type.
    static const std::string OSVALUE_VOLATILE;
    /// ObjectState attribute key for maximum simultaneous readers.
    static const std::string OSKEY_MAX_READS;
    /// ObjectState attribute key for maximum simultaneous writers.
    static const std::string OSKEY_MAX_WRITES;
    /// ObjectState attribute key for the guard latency.
    static const std::string OSKEY_GUARD_LATENCY;

protected:

private:
    bool updateMaxReadsAndWrites() const
        throw (OutOfRange);
    /// Copying forbidden, use the copy() method.
    RegisterFile(const RegisterFile&);
    /// Assingment forbidden.
    RegisterFile& operator=(const RegisterFile&);

    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void deleteGuards(int registers) const;

    /// Max number of ports that can read a register all in the same cycle.
    mutable int maxReads_;
    /// Max number of ports that can read a register all in the same cycle.
    mutable int maxWrites_;
    /// The guard latency of the register file.
    int guardLatency_;

    /// Type of the register file.
    Type type_;
};
}

#endif

