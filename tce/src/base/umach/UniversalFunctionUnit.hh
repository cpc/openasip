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
 * @file UniversalFunctionUnit.hh
 *
 * Declaration of UniversalFunctionUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_FUNCTION_UNIT_HH
#define TTA_UNIVERSAL_FUNCTION_UNIT_HH

#include "FunctionUnit.hh"
#include "SmartHWOperation.hh"

class Operation;
class OperationPool;

/**
 * UniversalFunctionUnit class represents a function unit which has
 * all the operations of an operation pool. The operations are added
 * on demand, not at construction.
 */
class UniversalFunctionUnit : public TTAMachine::FunctionUnit {
public:
    UniversalFunctionUnit(const std::string& name, OperationPool& opPool)
        throw (InvalidName);
    virtual ~UniversalFunctionUnit();

    virtual bool hasOperation(const std::string& name) const;
    virtual SmartHWOperation* operation(const std::string& name) const
        throw (InstanceNotFound);
    virtual void addPipelineElement(TTAMachine::PipelineElement& element)
        throw (ComponentAlreadyExists);
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    int portCount(int width) const
        throw (OutOfRange);
    TTAMachine::FUPort& port(int index, int width) const
        throw (OutOfRange);

    static bool is32BitOperation(const std::string& opName);

    /// Name of the 32 bit wide opcode setting port.
    static const std::string OC_SETTING_PORT_32;
    /// Name of the 64 bit wide opcode setting port.
    static const std::string OC_SETTING_PORT_64;

private:
    SmartHWOperation& addOperation(const Operation& operation);
    void ensureInputPorts(int width, int count);
    void ensureOutputPorts(int width, int count);

    /// Operation pool from which the operations are searched.
    OperationPool& opPool_;
    /// Table of names of 32 bit operations
    static const std::string OPERATIONS_OF_32_BITS[];


};

#endif
