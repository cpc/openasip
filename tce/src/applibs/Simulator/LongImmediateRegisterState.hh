/**
 * @file LongImmediateRegisterState.hh
 *
 * Declaration of LongImmediateRegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMMEDIATE_REGISTER_STATE_HH
#define TTA_LONG_IMMEDIATE_REGISTER_STATE_HH

#include <string>

#include "StateData.hh"

class LongImmediateUnitState;
class SimValue;

/**
 * Represents a register of a long immediate unit.
 */
class LongImmediateRegisterState : public StateData {
public:
    LongImmediateRegisterState(
        LongImmediateUnitState* parent, 
        int index,
        int width,
        bool signExtend);
    virtual ~LongImmediateRegisterState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    /// Copying not allowed.
    LongImmediateRegisterState(const LongImmediateRegisterState&);
    /// Assignment not allowed.
    LongImmediateRegisterState& operator=(const LongImmediateRegisterState&);

    /// Parent unit of the register state.
    LongImmediateUnitState* parent_;
    /// Index of the register.
    int index_;
    /// The bit width of the register (needed only while extending).
    int width_;
    /// True in case the written values should be sign extended.
    bool signExtend_;
};

#endif
