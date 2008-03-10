/**
 * @file UniversalFunctionUnit.hh
 *
 * Declaration of UniversalFunctionUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
