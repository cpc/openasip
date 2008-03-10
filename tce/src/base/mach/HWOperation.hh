/**
 * @file HWOperation.hh
 *
 * Declaration of HWOperation class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_HW_OPERATION_HH
#define TTA_HW_OPERATION_HH

#include <string>
#include <map>

#include "MachinePart.hh"
#include "ObjectState.hh"

class Operand;

namespace TTAMachine {

class FunctionUnit;
class ExecutionPipeline;
class FUPort;

/**
 * Represents an operation of the function unit.
 */
class HWOperation : public SubComponent {
public:
    HWOperation(const std::string& name, FunctionUnit& parent)
        throw (ComponentAlreadyExists, InvalidName);
    HWOperation(const ObjectState* state, FunctionUnit& parent)
        throw (ObjectStateLoadingException);
    ~HWOperation();

    std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    FunctionUnit* parentUnit() const;
    ExecutionPipeline* pipeline() const;

    int latency() const;
    int latency(int output) const
        throw (IllegalParameters);
    int slack(int input) const
        throw (IllegalParameters);

    virtual void bindPort(int operand, const FUPort& port)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange);
    virtual void unbindPort(const FUPort& port);
    virtual FUPort* port(int operand) const;
    bool isBound(const FUPort& port) const;
    int io(const FUPort& port) const
        throw (InstanceNotFound);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for HWOperation.
    static const std::string OSNAME_OPERATION;
    /// ObjectState attribute key for name of the operation.
    static const std::string OSKEY_NAME;
    /// ObjectState name for an operand binding.
    static const std::string OSNAME_OPERAND_BINDING;
    /// ObjectState attribute key for operand index.
    static const std::string OSKEY_OPERAND;
    /// ObjectState attribute key for port name.
    static const std::string OSKEY_PORT;

private:
    /// Map for mapping operand indexes to FUPorts.
    typedef std::map<int, const FUPort*> OperandBindingMap;

    /// Name of the operation.
    std::string name_;
    /// Pipeline of the operation.
    ExecutionPipeline* pipeline_;
    /// The parent unit.
    FunctionUnit* parent_;
    /// Maps operands of operation to particular ports of the parent unit.
    OperandBindingMap operandBinding_;

};
}

#endif
