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
#include "BaseFUPort.hh"
#include "CIStringSet.hh"

class ObjectState;

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
    FunctionUnit(const std::string& name);
    FunctionUnit(const ObjectState* state);
    virtual ~FunctionUnit();

    virtual FunctionUnit* copy() const;

    virtual void setName(const std::string& name);

    virtual BaseFUPort* port(const std::string& name) const;
    virtual BaseFUPort* port(int index) const;
    virtual int operationPortCount() const;
    virtual bool hasOperationPort(const std::string& name) const;
    virtual FUPort* operationPort(const std::string& name) const;
    virtual FUPort* operationPort(int index) const;
    virtual BaseFUPort* triggerPort() const;

    virtual void addOperation(HWOperation& operation);
    virtual void deleteOperation(HWOperation& operation);

    virtual bool hasOperation(const std::string& name) const;
    virtual bool hasOperationLowercase(const std::string& name) const;

    virtual HWOperation* operation(const std::string& name) const;
    virtual HWOperation* operationLowercase(const std::string& name) const;

    virtual HWOperation* operation(int index) const;
    virtual int operationCount() const;

    virtual void operationNames(TCETools::CIStringSet& opNames) const;

    virtual int maxLatency() const;

    virtual void addPipelineElement(PipelineElement& element);
    virtual void deletePipelineElement(PipelineElement& element);

    virtual int pipelineElementCount() const;
    virtual PipelineElement* pipelineElement(int index) const;
    virtual bool hasPipelineElement(const std::string& name) const;
    virtual PipelineElement* pipelineElement(const std::string& name) const;

    virtual AddressSpace* addressSpace() const;
    virtual void setAddressSpace(AddressSpace* as);
    virtual bool hasAddressSpace() const;

    virtual void cleanup(const std::string& resource);

    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

    virtual bool isArchitectureEqual(
        const FunctionUnit* fu, const bool checkPortWidths = true) const;

    bool needsConflictDetection() const;

    virtual int orderNumber() const;
    virtual void setOrderNumber(int);

    /// ObjectState name for function unit.
    static const std::string OSNAME_FU;
    /// ObjectState attribute key for name of the address space.
    static const std::string OSKEY_AS;
    /// ObjectState attribute key for FU order number name.
    static const std::string OSKEY_ORDER_NUMBER;

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
    void loadStateWithoutReferences(const ObjectState* fuState);
    void deleteAllOperations();

    /// Contains all the operations of the function unit.
    OperationTable operations_;

    /// Contains all the pipeline elements of the function unit.
    PipelineElementTable pipelineElements_;

    /// Address space used by the function unit.
    AddressSpace* addressSpace_;
    
    /// Number indicating possition of the FU in the ADF file. Alows
    /// putting the FU into the order in the instruction.
    int orderNumber_;
};
}

#endif
