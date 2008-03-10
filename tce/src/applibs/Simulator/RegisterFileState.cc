/**
 * @file RegisterFileState.cc
 *
 * Definition of RegisterFileState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "RegisterFileState.hh"
#include "RegisterState.hh"
#include "SequenceTools.hh"
#include "Application.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// RegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param size The size of the RegisterFileState.
 * @param width Width of the registers.
 */
RegisterFileState::RegisterFileState(int size, int width) {
    for (int i = 0; i < size; i++) {
        registerStates_.push_back(new RegisterState(width));
    }
}

/**
 * Destructor.
 */
RegisterFileState::~RegisterFileState() {
    SequenceTools::deleteAllItems(registerStates_);
}

/**
 * Returns RegisterState with a given index.
 *
 * @param index Index of the wanted RegisterState.
 * @return RegisterState with a given index.
 * @exception OutOfRange If index is out of range.
 */
RegisterState&
RegisterFileState::registerState(int index)
    throw (OutOfRange) {

    if (index < 0 || index > static_cast<int>(registerStates_.size()) - 1) {
        string msg = "Register index out of range";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return *registerStates_[index];
}

/**
 * Returns the count of registers in the register file.
 *
 * @return Count of registers.
 */
std::size_t 
RegisterFileState::registerCount() const {
    return registerStates_.size();
}

//////////////////////////////////////////////////////////////////////////////
// NullRegisterFileState
//////////////////////////////////////////////////////////////////////////////

NullRegisterFileState* NullRegisterFileState::instance_ = NULL;

/**
 * Returns the instance of NullRegisterFileState.
 *
 * @return The instance of NullRegisterFileState.
 */
NullRegisterFileState&
NullRegisterFileState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullRegisterFileState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullRegisterFileState::NullRegisterFileState() : RegisterFileState(0, 0) {
}

/**
 * Destructor.
 */
NullRegisterFileState::~NullRegisterFileState() {
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never throws.
 */
RegisterState&
NullRegisterFileState::registerState(int)
    throw (OutOfRange) {

    Application::abortWithError("registerState()");
    return NullRegisterState::instance();
}
