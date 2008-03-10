/**
 * @file LongImmediateRegisterState.cc
 *
 * Definition of LongImmediateRegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "LongImmediateRegisterState.hh"
#include "LongImmediateUnitState.hh"
#include "SimValue.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent unit of the register.
 * @param index Index of the register.
 * @param width The bit width of the register.
 * @param signExtend True in case written value should be sign extended 
 *                   (default is zero extension).
 */
LongImmediateRegisterState::LongImmediateRegisterState(
    LongImmediateUnitState* parent, int index, int width, bool signExtend) : 
    StateData(), parent_(parent), index_(index), width_(width), 
    signExtend_(signExtend) {
}

/**
 * Destructor.
 */
LongImmediateRegisterState::~LongImmediateRegisterState() {
}

/**
 * Sets the value of the register.
 *
 * Also extends the value if it's narrower than the register width.
 *
 * @param value New value.
 */
void
LongImmediateRegisterState::setValue(const SimValue& value) {
    // The value is extended in case it's smaller than WORD_BITWIDTH,
    // which is the width of (int) otherwise we won't extend, as it's
    // probably a double. In the future when we support 64 bit or larger
    // integers, this check is not needed.
    if (width_ <= WORD_BITWIDTH && value.width() < width_) {
        SimValue newValue(width_);
        UIntWord toBeExtended = value.uIntWordValue();
        int ext = width_ - value.width();

        if (signExtend_) {
            newValue = ((int)(toBeExtended << ext) >> ext);
        } else {
            // @todo: this might not be needed, we could assume the top bits 
            // to be zero already
            newValue = ((toBeExtended << ext) >> ext);
        }
        parent_->setRegisterValue(index_, newValue);
    } else {
        parent_->setRegisterValue(index_, value);
    }
}

/**
 * Returns the register value.
 *
 * @return Register value.
 */
const SimValue&
LongImmediateRegisterState::value() const {
    return parent_->registerValue(index_);
}

