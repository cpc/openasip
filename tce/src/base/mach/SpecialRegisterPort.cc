/**
 * @file SpecialRegisterPort.cc
 *
 * Implementation of SpecialRegisterPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "SpecialRegisterPort.hh"
#include "ControlUnit.hh"

using std::string;

namespace TTAMachine {

const std::string SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT = "sr_port";

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name or another port
 *                                   that sets operation code.
 */
SpecialRegisterPort::SpecialRegisterPort(
    const std::string& name,
    int width,
    ControlUnit& parent)
    throw (InvalidName, OutOfRange, ComponentAlreadyExists) :
    BaseFUPort(name, width, parent) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent unit of the port.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
SpecialRegisterPort::SpecialRegisterPort(
    const ObjectState* state,
    Unit& parent)
    throw (ObjectStateLoadingException) :
    BaseFUPort(state, parent) {
}


/**
 * The destructor.
 */
SpecialRegisterPort::~SpecialRegisterPort() {
}


/**
 * Always returns false. SpecialRegisterPort cannot be triggering.
 *
 * @return false
 */
bool
SpecialRegisterPort::isTriggering() const {
    return false;
}


/**
 * Always returns false. SpecialRegisterPort cannot be operation code
 * setting.
 *
 * @return false
 */
bool
SpecialRegisterPort::isOpcodeSetting() const {
    return false;
}


/**
 * Saves the state of the object to an ObjectState tree and returns it.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
SpecialRegisterPort::saveState() const {
    ObjectState* state = BaseFUPort::saveState();
    state->setName(OSNAME_SPECIAL_REG_PORT);
    return state;
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state An ObjectState instance representing state of a
 *              SpecialRegisterPort.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
SpecialRegisterPort::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    if (state->name() != OSNAME_SPECIAL_REG_PORT) {
        const string procName = "SpecialRegisterPort::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    BaseFUPort::loadState(state);
}

}
