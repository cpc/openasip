/**
 * @file OpcodeSettingVirtualInputPortState.hh
 *
 * Declaration of OpcodeSettingVirtualInputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi) 
 * @note rating: red
 */

#ifndef TTA_OPCODE_SETTING_VIRTUAL_INPUT_PORT_STATE_HH
#define TTA_OPCODE_SETTING_VIRTUAL_INPUT_PORT_STATE_HH

#include <string>

#include "InputPortState.hh"

class Operation;
class FUState;
class OperationExecutor;

/**
 * Operation setting input port state.
 *
 * Port is virtual, that is it doesn't model real input port state.
 * It has a pointer to the real port in which value is really set.
 */
class OpcodeSettingVirtualInputPortState : public InputPortState {
public:
    OpcodeSettingVirtualInputPortState(
        Operation& operation, 
        OperationExecutor& executor,
        FUState& parent, 
        InputPortState& real);

    OpcodeSettingVirtualInputPortState(
        Operation& operation, 
        FUState& parent, 
        InputPortState& real);

    virtual ~OpcodeSettingVirtualInputPortState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

    InputPortState* realPort() const;

private:
    /// Copying not allowed.
    OpcodeSettingVirtualInputPortState(
        const OpcodeSettingVirtualInputPortState&);
    /// Assignment not allowed.
    OpcodeSettingVirtualInputPortState& operator=(
        const OpcodeSettingVirtualInputPortState&);
    /// Operation of the port.
    Operation& operation_;
    /// Operation executor used to execute the operation in the target FU,
    /// this is an optimization to avoid searching for the executor every
    /// time the operation is triggered.
    OperationExecutor* executor_;
    /// Real input port.
    InputPortState& real_;
};

#endif
