/**
 * @file IUPortCode.cc
 *
 * Implementation of IUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "IUPortCode.hh"
#include "SocketCodeTable.hh"
#include "ObjectState.hh"

using std::string;

const std::string IUPortCode::OSNAME_IU_PORT_CODE = "iu_port_code";

/**
 * The constructor.
 *
 * Registers the created instance to the given socket code table
 * automatically.
 *
 * @param immediateUnit Name of the immediate unit.
 * @param encoding The encoding for the register file port.
 * @param extraBits The number of extra zero bits in the encoding.
 * @param indexWidth The number of bits reserved for the register index.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 * @exception OutOfRange If some of the given values is out of range.
 */
IUPortCode::IUPortCode(
    const std::string& immediateUnit,
    unsigned int encoding,
    unsigned int extraBits,
    int indexWidth,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(immediateUnit, encoding, extraBits, indexWidth) {

    setParent(NULL);
    parent.addIUPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Creates an IU port code without port encoding. That is, the port
 * code consists of mere register index. This is useful if the parent
 * socket code table does not contain other port codes. Registers the
 * created instance to the given socket code table automatically.
 *
 * @param immediateUnit Name of the immediate unit.
 * @param indexWidth The number of bits reserved for the register index.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 * @exception OutOfRange If some of the given values is out of range.
 */
IUPortCode::IUPortCode(
    const std::string& immediateUnit,
    int indexWidth,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(immediateUnit, indexWidth) {

    setParent(NULL);
    parent.addIUPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 */
IUPortCode::IUPortCode(const ObjectState* state, SocketCodeTable& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    PortCode(state) {

    if (state->name() != OSNAME_IU_PORT_CODE) {
	const string procName = "IUPortCode::IUPortCode";
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    setParent(NULL);
    parent.addIUPortCode(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
IUPortCode::~IUPortCode() {
    SocketCodeTable* parent = this->parent();
    setParent(NULL);
    parent->removeIUPortCode(*this);
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
IUPortCode::saveState() const {
    ObjectState* state = PortCode::saveState();
    state->setName(OSNAME_IU_PORT_CODE);
    return state;
}
