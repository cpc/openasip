/**
 * @file FUGuardEncoding.cc
 *
 * Implementation of FUGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUGuardEncoding.hh"
#include "GuardField.hh"

using std::string;

const std::string FUGuardEncoding::OSNAME_FU_GUARD_ENCODING = "fu_guard_enc";
const std::string FUGuardEncoding::OSKEY_FU_NAME = "fu_name";
const std::string FUGuardEncoding::OSKEY_PORT_NAME = "port_name";

/**
 * The constructor.
 *
 * Creates a guard encoding and registers it into the given guard field. The
 * guard expression is identified by the name of function unit and port, and
 * the "invert" flag, all given as parameters.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param inverted The "invert" flag.
 * @param encoding The control code of the guard expression.
 * @param parent The parent guard field.
 * @exception ObjectAlreadyExists If the guard expression is already encoded
 *                                in the parent field, or if the given control
 *                                code is already assigned to another guard
 *                                expression.
 */
FUGuardEncoding::FUGuardEncoding(
    const std::string& fu,
    const std::string& port,
    bool inverted,
    unsigned int encoding,
    GuardField& parent)
    throw (ObjectAlreadyExists) :
    GuardEncoding(inverted, encoding), functionUnit_(fu), port_(port) {

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent guard field.
 * @exception ObjectStateLoadingException If an error occurs while loading the
 *                                        state.
 * @exception ObjectAlreadyExists If the guard expression if already encoded
 *                                in the parent field, or if the given control
 *                                code is already assigned to another guard
 *                                expression.
 */
FUGuardEncoding::FUGuardEncoding(const ObjectState* state, GuardField& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    GuardEncoding(state), functionUnit_(""), port_("") {

    const string procName = "FUGuardEncoding::FUGuardEncoding";

    if (state->name() != OSNAME_FU_GUARD_ENCODING) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
	functionUnit_ = state->stringAttribute(OSKEY_FU_NAME);
	port_ = state->stringAttribute(OSKEY_PORT_NAME);
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
FUGuardEncoding::~FUGuardEncoding() {
    GuardField* parent = this->parent();
    setParent(NULL);
    parent->removeGuardEncoding(*this);
}


/**
 * Returns the name of the function unit that contains the port of this guard
 * expression.
 *
 * @return The name of the function unit.
 */
std::string
FUGuardEncoding::functionUnit() const {
    return functionUnit_;
}


/**
 * Returns the name of the FU port of this guard expression.
 *
 * @return The name of the FU port.
 */
std::string
FUGuardEncoding::port() const {
    return port_;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
FUGuardEncoding::saveState() const {
    ObjectState* state = GuardEncoding::saveState();
    state->setName(OSNAME_FU_GUARD_ENCODING);
    state->setAttribute(OSKEY_FU_NAME, functionUnit());
    state->setAttribute(OSKEY_PORT_NAME, port());
    return state;
}
