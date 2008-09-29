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
 * @file FunctionUnit.hh
 *
 * Declaration of FunctionUnit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_FUNCTION_UNIT_HH
#define TTA_FUNCTION_UNIT_HH

#include <string>
#include <vector>

#include "Unit.hh"
#include "ObjectState.hh"
#include "BaseFUPort.hh"

namespace TTAMachine {

class HWOperation;
class AddressSpace;
class HWOperation;
class FUPort;
class PipelineElement;

/**
 * Represents a function unit in machine object model.
 */
class FunctionUnit : public Unit {
public:
    FunctionUnit(const std::string& name)
        throw (InvalidName);
    FunctionUnit(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~FunctionUnit();

    virtual FunctionUnit* copy() const;

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual BaseFUPort* port(const std::string& name) const
        throw (InstanceNotFound);
    virtual BaseFUPort* port(int index) const
        throw (OutOfRange);
    virtual int operationPortCount() const;
    virtual bool hasOperationPort(const std::string& name) const;
    virtual FUPort* operationPort(const std::string& name) const
        throw (InstanceNotFound);
    virtual FUPort* operationPort(int index) const
        throw (OutOfRange);

    virtual void addOperation(HWOperation& operation)
        throw (ComponentAlreadyExists);
    virtual void deleteOperation(HWOperation& operation)
        throw (InstanceNotFound);

    virtual bool hasOperation(const std::string& name) const;
    virtual bool hasOperationLowercase(const std::string& name) const;

    virtual HWOperation* operation(const std::string& name) const
        throw (InstanceNotFound);
    virtual HWOperation* operationLowercase(const std::string& name) const
        throw (InstanceNotFound);

    virtual HWOperation* operation(int index) const
        throw (OutOfRange);
    virtual int operationCount() const;

    virtual void operationNames(std::set<std::string>& opNames) const;

    virtual int maxLatency() const;

    virtual void addPipelineElement(PipelineElement& element)
        throw (ComponentAlreadyExists);
    virtual void deletePipelineElement(PipelineElement& element);

    virtual int pipelineElementCount() const;
    virtual PipelineElement* pipelineElement(int index) const
        throw (OutOfRange);
    virtual bool hasPipelineElement(const std::string& name) const;
    virtual PipelineElement* pipelineElement(const std::string& name) const
        throw (InstanceNotFound);

    virtual AddressSpace* addressSpace() const;
    virtual void setAddressSpace(AddressSpace* as)
        throw (IllegalRegistration);
    virtual bool hasAddressSpace() const;

    virtual void cleanup(const std::string& resource);

    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual bool isArchitectureEqual(
        const FunctionUnit* fu, const bool checkPortWidths = true) const;

    bool needsConflictDetection() const;

    /// ObjectState name for function unit.
    static const std::string OSNAME_FU;
    /// ObjectState attribute key for name of the address space.
    static const std::string OSKEY_AS;

protected:
    void unsetMachineDerived();

private:
    /// Copying forbidden.
    FunctionUnit(const FunctionUnit&);
    /// Container for operations.
    typedef std::vector<HWOperation*> OperationTable;
    /// Container for pipeline elements.
    typedef std::vector<PipelineElement*> PipelineElementTable;

    void cleanupGuards() const;
    void loadStateWithoutReferences(const ObjectState* fuState)
        throw (ObjectStateLoadingException);
    void deleteAllOperations();

    /// Contains all the operations of the function unit.
    OperationTable operations_;

    /// Contains all the pipeline elements of the function unit.
    PipelineElementTable pipelineElements_;

    /// Address space used by the function unit.
    AddressSpace* addressSpace_;
};
}

#endif
