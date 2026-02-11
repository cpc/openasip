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
 * @file OperationPimpl.hh
 *
 * Declaration of OperationPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_PIMPL_HH
#define OPERATION_PIMPL_HH

#include <string>
#include <set>
#include <vector>
#include <map>

#include "TCEString.hh"

class Operation;
class OperationDAG;
class OperationBehavior;
class Operand;
class ObjectState;
class SimValue;
class OperationContext;

/**
 * A private implementation class for Operation
 */
class OperationPimpl {
public:
    friend class Operation;
    ~OperationPimpl();

    inline int numberOfInputs() const;
    inline int numberOfOutputs() const;
    TCEString name() const;
    void setCall(bool setting);
    void setBranch(bool setting);
    void setControlFlowOperation(bool setting);

    Operand& operand(int id) const;

    OperationPimpl(const OperationPimpl&) = delete;
    OperationPimpl& operator=(const OperationPimpl&) = delete;

private:
    OperationPimpl(const TCEString& name, OperationBehavior& behavior);
    OperationPimpl();

    TCEString description() const;

    void addDag(const TCEString& code);
    void removeDag(int index);
    int dagCount() const;
    OperationDAG& dag(int index) const;
    TCEString dagCode(int index) const;
    void setDagCode(int index, const TCEString& code);
    TCEString dagError(int index) const;

    inline bool usesMemory() const;
    inline bool readsMemory() const;
    inline bool writesMemory() const;
    bool canTrap() const;
    bool hasSideEffects() const;
    bool isClocked() const;
    bool isControlFlowOperation() const;
    bool isCall() const;
    bool isBranch() const;
    bool dependsOn(const Operation& op) const;
    void setReadsMemory(bool setting);
    void setWritesMemory(bool setting);
    int affectsCount() const;
    int affectedByCount() const;
    TCEString affects(unsigned int i) const;
    TCEString affectedBy(unsigned int i) const;
    bool canSwap(int id1, int id2) const;

    inline Operand& input(int index) const;
    void addInput(Operand* operand);
    Operand& output(int index) const;
    void addOutput(Operand* operand);

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
    /// Flag indicating if Operation is call.
    bool isCall_;
    /// Flag indicating if Operation is branch changing control flow.
    bool isBranch_;

};

#include "OperationPimpl.icc"

#endif

