/**
 * @file RFPort.cc
 *
 * Implementation of RFPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFPort.hh"
#include "BaseRegisterFile.hh"

using std::string;

namespace TTAMachine {

const string RFPort::OSNAME_RFPORT = "rf_port";

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param parent The parent register file of the port.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception ComponentAlreadyExists If there is another port with the same
 *                                   name in the parent unit.
 */
RFPort::RFPort(const std::string& name, BaseRegisterFile& parent)
    throw (InvalidName, ComponentAlreadyExists) :
    Port(name, parent) {
}


/**
 * The constuctor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent unit of the port.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
RFPort::RFPort(const ObjectState* state, Unit& parent)
    throw (ObjectStateLoadingException) : Port(state, parent) {
}


/**
 * The destructor.
 */
RFPort::~RFPort() {
}


/**
 * Returns the bit width of the port.
 *
 * @return The bit width of the port.
 */
int
RFPort::width() const {
    return parentUnit()->width();
}


/**
 * Returns the parent register file.
 *
 * @return The parent register file.
 */
BaseRegisterFile*
RFPort::parentUnit() const {
    Unit* parent = Port::parentUnit();
    BaseRegisterFile* rfParent = dynamic_cast<BaseRegisterFile*>(parent);
    assert(rfParent != NULL);
    return rfParent;
}


/**
 * Saves the state of the object to an ObjectState tree and returns it.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
RFPort::saveState() const {
    ObjectState* state = Port::saveState();
    state->setName(OSNAME_RFPORT);
    return state;
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state An ObjectState instance representing state of an RFPort.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
RFPort::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    if (state->name() != OSNAME_RFPORT) {
        const string procName = "RFPort::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Port::loadState(state);
}

}
