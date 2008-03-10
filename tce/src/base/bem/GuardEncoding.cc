/**
 * @file GuardEncoding.cc
 *
 * Implementation of GuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <cstddef>
#include <string>

#include "GuardEncoding.hh"
#include "ObjectState.hh"

using std::string;

const std::string GuardEncoding::OSNAME_GUARD_ENCODING = "guard_encoding";
const std::string GuardEncoding::OSKEY_INVERTED = "inverted";
const std::string GuardEncoding::OSKEY_ENCODING = "encoding";

/**
 * The constructor.
 *
 * @param inverted The "invert" flag.
 * @param encoding The control code.
 */
GuardEncoding::GuardEncoding(bool inverted, unsigned int encoding) :
    inverted_(inverted), encoding_(encoding), parent_(NULL) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs while loading the
 *                                        state.
 */
GuardEncoding::GuardEncoding(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    inverted_(false), encoding_(0), parent_(NULL) {

    try {
	inverted_ = state->boolAttribute(OSKEY_INVERTED);
	encoding_ = state->intAttribute(OSKEY_ENCODING);
    } catch (const Exception& exception) {
	const string procName = "GuardEncoding::GuardEncoding";
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * The destructor
 */
GuardEncoding::~GuardEncoding() {
}


/**
 * Returns the parent guard field.
 *
 * @return The parent guard field.
 */
GuardField*
GuardEncoding::parent() const {
    return parent_;
}


/**
 * Tells whether the guard expression is inverted.
 *
 * @return True if the guard expression is inverted, otherwise false.
 */
bool
GuardEncoding::isGuardInverted() const {
    return inverted_;
}


/**
 * Returns the control code of the guard expression.
 *
 * @return The control code.
 */
unsigned int
GuardEncoding::encoding() const {
    return encoding_;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
GuardEncoding::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_GUARD_ENCODING);
    state->setAttribute(OSKEY_INVERTED, isGuardInverted());
    state->setAttribute(OSKEY_ENCODING, encoding());
    return state;
}


/**
 * Sets the parent pointer.
 *
 * @param parent The parent.
 */
void
GuardEncoding::setParent(GuardField* parent) {
    parent_ = parent;
}
