/**
 * @file GuardField.cc
 *
 * Implementation of GuardField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "GuardField.hh"
#include "MoveSlot.hh"
#include "GPRGuardEncoding.hh"
#include "NullGPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "NullFUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "NullUnconditionalGuardEncoding.hh"
#include "MathTools.hh"
#include "ContainerTools.hh"
#include "SequenceTools.hh"
#include "Application.hh"

using std::string;

const std::string GuardField::OSNAME_GUARD_FIELD = "guard_field";

/**
 * The constructor.
 *
 * Registers the guard field to the parent move slot automatically.
 *
 * @param parent The parent move slot.
 * @exception ObjectAlreadyExists If the parent move slot has a guard field
 *                                already.
 */
GuardField::GuardField(MoveSlot& parent)
    throw (ObjectAlreadyExists) : 
    InstructionField(&parent), alwaysTrue_(NULL), alwaysFalse_(NULL) {

    setParent(NULL);
    parent.setGuardField(*this);
    setParent(&parent);

    // update the relative position of the field
    int position(0);
    if (parent.hasSourceField()) {
	position++;
    }
    if (parent.hasDestinationField()) {
	position++;
    }
    InstructionField::setRelativePosition(position);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent move slot.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the parent move slot has a guard field
 *                                already.
 */
GuardField::GuardField(const ObjectState* state, MoveSlot& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    InstructionField(state, &parent), alwaysTrue_(NULL), alwaysFalse_(NULL) {

    loadState(state);
    setParent(NULL);
    parent.setGuardField(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
GuardField::~GuardField() {
    deleteGPRGuardEncodings();
    deleteFUGuardEncodings();
    deleteUnconditionalGuardEncodings();
    MoveSlot* oldParent = parent();
    setParent(NULL);
    assert(oldParent != NULL);
    oldParent->unsetGuardField();
}


/**
 * Returns the parent move slot.
 *
 * @return The parent move slot.
 */
MoveSlot*
GuardField::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent == NULL) {
	return NULL;
    } else {
	MoveSlot* slot = dynamic_cast<MoveSlot*>(parent);
	assert(slot != NULL);
	return slot;
    }
}


/**
 * Adds a guard expression and its encoding to the set of expressions
 * supported by this guard field.
 *
 * This method is to be called from the constructor GPRGuardEncoding.
 *
 * @param encoding The encoding to be added.
 * @exception ObjectAlreadyExists If the guard field already contains a guard
 *                                encoding for the same guard expression or
 *                                if the given encoding is already assigned.
 */
void
GuardField::addGuardEncoding(GPRGuardEncoding& encoding)
    throw (ObjectAlreadyExists) {

    // verify that this is called from GPRGuardEncoding constructor
    assert(encoding.parent() == NULL);

    if (hasGPRGuardEncoding(
	    encoding.registerFile(), encoding.registerIndex(),
	    encoding.isGuardInverted()) || isAssigned(encoding.encoding())) {
	const string procName = "GuardField::addGuardEncoding";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    gprGuards_.push_back(&encoding);
}


/**
 * Removes the given guard encoding.
 *
 * This method is to be called from the destructor of GPRGuardField.
 *
 * @param encoding The encoding to be removed.
 */
void
GuardField::removeGuardEncoding(GPRGuardEncoding& encoding) {
    // verify that this is called from GPRGuardEncoding destructor
    assert(encoding.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(gprGuards_, &encoding));
}


/**
 * Adds a guard expression and its encoding to the set of expressions
 * supported by this guard field.
 *
 * This method is to be called from the constructor of FUGuardEncoding.
 *
 * @param encoding The encoding to be added.
 * @exception ObjectAlreadyExists If the guard field already contains a guard
 *                                encoding for the same guard expression or
 *                                if the encoding is already assigned to
 *                                another guard expression.
 */
void
GuardField::addGuardEncoding(FUGuardEncoding& encoding)
    throw (ObjectAlreadyExists) {

    // verify that this is called from GPRGuardEncoding constructor
    assert(encoding.parent() == NULL);

    if (hasFUGuardEncoding(
	    encoding.functionUnit(), encoding.port(),
	    encoding.isGuardInverted()) || isAssigned(encoding.encoding())) {
	const string procName = "GuardField::addGuardEncoding";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    fuGuards_.push_back(&encoding);
}


/**
 * Removes the given guard encoding.
 *
 * This method is to be called from the destructor of FUGuardField.
 *
 * @param encoding The encoding to be removed.
 */
void
GuardField::removeGuardEncoding(FUGuardEncoding& encoding) {
    // verify that this is called from GPRGuardEncoding destructor
    assert(encoding.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(fuGuards_, &encoding));
}


/**
 * Adds a guard expression and its encoding to the set of expressions
 * supported by this guard field.
 *
 * This method is to be called from the constructor of 
 * UnconditionalGuardEncoding.
 *
 * @param encoding The encoding to be added.
 * @exception ObjectAlreadyExists If the guard field already contains an
 *                                encoding for the same unconditional guard 
 *                                expression or if the encoding is already
 *                                assigned to another guard expression.
 */
void
GuardField::addGuardEncoding(UnconditionalGuardEncoding& encoding)
    throw (ObjectAlreadyExists) {

    // verify that this is called from UnconditionalGuardEncoding constructor
    assert(encoding.parent() == NULL);
    
    if (hasUnconditionalGuardEncoding(encoding.isGuardInverted()) || 
        isAssigned(encoding.encoding())) {
        const string procName = "GuardField::addGuardEncoding";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }
 
    if (encoding.isGuardInverted()) {
        alwaysFalse_ = &encoding;
    } else {
        alwaysTrue_ = &encoding;
    }
}


/**
 * Removes the encoding for unconditional guard.
 *
 * This method is to be called from the destructor of 
 * UnconditionalGuardEncoding class.
 *
 * @param encoding The encoding to be removed.
 */
void
GuardField::removeUnconditionalGuardEncoding(
    UnconditionalGuardEncoding& encoding) {

    assert(hasUnconditionalGuardEncoding(encoding.isGuardInverted()));
    assert(
        &unconditionalGuardEncoding(encoding.isGuardInverted()) == 
        &encoding);
    assert(encoding.parent() == NULL);
    
    if (encoding.isGuardInverted()) {
        alwaysFalse_ = NULL;
    } else {
        alwaysTrue_ = NULL;
    }
}


/**
 * Returns the number of guard expressions with general purpose register term
 * that are encoded in this field.
 *
 * @return The number of guard expressions.
 */
int
GuardField::gprGuardEncodingCount() const {
    return gprGuards_.size();
}


/**
 * Returns the GPRGuardEncoding at the given position.
 *
 * @param index The position.
 * @return The GPR guard encoding.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of GPR guard encodings.
 */
GPRGuardEncoding&
GuardField::gprGuardEncoding(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= gprGuardEncodingCount()) {
	const string procName = "GuardField::gprGuardEncoding";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *gprGuards_[index];
}


/**
 * Tells whether the guard field has an encoding for the given guard
 * expression.
 *
 * The expression is identified by a general purpose register (register file
 * name and register index) and by the "invert" flag.
 *
 * @param regFile Name of the register file.
 * @param index The register index.
 * @param inverted The "invert" flag.
 * @return True if the guard field has an encoding for the given guard
 *         expression, otherwise false.
 */
bool
GuardField::hasGPRGuardEncoding(
    const std::string& regFile,
    int index,
    bool inverted) const {

    for (GPRGuardTable::const_iterator iter = gprGuards_.begin();
	 iter != gprGuards_.end(); iter++) {

	GPRGuardEncoding* encoding = *iter;
	if (encoding->registerFile() == regFile &&
	    encoding->registerIndex() == index &&
	    encoding->isGuardInverted() == inverted) {
	    return true;
	}
    }

    return false;
}


/**
 * Returns the GPRGuardEncoding for the given guard expression.
 *
 * The expression is identified by a general purpose register
 * (register file name and register index) and by the "invert" flag. Returns
 * a NullGPRGuardEncoding instance if there is no such guard.
 *
 * @param regFile Name of the register file.
 * @param index The register index.
 * @param inverted The "invert" flag.
 * @return GPRGuardEncoding of the given guard expression.
 */
GPRGuardEncoding&
GuardField::gprGuardEncoding(
    const std::string& regFile,
    int index,
    bool inverted) const {

    for (GPRGuardTable::const_iterator iter = gprGuards_.begin();
	 iter != gprGuards_.end(); iter++) {

	GPRGuardEncoding* encoding = *iter;
	if (encoding->registerFile() == regFile &&
	    encoding->registerIndex() == index &&
	    encoding->isGuardInverted() == inverted) {
	    return *encoding;
	}
    }

    return NullGPRGuardEncoding::instance();
}


/**
 * Returns the number of guard expressions with function unit output term
 * that are encoded in this field.
 *
 * @return The number of guard expressions.
 */
int
GuardField::fuGuardEncodingCount() const {
    return fuGuards_.size();
}


/**
 * Returns the FUGuardEncoding at the given position.
 *
 * @param index The position.
 * @return The FU guard encoding.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of FU guard encodings.
 */
FUGuardEncoding&
GuardField::fuGuardEncoding(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= fuGuardEncodingCount()) {
	const string procName = "GuardField::fuGuardEncoding";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *fuGuards_[index];
}


/**
 * Tells whether the guard field has an encoding for the given guard
 * expression.
 *
 * The expression is identified by a function unit output port (FU name and
 * port name) and by the "invert" flag.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param inverted The "invert" flag.
 * @return True if the guard field has an encoding for the given guard
 *         expression, otherwise false.
 */
bool
GuardField::hasFUGuardEncoding(
    const std::string& fu,
    const std::string& port,
    bool inverted) const {

    for (FUGuardTable::const_iterator iter = fuGuards_.begin();
	 iter != fuGuards_.end(); iter++) {

	FUGuardEncoding* encoding = *iter;
	if (encoding->functionUnit() == fu &&
	    encoding->port() == port &&
	    encoding->isGuardInverted() == inverted) {
	    return true;
	}
    }

    return false;
}


/**
 * Returns the FUGuardEncoding for the given guard expression.
 *
 * The expression is identified by a function unit output port (FU
 * name and port name) and by the "invert" flag. Returns a
 * NullFUGuardEncoding instance if there is no such guard.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param inverted The "invert" flag.
 * @return FUGuardEncoding of the given guard expression.
 */
FUGuardEncoding&
GuardField::fuGuardEncoding(
    const std::string& fu,
    const std::string& port,
    bool inverted) const {

    for (FUGuardTable::const_iterator iter = fuGuards_.begin();
	 iter != fuGuards_.end(); iter++) {

	FUGuardEncoding* encoding = *iter;
	if (encoding->functionUnit() == fu &&
	    encoding->port() == port &&
	    encoding->isGuardInverted() == inverted) {
	    return *encoding;
	}
    }

    return NullFUGuardEncoding::instance();
}


/**
 * Tells whether the guard field has an encoding for unconditional guard.
 *
 * @param inverted The "invert" flag.
 * @return True if there is an encoding for unconditional guard, otherwise
 *         false.
 */
bool
GuardField::hasUnconditionalGuardEncoding(bool inverted) const {
    if (inverted) {
        return alwaysFalse_ != NULL;
    } else {
        return alwaysTrue_ != NULL;
    }
}


/**
 * Returns the unconditional guard encoding.
 *
 * Returns NullUnconditionalGuardEncoding instance if there is no encoding
 * for unconditional guard.
 *
 * @param inverted The "invert" flag.
 * @return The unconditional guard encoding.
 */
UnconditionalGuardEncoding&
GuardField::unconditionalGuardEncoding(bool inverted) const {
    if (hasUnconditionalGuardEncoding(inverted)) {
        if (inverted) {
            return *alwaysFalse_;
        } else {
            return *alwaysTrue_;
        }
    } else {
        return NullUnconditionalGuardEncoding::instance();
    }
}


/**
 * Always returns 0 because guard field does not have any child fields.
 *
 * @return 0.
 */
int
GuardField::childFieldCount() const {
    return 0;
}


/**
 * Always throws OutOfRange exception since guard field does not have any
 * child fields.
 *
 * @return Never returns.
 * @exception OutOfRange Always thrown.
 */
InstructionField&
GuardField::childField(int) const
    throw (OutOfRange) {

    const string procName = "GuardField::childField";
    throw OutOfRange(__FILE__, __LINE__, procName);
}


/**
 * Returns the bit width of the guard field.
 *
 * @return Bit width of the field.
 */
int
GuardField::width() const {

    int gprGuards = gprGuardEncodingCount();
    int fuGuards = fuGuardEncodingCount();
    int width(0);

    for (int i = 0; i < gprGuards; i++) {
	GPRGuardEncoding& encoding = gprGuardEncoding(i);
	int requiredBits = MathTools::requiredBits(encoding.encoding());
	if (requiredBits > width) {
	    width = requiredBits;
	}
    }

    for (int i = 0; i < fuGuards; i++) {
	FUGuardEncoding& encoding = fuGuardEncoding(i);
	int requiredBits = MathTools::requiredBits(encoding.encoding());
	if (requiredBits > width) {
	    width = requiredBits;
	}
    }

    if (hasUnconditionalGuardEncoding(true)) {
        UnconditionalGuardEncoding& encoding = unconditionalGuardEncoding(
            true);
        int requiredBits = MathTools::requiredBits(encoding.encoding());
        if (requiredBits > width) {
            width = requiredBits;
        }
    }

    if (hasUnconditionalGuardEncoding(false)) {
        UnconditionalGuardEncoding& encoding = unconditionalGuardEncoding(
            false);
        int requiredBits = MathTools::requiredBits(encoding.encoding());
        if (requiredBits > width) {
            width = requiredBits;
        }
    }    

    return width + extraBits();
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
GuardField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "GuardField::loadState";

    if (state->name() != OSNAME_GUARD_FIELD) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    deleteGPRGuardEncodings();
    deleteFUGuardEncodings();
    InstructionField::loadState(state);

    try {
	for (int i = 0; i < state->childCount(); i++) {
	    ObjectState* child = state->child(i);
	    if (child->name() == FUGuardEncoding::OSNAME_FU_GUARD_ENCODING) {
		new FUGuardEncoding(child, *this);
	    } else if (child->name() ==
		       GPRGuardEncoding::OSNAME_GPR_GUARD_ENCODING) {
		new GPRGuardEncoding(child, *this);
	    } else if (child->name() == 
                       UnconditionalGuardEncoding::
                       OSNAME_UNCONDITIONAL_GUARD_ENCODING) {
                new UnconditionalGuardEncoding(child, *this);
            }
	}
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Saves the state of the guard field to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
GuardField::saveState() const {

    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_GUARD_FIELD);

    // add GPR guard encodings
    for (int i = 0; i < gprGuardEncodingCount(); i++) {
	GPRGuardEncoding& enc = gprGuardEncoding(i);
	state->addChild(enc.saveState());
    }

    // add FU guard encodings
    for (int i = 0; i < fuGuardEncodingCount(); i++) {
	FUGuardEncoding& enc = fuGuardEncoding(i);
	state->addChild(enc.saveState());
    }

    // add unconditional guard encodings
    if (hasUnconditionalGuardEncoding(true)) {
        state->addChild(unconditionalGuardEncoding(true).saveState());
    }
    if (hasUnconditionalGuardEncoding(false)) {
        state->addChild(unconditionalGuardEncoding(false).saveState());
    }    

    return state;
}


/**
 * Deletes all the register guard encodings from the guard field.
 */
void
GuardField::deleteGPRGuardEncodings() {
    SequenceTools::deleteAllItems(gprGuards_);
}


/**
 * Deletes all the function unit port guard encodings from the guard field.
 */
void
GuardField::deleteFUGuardEncodings() {
    SequenceTools::deleteAllItems(fuGuards_);
}


/**
 * Deletes the unconditional guard encodings from the guard field.
 */
void
GuardField::deleteUnconditionalGuardEncodings() {
    if (hasUnconditionalGuardEncoding(true)) {
        delete alwaysFalse_;
        alwaysFalse_ = NULL;
    }
    if (hasUnconditionalGuardEncoding(false)) {
        delete alwaysTrue_;
        alwaysTrue_ = NULL;
    }
}


/**
 * Tells whether the given encoding is assigned to a guard expression.
 *
 * @param encoding The encoding.
 * @return True if the encoding is assigned, otherwise false.
 */
bool
GuardField::isAssigned(unsigned int encoding) const {

    int fuGuards = fuGuardEncodingCount();
    int rfGuards = gprGuardEncodingCount();

    for (int i = 0; i < fuGuards; i++) {
	FUGuardEncoding& guard = fuGuardEncoding(i);
	if (guard.encoding() == encoding) {
	    return true;
	}
    }

    for (int i = 0; i < rfGuards; i++) {
	GPRGuardEncoding& guard = gprGuardEncoding(i);
	if (guard.encoding() == encoding) {
	    return true;
	}
    }

    if (hasUnconditionalGuardEncoding(false)) {
        if (unconditionalGuardEncoding(false).encoding() == encoding) {
            return true;
        }
    }

    if (hasUnconditionalGuardEncoding(true)) {
        if (unconditionalGuardEncoding(true).encoding() == encoding) {
            return true;
        }
    }

    return false;
}

