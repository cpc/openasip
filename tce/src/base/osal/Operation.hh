/**
 * @file Operation.hh
 *
 * Declaration of Operation class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#ifndef TTA_OPERATION_HH
#define TTA_OPERATION_HH

#include <string>
#include <vector>
#include <set>

#include "Serializable.hh"
#include "Exception.hh"
#include "OperationBehavior.hh"
#include "Operand.hh"

class OperationDAG;

/**
 * Class that models the static properties and the behavior of operations
 * of the target processor.
 */
class Operation : public Serializable {
public:  
    /// Object state name for operation.
    static const std::string OPRN_OPERATION;
    /// Object state name for name.
    static const std::string OPRN_NAME;
    /// Object state name for description.
    static const std::string OPRN_DESCRIPTION;
    /// Object state name for inputs.
    static const std::string OPRN_INPUTS;
    /// Object state name for outputs.
    static const std::string OPRN_OUTPUTS;
    /// Object state name for trap.
    static const std::string OPRN_TRAP;
    /// Object state name for side effects.
    static const std::string OPRN_SIDE_EFFECTS;
    /// Object state name for control flow property.
    static const std::string OPRN_CONTROL_FLOW;
    /// Object state name for reads memory.
    static const std::string OPRN_READS_MEMORY;
    /// Object state name for writes memory.
    static const std::string OPRN_WRITES_MEMORY;
    /// Object state name for affects.
    static const std::string OPRN_AFFECTS;
    /// Object state name for affected by.
    static const std::string OPRN_AFFECTED_BY;
    /// Object state name for input operand.
    static const std::string OPRN_IN;
    /// Object state name for output operand.
    static const std::string OPRN_OUT;
    /// Object state name for trigger semantics.
    static const std::string OPRN_TRIGGER;

    Operation(const std::string& name, OperationBehavior& behavior);
    virtual ~Operation();

    virtual std::string name() const;
    virtual std::string description() const;
    
    virtual void addDag(std::string code);
    virtual void removeDag(int index);
    virtual int dagCount() const;
    virtual OperationDAG& dag(int index) const;
    virtual std::string dagCode(int index) const;
    virtual void setDagCode(int index, std::string& code);
    virtual std::string dagError(int index) const;

    virtual int numberOfInputs() const;
    virtual int numberOfOutputs() const;
    virtual bool usesMemory() const;
    virtual bool readsMemory() const;
    virtual bool writesMemory() const;
    virtual bool canTrap() const;
    virtual bool hasSideEffects() const;
    virtual bool isControlFlowOperation() const;
    virtual bool dependsOn(const Operation& op) const;
    virtual int affectsCount() const;
    virtual int affectedByCount() const;
    virtual std::string affects(unsigned int i) const
        throw (OutOfRange);
    virtual std::string affectedBy(unsigned int i) const
        throw (OutOfRange);
    virtual bool canSwap(int id1, int id2) const;

    virtual Operand& input(int id) const;
    virtual Operand& output(int id) const;
    virtual Operand& operand(int id) const;

    virtual void setBehavior(OperationBehavior& behavior);
    virtual OperationBehavior& behavior() const;

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;

    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

    virtual bool canBeSimulated() const;

    bool isNull() const;

    std::string emulationFunctionName() const;
    
private:
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
	
    /// Copying not allowed.
    Operation(const Operation& operation);
    /// Assignment not allowed.
    Operation& operator=(const Operation&);

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

    virtual std::string name() const;
    virtual std::string description() const;
    virtual int numberOfInputs() const;
    virtual int numberOfOutputs() const;
    virtual bool usesMemory() const;
    virtual bool readsMemory() const;
    virtual bool writesMemory() const;
    virtual bool canTrap() const;
    virtual bool hasSideEffects() const;
    virtual bool isControlFlowOperation() const;
    virtual bool dependsOn(const Operation& op) const;
    virtual int affectsCount() const;
    virtual int affectedByCount() const;
    virtual std::string affects(unsigned int i) const
        throw (OutOfRange);
    virtual std::string affectedBy(unsigned int i) const
        throw (OutOfRange);
    virtual bool canSwap(int id1, int id2) const;

    virtual Operand& input(int id) const;
    virtual Operand& output(int id) const;
    virtual Operand& operand(int id) const;

    virtual void setBehavior(OperationBehavior& behavior);
    virtual OperationBehavior& behavior() const;
    
    virtual bool simulateTrigger(
        SimValue**,
        OperationContext& context) const
        throw (Exception);

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
