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
 * @file NullImmediateUnit.hh
 *
 * Declaration of NullImmediateUnit class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_IMMEDIATE_UNIT_HH
#define TTA_NULL_IMMEDIATE_UNIT_HH

#include "ImmediateUnit.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null ImmediateUnit.
 *
 * All the methods abort the program.
 */
class NullImmediateUnit : public ImmediateUnit {
public:
    static NullImmediateUnit& instance();

    virtual Machine* machine() const;
    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual bool hasPort(const std::string& name) const;
    virtual int portCount() const;

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual Machine::Extension extensionMode() const;
    virtual int latency() const;
    virtual void setExtensionMode(Machine::Extension mode);
    virtual void setLatency(int latency)
        throw (OutOfRange);

    virtual int maxReads() const;
    virtual int maxWrites() const;
    virtual RegisterFile::Type type() const;

    virtual bool isNormal() const;
    virtual bool isVolatile() const;
    virtual bool isReserved() const;

    virtual int numberOfRegisters() const;
    virtual int width() const;

    virtual void setMaxReads(int reads)
        throw (OutOfRange);
    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setType(RegisterFile::Type type);

    virtual int guardLatency() const;
    virtual void setGuardLatency(int latency)
        throw (OutOfRange);

    virtual void setNumberOfRegisters(int registers)
        throw (OutOfRange);
    virtual void setWidth(int width)
        throw (OutOfRange);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullImmediateUnit();
    virtual ~NullImmediateUnit();

    static NullImmediateUnit instance_;
};
}

#endif
