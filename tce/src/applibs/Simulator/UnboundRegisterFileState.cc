/**
 * @file UnboundRegisterFileState.cc
 *
 * Definition of UnboundRegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "UnboundRegisterFileState.hh"
#include "RegisterState.hh"
#include "SequenceTools.hh"

/**
 * Constructor.
 */
UnboundRegisterFileState::UnboundRegisterFileState() : 
    RegisterFileState(-1, 32) {
}

/**
 * Destructor.
 */
UnboundRegisterFileState::~UnboundRegisterFileState() {
    SequenceTools::deleteAllItems(registerStates_);
}

/**
 * Returns a register with a given index.
 *
 * @param index The index of the wanted RegisterState.
 * @return RegisterState with a given index.
 * @exception OutOfRange Never throws.
 */
RegisterState&
UnboundRegisterFileState::registerState(int index) 
    throw (OutOfRange) {

    if (index > static_cast<int>(registerStates_.size()) - 1) {
        registerStates_.resize(index + 1, NULL);
    }

    if (registerStates_[index] == NULL) {
        registerStates_[index] = new RegisterState(32);
    }

    return *registerStates_[index];
}

/**
 * Returns the count of registers in the register file.
 *
 * @return Count of registers.
 */
std::size_t 
UnboundRegisterFileState::registerCount() const {
    return registerStates_.size();
}

/**
 * Returns true if the register with given index is in use.
 *
 * Register is considered to be in use when it's been written to,
 * that is, the SimValue storing its value has isActive() == true.
 */
bool
UnboundRegisterFileState::isRegisterInUse(std::size_t index) const {
    return registerStates_[index] != NULL &&
        registerStates_[index]->value().isActive();
}
