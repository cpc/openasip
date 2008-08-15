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
 * @file Operation.hh
 *
 * Declaration of Operation class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#ifndef TTA_OPERATION_HH
#define TTA_OPERATION_HH

#include "Serializable.hh"
#include "OperationBehavior.hh"

class OperationDAG;
class TCEString;
class OperationPimpl;
class Operand;

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
    virtual bool usesMemory() const;
    virtual bool readsMemory() const;
    virtual bool writesMemory() const;
    virtual bool canTrap() const;
    virtual bool hasSideEffects() const;
    virtual bool isClocked() const;
    virtual bool isControlFlowOperation() const;
    virtual bool dependsOn(const Operation& op) const;
    virtual int affectsCount() const;
    virtual int affectedByCount() const;
    virtual TCEString affects(unsigned int i) const;
    virtual TCEString affectedBy(unsigned int i) const;
    virtual bool canSwap(int id1, int id2) const;

    virtual Operand& input(int index) const;
    virtual Operand& output(int index) const;
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
