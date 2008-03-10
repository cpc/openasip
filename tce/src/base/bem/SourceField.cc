/**
 * @file SourceField.cc
 *
 * Implementation of SourceField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "SourceField.hh"
#include "MoveSlot.hh"
#include "BridgeEncoding.hh"
#include "ImmediateEncoding.hh"
#include "NullImmediateEncoding.hh"
#include "NullBridgeEncoding.hh"
#include "BEMTester.hh"
#include "BEMTextGenerator.hh"
#include "ContainerTools.hh"
#include "SequenceTools.hh"
#include "Application.hh"

using std::string;
using boost::format;

const std::string SourceField::OSNAME_SOURCE_FIELD = "source_field";

/**
 * The constructor.
 *
 * Creates a source field and registers it into the given move slot.
 *
 * @param componentIDPos Position of the socket or bridge ID within the
 *                       source field.
 * @param parent The parent move slot.
 * @exception ObjectAlreadyExists If the given move slot already has a source
 *                                field.
 * @exception IllegalParameters If the given component ID position is not the
 *                              same with other source fields in the binary
 *                              encoding map.
 */
SourceField::SourceField(
    BinaryEncoding::Position componentIDPos,
    MoveSlot& parent)
    throw (ObjectAlreadyExists, IllegalParameters) :
    SlotField(componentIDPos, parent), immEncoding_(NULL) {

    BinaryEncoding* bem = parent.parent();
    for (int i = 0; i < bem->moveSlotCount(); i++) {
	MoveSlot& slot = bem->moveSlot(i);
	if (slot.hasSourceField() &&
	    (slot.sourceField().componentIDPosition() != componentIDPos)) {
	    const string procName = "SourceField::SourceField";
	    throw IllegalParameters(__FILE__, __LINE__, procName);
	} else {
	    break;
	}
    }

    setParent(NULL);
    parent.setSourceField(*this);
    setParent(&parent);
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
 * @exception ObjectAlreadyExists If the given move slot already has a source
 *                                field.
 */
SourceField::SourceField(const ObjectState* state, MoveSlot& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    SlotField(state, parent), immEncoding_(NULL) {

    loadState(state);
    setParent(NULL);
    parent.setSourceField(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
SourceField::~SourceField() {
    clearBridgeEncodings();
    clearImmediateEncoding();
    MoveSlot* parent = this->parent();
    setParent(NULL);
    parent->unsetSourceField();
}


/**
 * Adds the given encoding for a bridge to the source field.
 *
 * This method is to be called from the constructor of BridgeEncoding class.
 *
 * @param encoding The encoding to be added.
 * @exception ObjectAlreadyExists If the field already has an encoding for
 *                                the given bridge or if the encoding is
 *                                ambiguous with another encoding or if the
 *                                source field has encodings for two bridges
 *                                already.
 */
void
SourceField::addBridgeEncoding(BridgeEncoding& encoding)
    throw (ObjectAlreadyExists) {

    assert(encoding.parent() == NULL);
    string bridge = encoding.bridgeName();

    const string procName = "SourceField::addBridgeEncoding";

    if (hasBridgeEncoding(bridge) ||
	!BEMTester::canAddComponentEncoding(
	    *this, encoding.encoding(), encoding.extraBits()) ||
	bridgeEncodingCount() == 2) {
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    bridgeEncodings_.push_back(&encoding);
}


/**
 * Removes the given bridge encoding from the source field.
 *
 * This method is to be called from the destructor of BridgeEncoding class.
 *
 * @param encoding The encoding to be removed.
 */
void
SourceField::removeBridgeEncoding(BridgeEncoding& encoding) {
    assert(encoding.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(bridgeEncodings_, &encoding));
}


/**
 * Tells whether the source field has an encoding defined for the given
 * bridge.
 *
 * @param bridge Name of the bridge.
 * @return True if there is an encoding for the given bridge, otherwise
 *         false.
 */
bool
SourceField::hasBridgeEncoding(const std::string& bridge) const {

    int bridgeEncodings = bridgeEncodingCount();
    for (int i = 0; i < bridgeEncodings; i++) {
	BridgeEncoding& encoding = bridgeEncoding(i);
	if (encoding.bridgeName() == bridge) {
	    return true;
	}
    }

    return false;
}


/**
 * Returns the encoding for the given bridge.
 *
 * @param bridge Name of the bridge.
 * @return The bridge encoding for the given bridge or NullBridgeEncoding
 *         instance if there is no encoding for the given bridge.
 */
BridgeEncoding&
SourceField::bridgeEncoding(const std::string& bridge) const {
    int encodingCount = bridgeEncodingCount();
    for (int i = 0; i < encodingCount; i++) {
        BridgeEncoding& encoding = bridgeEncoding(i);
        if (encoding.bridgeName() == bridge) {
            return encoding;
        }
    }
    return NullBridgeEncoding::instance();
}


/**
 * Returns the number of bridge encodings defined for this source field.
 *
 * @return The number of bridge encodings.
 */
int
SourceField::bridgeEncodingCount() const {
    return bridgeEncodings_.size();
}


/**
 * Returns the bridge encoding stored at the given position.
 *
 * @param index The position.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of bridge encodings.
 */
BridgeEncoding&
SourceField::bridgeEncoding(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= bridgeEncodingCount()) {
	const string procName = "SourceField::bridgeEncoding";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *bridgeEncodings_[index];
}


/**
 * Sets the given encoding for inline immediates.
 *
 * This method is to be called from the constructor of ImmediateEncoding.
 *
 * @param encoding The encoding to be set.
 * @exception ObjectAlreadyExists If the source field has an immediate
 *                                encoding already or if the given encoding
 *                                is ambiguous with some socket or bridge
 *                                encoding.
 */
void
SourceField::setImmediateEncoding(ImmediateEncoding& encoding)
    throw (ObjectAlreadyExists) {

    assert(encoding.parent() == NULL);
    
    if (hasImmediateEncoding() ||
        !BEMTester::canAddComponentEncoding(
            *this, encoding.encoding(), encoding.extraBits())) {
        BEMTextGenerator textGen;
        format text = textGen.text(BEMTextGenerator::TXT_ILLEGAL_IMM_ENC);
        text % parent()->name();
        const string procName = "SourceField::setImmediateEncoding";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName, text.str());
    }

    immEncoding_ = &encoding;
}


/**
 * Unsets the immediate encoding.
 *
 * This method is to be called from the destructor of ImmediateEncoding.
 */
void
SourceField::unsetImmediateEncoding() {
    assert(hasImmediateEncoding());
    assert(immediateEncoding().parent() == NULL);
    immEncoding_ = NULL;
}


/**
 * Tells whether the source field has an immediate encoding.
 *
 * @return True if the source field has an immediate encoding, otherwise
 *         false.
 */
bool
SourceField::hasImmediateEncoding() const {
    return immEncoding_ != NULL;
}


/**
 * Returns the immediate encoding.
 *
 * Returns NullImmediateEncoding instance if the source field does not have
 * an immediate encoding.
 *
 * @return The immediate encoding.
 */
ImmediateEncoding&
SourceField::immediateEncoding() const {
    if (hasImmediateEncoding()) {
        return *immEncoding_;
    } else {
        return NullImmediateEncoding::instance();
    }
}


/**
 * Returns the bit width of the source field.
 *
 * @return The bit width.
 */
int
SourceField::width() const {

    int maxEncodingWidth = SlotField::width() - extraBits();

    for (int i = 0; i < bridgeEncodingCount(); i++) {
	BridgeEncoding& encoding = bridgeEncoding(i);
	if (encoding.width() > maxEncodingWidth) {
	    maxEncodingWidth = encoding.width();
	}
    }

    if (hasImmediateEncoding() && immediateEncoding().width() > 
        maxEncodingWidth) {
        maxEncodingWidth = immediateEncoding().width();
    }

    return maxEncodingWidth + extraBits();
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
SourceField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "SourceField::loadState";

    if (state->name() != OSNAME_SOURCE_FIELD) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    clearBridgeEncodings();
    clearImmediateEncoding();
    SlotField::loadState(state);

    try {
	for (int i = 0; i < state->childCount(); i++) {
	    ObjectState* child = state->child(i);
	    if (child->name() == BridgeEncoding::OSNAME_BRIDGE_ENCODING) {
		new BridgeEncoding(child, *this);
	    } else if (child->name() ==
                       ImmediateEncoding::OSNAME_IMM_ENCODING) {
                new ImmediateEncoding(child, *this);
            }
        }
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }

    // TODO: check that the positions of the encodings match with the
    // ones given in the ObjectState instances
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
SourceField::saveState() const {

    ObjectState* state = SlotField::saveState();
    state->setName(OSNAME_SOURCE_FIELD);

    // add bridge encodings
    for (int i = 0; i < bridgeEncodingCount(); i++) {
	BridgeEncoding& enc = bridgeEncoding(i);
	state->addChild(enc.saveState());
    }

    // add immediate encoding
    if (hasImmediateEncoding()) {
        state->addChild(immediateEncoding().saveState());
    }

    return state;
}


/**
 * Clears all the bridge encodings from the source field.
 */
void
SourceField::clearBridgeEncodings() {
    SequenceTools::deleteAllItems(bridgeEncodings_);
}


/**
 * Deletes the immediate encoding if one exists.
 */
void
SourceField::clearImmediateEncoding() {
    if (hasImmediateEncoding()) {
        delete immEncoding_;
    }
}
