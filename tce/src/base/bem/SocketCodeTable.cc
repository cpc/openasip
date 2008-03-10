/**
 * @file SocketCodeTable.cc
 *
 * Implementation of SocketCodeTable class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "SocketCodeTable.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"
#include "NullFUPortCode.hh"
#include "NullRFPortCode.hh"
#include "NullIUPortCode.hh"
#include "SocketEncoding.hh"
#include "BEMTester.hh"
#include "ContainerTools.hh"
#include "SequenceTools.hh"
#include "StringTools.hh"
#include "Application.hh"

using std::string;

const std::string SocketCodeTable::OSNAME_SOCKET_CODE_TABLE = "sc_table";
const std::string SocketCodeTable::OSKEY_NAME = "name";
const std::string SocketCodeTable::OSKEY_EXTRA_BITS = "extra_bits";

/**
 * The constructor.
 *
 * Creates a code table and registers it to a binary encoding map.
 *
 * @param name Name of the table.
 * @param parent The encoding map.
 * @exception ObjectAlreadyExists If the parent encoding map already contains
 *                                a socket code table with the same name.
 */
SocketCodeTable::SocketCodeTable(
    const std::string& name,
    BinaryEncoding& parent)
    throw (ObjectAlreadyExists) : parent_(NULL), name_(name), extraBits_(0) {

    parent.addSocketCodeTable(*this);
    parent_ = &parent;
}


/**
 * The constructor.
 *
 * Loads the state of the table from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent binary encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
SocketCodeTable::SocketCodeTable(
    const ObjectState* state,
    BinaryEncoding& parent)
    throw (ObjectStateLoadingException) :
    parent_(&parent), name_(""), extraBits_(0) {

    loadState(state);
    parent_ = NULL;
    parent.addSocketCodeTable(*this);
    parent_ = &parent;
}


/**
 * The destructor.
 *
 * Removes the all the references to the socket code table from socket
 * encodings.
 */
SocketCodeTable::~SocketCodeTable() {

    BinaryEncoding* parent = this->parent();
    for (int i = 0; i < parent->moveSlotCount(); i++) {
	MoveSlot& slot = parent->moveSlot(i);
	SlotField& src = slot.sourceField();
	removeReferences(src);
	SlotField& dst = slot.destinationField();
	removeReferences(dst);
    }

    deleteRFPortCodes();
    deleteFUPortCodes();

    parent_ = NULL;
    parent->removeSocketCodeTable(*this);
}


/**
 * Returns the parent binary encoding map.
 *
 * @return The parent binary encoding map.
 */
BinaryEncoding*
SocketCodeTable::parent() const {
    return parent_;
}


/**
 * Returns the name of the socket code table.
 *
 * @return The name.
 */
std::string
SocketCodeTable::name() const {
    return name_;
}


/**
 * Sets new name for the socket code table.
 *
 * @param name The new name.
 * @exception ObjectAlreadyExists If the parent binary encoding map already
 *                                has a socket code table with the same name.
 */
