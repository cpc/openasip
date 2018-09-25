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
 * @file Operation.hh
 *
 * Declaration of Operation class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#ifndef TTA_OPERATION_HH
#define TTA_OPERATION_HH

#include "Serializable.hh"

class OperationContext;
class OperationBehavior;
class OperationDAG;
class TCEString;
class OperationPimpl;
class Operand;
class SimValue;

/**
 * Class that models the static properties and the behavior of operations
 * of the target processor.
 */
class Operation : public Serializable {
public:  
    /// Object state name for operation.
    static const char* OPRN_OPERATION;
    /// Object state name for name.
    static const char* OPRN_NAME;
    /// Object state name for description.
    static const char* OPRN_DESCRIPTION;
    /// Object state name for inputs.
    static const char* OPRN_INPUTS;
    /// Object state name for outputs.
    static const char* OPRN_OUTPUTS;
    /// Object state name for trap.
    static const char* OPRN_TRAP;
    /// Object state name for side effects.
    static const char* OPRN_SIDE_EFFECTS;
    /// Object state name for clockedness
    static const char* OPRN_CLOCKED;
    /// Object state name for control flow property.
    static const char* OPRN_CONTROL_FLOW;
    /// Object state name for reads memory.
    static const char* OPRN_READS_MEMORY;
    /// Object state name for writes memory.
    static const char* OPRN_WRITES_MEMORY;
    /// Object state name for affects.
    static const char* OPRN_AFFECTS;
    /// Object state name for affected by.
    static const char* OPRN_AFFECTED_BY;
    /// Object state name for input operand.
    static const char* OPRN_IN;
    /// Object state name for output operand.
    static const char* OPRN_OUT;
    /// Object state name for trigger semantics.
    static const char* OPRN_TRIGGER;
    /// Object state name for call property.
    static const char* OPRN_ISCALL;
    /// Object state name for branch property.
    static const char* OPRN_ISBRANCH;

    Operation(const TCEString& name, OperationBehavior& behavior);
    virtual ~Operation();

    virtual TCEString name() const;
    virtual TCEString description() const;
    
    virtual void addDag(const TCEString& code);
    virtual void removeDag(int index);
    virtual int dagCount() const;
    virtual OperationDAG& dag(int index) const;
    virtual TCEString dagCode(int index) const;
    virtual void setDagCode(int index, const TCEString& code);
    virtual TCEString dagError(int index) const;

    virtual int numberOfInputs() const;
    virtual int numberOfOutputs() const;
    virtual int operandCount() const;
    virtual bool usesMemory() const;
    virtual bool readsMemory() const;
    virtual bool writesMemory() const;
    virtual bool canTrap() const;
    virtual bool hasSideEffects() const;
    virtual bool isClocked() const;
    virtual bool isControlFlowOperation() const;
    virtual bool dependsOn(const Operation& op) const;
    virtual void setReadsMemory(bool setting);
    virtual void setWritesMemory(bool setting);
    virtual int affectsCount() const;
    virtual int affectedByCount() const;
    virtual TCEString affects(unsigned int i) const;
    virtual TCEString affectedBy(unsigned int i) const;
    virtual bool canSwap(int id1, int id2) const;
    virtual bool isCall() const;
    virtual bool isBranch() const;
    virtual bool isBaseOffsetMemOperation() const;
    virtual void setCall(bool setting);
    virtual void setBranch(bool setting);
    virtual void setControlFlowOperation(bool setting);
    
    virtual Operand& input(int index) const;
    virtual void addInput(Operand* operand);
    virtual Operand& output(int index) const;
    virtual void addOutput(Operand* operand);
    virtual Operand& operand(int id) const;

    virtual void setBehavior(OperationBehavior& behavior);
    virtual OperationBehavior& behavior() const;

    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

    virtual bool canBeSimulated() const;

    bool isNull() const;

    TCEString emulationFunctionName() const;
    
    OperationPimpl& impl() { return *pimpl_; }
private:
    /// Private implementation in a separate source file.
    OperationPimpl* pimpl_;
};

//////////////////////////////////////////////////////////////////////////////
// NullOperation
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null operation behavior.
 *
 * All methods cause program abort with an error log message.
 *
 */
class NullOperation : public Operation {
public:

    virtual ~NullOperation();

    static NullOperation& instance();

    virtual TCEString name() const;
    virtual TCEString description() const;
    virtual int numberOfInputs() const;
    virtual int numberOfOutputs() const;
    virtual bool usesMemory() const;
    virtual bool readsMemory() const;
    virtual bool writesMemory() const;
    virtual bool canTrap() const;
    virtual bool hasSideEffects() const;
    virtual bool isClocked() const;
    virtual bool isControlFlowOperation() const;
    virtual bool isCall() const;
    virtual bool isBranch() const;    
    virtual bool dependsOn(const Operation& op) const;
    virtual int affectsCount() const;
    virtual int affectedByCount() const;
    virtual TCEString affects(unsigned int i) const;
    virtual TCEString affectedBy(unsigned int i) const;
    virtual bool canSwap(int id1, int id2) const;

    virtual Operand& input(int id) const;
    virtual Operand& output(int id) const;
    virtual Operand& operand(int id) const;

    virtual void setBehavior(OperationBehavior& behavior);
    virtual OperationBehavior& behavior() const;
    
    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

protected:
    /// Some gcc versions warn about private constructors.
    NullOperation();
private:
    /// Copying not allowed.
    NullOperation(const NullOperation&);
    /// Assignment not allowed.
    NullOperation& operator=(const NullOperation&);
    
    /// Unique instance of NullOperation.
    static NullOperation instance_;
};

#include "Operation.icc"

#endif
