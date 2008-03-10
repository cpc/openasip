/**
 * @file LongImmUpdateAction.hh
 *
 * Declaration of LongImmUpdateAction class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMM_UPDATE_ACTION_HH
#define TTA_LONG_IMM_UPDATE_ACTION_HH

#include "SimValue.hh"
class LongImmediateRegisterState;

/**
 * Updates the value of the long immediate register state.
 */
class LongImmUpdateAction {
public:
    LongImmUpdateAction(SimValue value, LongImmediateRegisterState& state);
    virtual ~LongImmUpdateAction();

    void execute();

private:
    /// Copying not allowed.
    LongImmUpdateAction(const LongImmUpdateAction&);
    /// Assignment not allowed.
    LongImmUpdateAction& operator=(const LongImmUpdateAction&);

    /// Value to be updated.
    SimValue value_;
    /// Destination register.
    LongImmediateRegisterState& state_;
};

#endif
