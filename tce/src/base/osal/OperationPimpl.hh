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
 * @file OperationPimpl.hh
 *
 * Declaration of OperationPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_PIMPL_HH
#define OPERATION_PIMPL_HH

#include <string>
#include <set>
#include <vector>

class Operation;
class OperationBehavior;

/**
 * A private implementation class for Operation
 */
class OperationPimpl {
public:    
    friend class Operation;
    ~OperationPimpl();
    
private:
    OperationPimpl(
        const TCEString& name, 
        OperationBehavior& behavior);
    OperationPimpl();
    
    
    /// Copying not allowed.
    OperationPimpl(const OperationPimpl&);
    /// Assignment not allowed.
    OperationPimpl& operator=(const OperationPimpl&);
    
    TCEString name() const;
    TCEString description() const;
    
    void addDag(const TCEString& code);
    void removeDag(int index);
    int dagCount() const;
    OperationDAG& dag(int index) const;
    TCEString dagCode(int index) const;
    void setDagCode(int index, const TCEString& code);
    TCEString dagError(int index) const;

    int numberOfInputs() const;
    int numberOfOutputs() const;
    bool usesMemory() const;
    bool readsMemory() const;
    bool writesMemory() const;
    bool canTrap() const;
    bool hasSideEffects() const;
    bool isClocked() const;
    bool isControlFlowOperation() const;
    bool dependsOn(const Operation& op) const;
    int affectsCount() const;
    int affectedByCount() const;
    TCEString affects(unsigned int i) const;
    TCEString affectedBy(unsigned int i) const;
    bool canSwap(int id1, int id2) const;

    Operand& input(int index) const;
    Operand& output(int index) const;
    Operand& operand(int id) const;

    void setBehavior(OperationBehavior& behavior);
    OperationBehavior& behavior() const;

    void loadState(const ObjectState* state);
    ObjectState* saveState() const;

    bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;

    void createState(OperationContext& context) const;
    void deleteState(OperationContext& context) const;

    bool canBeSimulated() const;

    bool isNull() const;

    TCEString emulationFunctionName() const;
    
    /**
     * Internal container for information of one DAG.
     */
    struct OperationDAGInfo { 
        /// Source code for creating DAG for operation.
        std::string code;
        /// Error message if creating DAG failed.
        std::string error;
        /// If code was already tried to compile and it did not succeed.
        bool compilationFailed;
        /// DAG presentation of code. set to 
        /// NullOperationDAG if could not be created.
        OperationDAG* dag;
      
    };

    typedef std::vector<OperationDAGInfo> DAGContainer;

    void clear();

    Operand& fetchOperand(int id, const std::vector<Operand*>& ops) const;
    Operand& fetchOperand(int id) const;
    void insertOperand(Operand* operand, std::vector<Operand*>& ops);

    /// The behavior of the Operation.
    OperationBehavior* behavior_;
    /// Name of the Operation.
    std::string name_;
    /// Description of the Operation.
    std::string description_;
    /// Table of DAGs and their source codes of an operation.
    mutable DAGContainer dags_;
   
    /// The number of inputs of the Operation.
    int inputs_;
    /// The number of outputs of the Operation.
    int outputs_;
    /// Flag indicating if Operation reads from memory.
    bool readsMemory_;
    /// Flag indicating if Operation writes to memory.
    bool writesMemory_;
    /// Flag indicating if Operation can trap.
    bool canTrap_;
    /// Flag indicating if Operation has side effects.
    bool hasSideEffects_;
    /// Flag indicating if Operation is clocked and needs AdvanceClock.
    bool isClocked_;
    /// Flag indicating if the Operation can change program flow.
    bool controlFlowOperation_;
    /// Operations that affects this Operation.
    std::set<std::string> affects_;
    /// Operations that are affected by this Operation.
    std::set<std::string> affectedBy_;
    /// Input Operands of the Operation.
    std::vector<Operand*> inputOperands_;
    /// Output Operands of the Operation.
    std::vector<Operand*> outputOperands_;

};

#endif

