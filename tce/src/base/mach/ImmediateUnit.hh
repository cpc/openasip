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
 * @file ImmediateUnit.hh
 *
 * Declaration of ImmediateUnit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_IMMEDIATE_UNIT_HH
#define TTA_IMMEDIATE_UNIT_HH

#include <string>
#include <list>

#include "Machine.hh"
#include "RegisterFile.hh"

class ObjectState;

namespace TTAMachine {

class InstructionTemplate;

/**
 * Represent a immediate unit in the machine.
 */
class ImmediateUnit : public RegisterFile {
public:
    ImmediateUnit(
        const std::string& name,
        unsigned int size,
        unsigned int width,
        unsigned int maxReads,
        unsigned int guardLatency,
        Machine::Extension extension)
        throw (OutOfRange, InvalidName);
    ImmediateUnit(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ~ImmediateUnit();

    virtual void setMaxWrites(int maxWrites)
        throw (OutOfRange);
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    bool signExtends() const { return extension_ == Machine::SIGN; }
    bool zeroExtends() const { return extension_ == Machine::ZERO; }
    
    virtual Machine::Extension extensionMode() const;
    virtual int latency() const;
    virtual void setExtensionMode(Machine::Extension mode);

    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for ImmediateUnit.
    static const std::string OSNAME_IMMEDIATE_UNIT;
    /// ObjectState attribute key for the extension mode.
    static const std::string OSKEY_EXTENSION;
    /// ObjectState attribute value for sign extension.
    static const std::string OSVALUE_SIGN;
    /// ObjectState attribute value for zero extension.
    static const std::string OSVALUE_ZERO;
    /// ObjectState attribute key for latency.
    static const std::string OSKEY_LATENCY;

private:
    virtual void setLatency(int latency)
        throw (OutOfRange);
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /**
     * Extension mode applied to the long immediate when it is narrower than
     * the immediate register.
     */
    Machine::Extension extension_;

    /**
     * Minimum number of cycles needed between the encoding of a long
     * immediate and its earliest transport on a data bus.
     */
    int latency_;
};
}

#endif