void
SocketCodeTable::setName(const std::string& name)
    throw (ObjectAlreadyExists) {

    if (name == this->name()) {
	return ;
    }

    if (hasParentSCTable(name)) {
	const string procName = "SocketCodeTable::setName";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    name_ = name;
}


/**
 * Sets the number of extra zero bits for the table.
 *
 * The bit width of the table can be forced longer than necessary by defining
 * some number of extra zero bits that always exists in the right end of the
 * table field.
 *
 * @param bits The number of extra zero bits.
 * @exception OutOfRange If the given number is negative.
 */
void
SocketCodeTable::setExtraBits(int bits)
    throw (OutOfRange) {

    if (bits < 0) {
	const string procName = "SocketCodeTable::setExtraBits";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    extraBits_ = bits;
}


/**
 * Returns the number of extra bits.
 *
 * @return The number of extra bits.
 */
int
SocketCodeTable::extraBits() const {
    return extraBits_;
}


/**
 * Returns the bit width of the control codes in the table.
 *
 * @return The bit width.
 */
int
SocketCodeTable::width() const {

    int width(0);

    for (int i = 0; i < fuPortCodeCount(); i++) {
	FUPortCode& code = fuPortCode(i);
	if (width < code.width()) {
	    width = code.width();
	}
    }

    for (int i = 0; i < rfPortCodeCount(); i++) {
	RFPortCode& code = rfPortCode(i);
	if (width < code.width()) {
	    width = code.width();
	}
    }

    for (int i = 0; i < iuPortCodeCount(); i++) {
        IUPortCode& code = iuPortCode(i);
        if (width < code.width()) {
            width = code.width();
        }
    }

    return width + extraBits();
}


/**
 * Adds the given control code that identifies a port of a function unit.
 *
 * This method is to be called from the constructor of FUPortCode class.
 *
 * @param code The code to be added.
 * @exception ObjectAlreadyExists If the port is already encoded in this
 *                                table or if the encoding is ambiguous with
 *                                other encodings in the socket code table.
 */
void
SocketCodeTable::addFUPortCode(FUPortCode& code)
    throw (ObjectAlreadyExists) {

    const string procName = "SocketCodeTable::addFUPortCode";

    if (hasRFOrIUPortCodeWithoutEncoding()) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    if (!BEMTester::canAddPortEncoding(
	    *this, code.encoding(), code.extraBits())) {
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    if (code.hasOperation()) {
	if (hasFUPortCode(
		code.unitName(), code.portName(), code.operationName())) {
	    throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
	}
    } else {
	if (hasFUPortCode(code.unitName(), code.portName())) {
	    throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
	}
    }

    fuPortCodes_.push_back(&code);
}


/**
 * Removes the given FU port code from the socket code table.
 *
 * This method is to be called from the destructor of FUPortCode class.
 *
 * @param code The code to be removed.
 */
void
SocketCodeTable::removeFUPortCode(FUPortCode& code) {
    assert(code.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(fuPortCodes_, &code));
}


/**
 * Returns the number of FU port codes in this table.
 *
 * @return The number of FU port codes.
 */
int
SocketCodeTable::fuPortCodeCount() const {
    return fuPortCodes_.size();
}


/**
 * Returns the FU port code stored at the given position in the table.
 *
 * @param index The position.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of FU port codes in the table.
 */
FUPortCode&
SocketCodeTable::fuPortCode(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= fuPortCodeCount()) {
	const string procName = "SocketCodeTable::fuPortCode";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *fuPortCodes_[index];
}


/**
 * Tells whether the table has a control code for the port of the function
 * unit.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @return True if the table has the control code, otherwise false.
 */
bool
SocketCodeTable::hasFUPortCode(
    const std::string& fu,
    const std::string& port) const {

    FUPortCode& code = fuPortCode(fu, port);
    return &code != &NullFUPortCode::instance();
}


/**
 * Tells whether the table has a control code that identifies the given
 * operation and the port that carries it.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param operation Name of the operation.
 * @return True if the table has the control code, otherwise false.
 */
bool
SocketCodeTable::hasFUPortCode(
    const std::string& fu,
    const std::string& port,
    const std::string& operation) const {

    FUPortCode& code = fuPortCode(fu, port, operation);
    return &code != &NullFUPortCode::instance();
}


/**
 * Returns the code for the given port of the given function unit.
 *
 * Returns a NullFUPortCode instance if the requested code is not found.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @return The control code for the requested port.
 */
FUPortCode&
SocketCodeTable::fuPortCode(
    const std::string& fu,
    const std::string& port) const {

    int codes = fuPortCodeCount();
    for (int i = 0; i < codes; i++) {
	FUPortCode& code = fuPortCode(i);
	if (code.unitName() == fu && code.portName() == port &&
	    !code.hasOperation()) {
	    return code;
	}
    }

    return NullFUPortCode::instance();
}


/**
 * Returns the code that identifies the given operation and the port that
 * carries it.
 *
 * Returns a NullFUPortCode instance if the requested code is not found.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param operation Name of the operation.
 * @return The control code for the requested port.
 */
FUPortCode&
SocketCodeTable::fuPortCode(
    const std::string& fu,
    const std::string& port,
    const std::string& operation) const {

    int codes = fuPortCodeCount();
    for (int i = 0; i < codes; i++) {
	FUPortCode& code = fuPortCode(i);
	if (code.unitName() == fu && code.portName() == port &&
	    code.hasOperation() && 
            StringTools::ciEqual(code.operationName(), operation)) {
            return code;
        }
    }

    return NullFUPortCode::instance();
}


/**
 * Adds a control code that identifies a register file.
 *
 * In fact, the control code identifies a RF port, but which port is
 * irrelevant, since all ports are identical and each must be attached to a
 * different socket. This method is to be called from the constructor of
 * RFPortCode class.
 *
 * @param code The control code to be added.
 * @exception ObjectAlreadyExists If the register file is already encoded in
 *                                this table or if the encoding is ambiguous
 *                                with another encoding in the socket code
 *                                table.
 */
void
SocketCodeTable::addRFPortCode(RFPortCode& code)
    throw (ObjectAlreadyExists) {

    assert(code.parent() == NULL);
    const string procName = "SocketCodeTable::addRFPortCode";

    if (!code.hasEncoding() && containsPortCode()) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    if (code.hasEncoding()) {
        if (!BEMTester::canAddPortEncoding(
                *this, code.encoding(), code.extraBits())) {
            throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
        }
    }

    if (hasRFPortCode(code.unitName())) {
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    rfPortCodes_.push_back(&code);
}


/**
 * Removes the given RF port code.
 *
 * This method is to be called from the destructor of RFPortCode.
 *
 * @param code The code to be removed.
 */
void
SocketCodeTable::removeRFPortCode(RFPortCode& code) {
    assert(code.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(rfPortCodes_, &code));
}


/**
 * Returns the number of register file sources or destinations encoded in this
 * table.
 *
 * @return The number of register file sources or destinations.
 */
int
SocketCodeTable::rfPortCodeCount() const {
    return rfPortCodes_.size();
}


/**
 * Returns the RF code stored at the given position.
 *
 * @param index The position.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of RF codes in this table.
 */
RFPortCode&
SocketCodeTable::rfPortCode(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= rfPortCodeCount()) {
	const string procName = "SocketCodeTable::rfPortCode";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *rfPortCodes_[index];
}


/**
 * Tells whether the socket code table contains a control code for the given
 * register file.
 *
 * @param regFile Name of the register file.
 * @return True If the socket code table contains the control code, otherwise
 *         false.
 */
bool
SocketCodeTable::hasRFPortCode(const std::string& regFile) const {
    return &rfPortCode(regFile) != &NullRFPortCode::instance();
}


/**
 * Returns the code for the given register file.
 *
 * Returns a NullRFPortCode instance if there is no code for the given
 * register file in the table.
 *
 * @param regFile Name of the register file.
 */
RFPortCode&
SocketCodeTable::rfPortCode(const std::string& regFile) const {
    int codes = rfPortCodeCount();
    for (int i = 0; i < codes; i++) {
	RFPortCode& code = rfPortCode(i);
	if (code.unitName() == regFile) {
	    return code;
	}
    }

    return NullRFPortCode::instance();
}


/**
 * Adds a control code that identifies an immediate unit.
 *
 * In fact, the control code identifies a IU port, but which port is
 * irrelevant, since all ports are identical and each must be attached to a
 * different socket. This method is to be called from the constructor of
 * IUPortCode class.
 *
 * @param code The control code to be added.
 * @exception ObjectAlreadyExists If the register file is already encoded in
 *                                this table or if the encoding is ambiguous
 *                                with another encoding in the socket code
 *                                table.
 */
void
SocketCodeTable::addIUPortCode(IUPortCode& code)
    throw (ObjectAlreadyExists) {

    assert(code.parent() == NULL);
    const string procName = "SocketCodeTable::addIUPortCode";

    if (!code.hasEncoding() && containsPortCode()) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    if (code.hasEncoding()) {
        if (!BEMTester::canAddPortEncoding(
                *this, code.encoding(), code.extraBits())) {
            throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
        }
    }

    if (hasIUPortCode(code.unitName())) {
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    iuPortCodes_.push_back(&code);
}


/**
 * Removes the given IU port code.
 *
 * This method is to be called from the destructor of IUPortCode.
 *
 * @param code The code to be removed.
 */
void
SocketCodeTable::removeIUPortCode(IUPortCode& code) {
    assert(code.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(iuPortCodes_, &code));
}


/**
 * Returns the number of immediate unit sources or destinations encoded in this
 * table.
 *
 * @return The number of immediate unit sources or destinations.
 */
int
SocketCodeTable::iuPortCodeCount() const {
    return iuPortCodes_.size();
}


/**
 * Returns the IU code stored at the given position.
 *
 * @param index The position.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of IU codes in this table.
 */
IUPortCode&
SocketCodeTable::iuPortCode(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= iuPortCodeCount()) {
	const string procName = "SocketCodeTable::iuPortCode";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *iuPortCodes_[index];
}


/**
 * Tells whether the socket code table contains a control code for the given
 * immediate unit.
 *
 * @param immediateUnit Name of the immediate unit.
 * @return True If the socket code table contains the control code, otherwise
 *         false.
 */
bool
SocketCodeTable::hasIUPortCode(const std::string& immediateUnit) const {
    return &iuPortCode(immediateUnit) != &NullIUPortCode::instance();
}


/**
 * Returns the code for the given immediate unit.
 *
 * Returns a NullIUPortCode instance if there is no code for the given register
 * file in the table.
 *
 * @param immediateUnit Name of the immediate unit.
 */
IUPortCode&
SocketCodeTable::iuPortCode(const std::string& immediateUnit) const {
    int codes = iuPortCodeCount();
    for (int i = 0; i < codes; i++) {
	IUPortCode& code = iuPortCode(i);
	if (code.unitName() == immediateUnit) {
	    return code;
	}
    }

    return NullIUPortCode::instance();
}


/**
 * Loads the state of the socket code table from the given ObjectState
 * instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
SocketCodeTable::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    deleteFUPortCodes();
    deleteRFPortCodes();
    deleteIUPortCodes();

    try {
	setName(state->stringAttribute(OSKEY_NAME));
	setExtraBits(state->intAttribute(OSKEY_EXTRA_BITS));

	for (int i = 0; i < state->childCount(); i++) {
	    ObjectState* child = state->child(i);
	    if (child->name() == FUPortCode::OSNAME_FU_PORT_CODE) {
		new FUPortCode(child, *this);
	    } else if (child->name() == RFPortCode::OSNAME_RF_PORT_CODE) {
		new RFPortCode(child, *this);
	    } else if (child->name() == IUPortCode::OSNAME_IU_PORT_CODE) {
		new IUPortCode(child, *this);
	    }
	}
    } catch (const Exception& exception) {
	const string procName = "SocketCodeTable::loadState";
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}


/**
 * Saves the state of the socket code table to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
SocketCodeTable::saveState() const {

    ObjectState* state = new ObjectState(OSNAME_SOCKET_CODE_TABLE);
    state->setAttribute(OSKEY_NAME, name());
    state->setAttribute(OSKEY_EXTRA_BITS, extraBits());

    // add FU port codes
    for (int i = 0; i < fuPortCodeCount(); i++) {
	FUPortCode& code = fuPortCode(i);
	state->addChild(code.saveState());
    }

    // add RF port codes
    for (int i = 0; i < rfPortCodeCount(); i++) {
	RFPortCode& code = rfPortCode(i);
	state->addChild(code.saveState());
    }

    // add IU port codes
    for (int i = 0; i < iuPortCodeCount(); i++) {
	IUPortCode& code = iuPortCode(i);
	state->addChild(code.saveState());
    }

    return state;
}


/**
 * Removes all the references to this socket code table from the socket
 * encodings of the given slot field.
 */
void
SocketCodeTable::removeReferences(SlotField& field) const {
    for (int i = 0; i < field.socketEncodingCount(); i++) {
	SocketEncoding& encoding = field.socketEncoding(i);
	if (&encoding.socketCodes() == this) {
	    encoding.unsetSocketCodes();
	}
    }
}


/**
 * Tells whether the parent binary encoding table has a socket code table with
 * the given name.
 *
 * @param name The name.
 * @return True if the parent has a socket code table with the given name,
 *         otherwise false.
 */
bool
SocketCodeTable::hasParentSCTable(const std::string& name) const {
    BinaryEncoding* parent = this->parent();
    for (int i = 0; i < parent->socketCodeTableCount(); i++) {
	SocketCodeTable& table = parent->socketCodeTable(i);
	if (table.name() == name) {
	    return true;
	}
    }
    return false;
}


/**
 * Deletes all the RF port codes from the table.
 */
void
SocketCodeTable::deleteRFPortCodes() {
    SequenceTools::deleteAllItems(rfPortCodes_);
}


/**
 * Deletes all the FU port codes from the table.
 */
void
SocketCodeTable::deleteFUPortCodes() {
    SequenceTools::deleteAllItems(fuPortCodes_);
}


/**
 * Deletes all the IU port codes from the table.
 */
void
SocketCodeTable::deleteIUPortCodes() {
    SequenceTools::deleteAllItems(iuPortCodes_);
}


/**
 * Tells whether the socket code table contains a RF or IU port code without
 * port encoding.
 *
 * @return True If the socket code table contains a RF or IU port code
 *              without port encoding, otherwise false.
 */
bool
SocketCodeTable::hasRFOrIUPortCodeWithoutEncoding() const {
    if (rfPortCodeCount() == 1 && !rfPortCode(0).hasEncoding()) {
        return true;
    }
    if (iuPortCodeCount() == 1 && !iuPortCode(0).hasEncoding()) {
        return true;
    }
    return false;
}


/**
 * Tells whether the socket code table contains at least one port code.
 *
 * @return True if the table contains at least one port code, otherwise
 *         false.
 */
bool
SocketCodeTable::containsPortCode() const {
    if (fuPortCodeCount() != 0 || rfPortCodeCount() != 0 ||
        iuPortCodeCount() != 0) {
        return true;
    } else {
        return false;
    }
}
