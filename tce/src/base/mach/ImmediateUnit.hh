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
#include "ObjectState.hh"

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
