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
        const std::string& name, unsigned int size, unsigned int width,
        unsigned int maxReads, unsigned int maxwrites,
        unsigned int guardLatency, Type type);
    explicit RegisterFile(const ObjectState* state);
    virtual ~RegisterFile();
    virtual RegisterFile* copy() const;

    virtual int maxReads() const;
    virtual int maxWrites() const;
    virtual RegisterFile::Type type() const;

    virtual bool isNormal() const;
    virtual bool isVolatile() const;
    virtual bool isReserved() const;

    virtual void setName(const std::string& name);
    virtual void setMaxReads(int maxReads);
    virtual void setMaxWrites(int maxWrites);
    virtual void setType(RegisterFile::Type type);
    virtual void setNumberOfRegisters(int registers);

    virtual int guardLatency() const;
    virtual void setGuardLatency(int latency);

    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

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

    Port* firstReadPort() const;
    Port* firstWritePort() const;

protected:

private:
    bool updateMaxReadsAndWrites() const;
    /// Copying forbidden, use the copy() method.
    RegisterFile(const RegisterFile&);
    /// Assingment forbidden.
    RegisterFile& operator=(const RegisterFile&);

    void loadStateWithoutReferences(const ObjectState* state);
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

