/**
 * @file RegisterState.cc
 *
 * Definition of RegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "RegisterState.hh"
#include "Application.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// RegisterState
//////////////////////////////////////////////////////////////////////////////


/**
 * Constructor.
 *
 * @param width The width of the register.
 */
RegisterState::RegisterState(int width) : 
    StateData(), value_(*(new SimValue(width))), shared_(false) {
}

/**
 * Constructor for RegisterState which shares the actual register storage.
 *
 * @param sharedRegister The register which is shared with this.
 */
RegisterState::RegisterState(SimValue& sharedRegister) : 
    StateData(), value_(sharedRegister), shared_(true) {
}

/**
 * Destructor.
 */
RegisterState::~RegisterState() {
    if (!shared_) {
        delete &value_;
    }
}

/**
 * Sets the value for the register.
 *
 * @param value Value to be set.
 */
void
RegisterState::setValue(const SimValue& value) {
    value_ = value;
    value_.setActive();
}

/**
 * Returns the value of the register.
 *
 * @return The value of the register.
 */
const SimValue&
RegisterState::value() const {
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// NullRegisterState
//////////////////////////////////////////////////////////////////////////////

NullRegisterState* NullRegisterState::instance_ = NULL;

/**
 * Returns instance of NullRegisterState.
 *
 * @return The instance of NullRegisterState.
 */
NullRegisterState&
NullRegisterState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullRegisterState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullRegisterState::NullRegisterState() : RegisterState(0) {
}

/**
 * Destructor.
 */
NullRegisterState::~NullRegisterState() {
}

/**
 * Aborts the program with error message.
 */
void
NullRegisterState::setValue(const SimValue&) {
    Application::abortWithError("setValue()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
const SimValue&
NullRegisterState::value() const {
    Application::abortWithError("value()");
    return NullSimValue::instance();
}

