/**
 * @file BusState.cc
 *
 * Definition of BusState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BusState.hh"
#include "Application.hh"
#include "SimValue.hh"
#include "BaseType.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// BusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param width Bit width of the bus.
 */
BusState::BusState(int width) : 
    RegisterState(width), squashed_(false) {
}

/**
 * Destructor.
 */
BusState::~BusState() {
}

/**
 * Sets the value of the bus.
 *
 * Does not extend the value.
 *
 * @param value New value.
 */
void
BusState::setValue(const SimValue& value) {
    RegisterState::setValue(value);
}

/**
 * Sets whether the last executed move scheduled to this bus was squashed.
 *
 * @param isSquashed True in case this bus was squashed.
 */
void 
BusState::setSquashed(bool isSquashed) {
    squashed_ = isSquashed;
}

/**
 * Returns true if the last executed move scheduled to this bus was squashed.
 *
 * @param isSquashed True in case this bus was squashed.
 */
bool 
BusState::isSquashed() const {
    return squashed_;
}

//////////////////////////////////////////////////////////////////////////////
// NullBusState
//////////////////////////////////////////////////////////////////////////////

NullBusState* NullBusState::instance_ = NULL;

/**
 * Returns the instance of NullBusState.
 *
 * @return The instance of NullBusState.
 */
NullBusState&
NullBusState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullBusState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullBusState::NullBusState() : BusState(0) {
}

/**
 * Destructor.
 */
NullBusState::~NullBusState() {
}

/**
 * Aborts the program with error message.
 */
void
NullBusState::setValue(const SimValue&) {
    Application::abortWithError("setValue()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
const SimValue&
NullBusState::value() const {
    Application::abortWithError("value()");
    return NullSimValue::instance();
}
