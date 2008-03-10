/**
 * @file SmartHWOperation.hh
 *
 * Declaration of SmartHWOperation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_SMART_HW_OPERATION_HH
#define TTA_SMART_HW_OPERATION_HH

#include "HWOperation.hh"

class Operation;
class UniversalFunctionUnit;

/**
 * Represents an operation in a function unit. The smart operation
 * automatically creates operand bindings on demand. This kind of
 * operations are used in UniversalFunctionUnit. The last operand
 * requested is bound to the operation code setting port.
 */
class SmartHWOperation : public TTAMachine::HWOperation {
public:
    SmartHWOperation(
        const Operation& operation,
        UniversalFunctionUnit& parent)
        throw (ComponentAlreadyExists, InvalidName);
    virtual ~SmartHWOperation();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    TTAMachine::FUPort* port(int operand) const;

    UniversalFunctionUnit* parentUnit() const;

    virtual void bindPort(int operand, const TTAMachine::FUPort& port)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange);
    virtual void unbindPort(const TTAMachine::FUPort& port);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    bool otherMandatoryInputsBound(int operand) const;

    /// The operation represented by this SmartHWOperation instance.
    const Operation& operation_;
    /// Tells whether this operation has 32 bits wide operands.
    bool is32BitOperation_;
};

#endif
