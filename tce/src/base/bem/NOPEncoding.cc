/**
 * @file NOPEncoding.cc
 *
 * Implementation of NOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "NOPEncoding.hh"
#include "SourceField.hh"
#include "Application.hh"

using std::string;

const std::string NOPEncoding::OSNAME_NOP_ENCODING = "nop_encoding";

/**
 * The constructor.
 *
 * Registers the encoding automatically to the parent src/dst field.
 *
 * @param encoding The encoding for inline immediates.
 * @param extraBits The number of extra bits in the encoding.
 * @param parent The parent src/dst field.
 * @exception ObjectAlreadyExists If the given parent field has a NOP
 *                                encoding already or if the encoding is
 *                                ambiguous with some other encoding in the
 *                                parent field.
 */
NOPEncoding::NOPEncoding(
    unsigned int encoding,
    unsigned int extraBits,
    SlotField& parent)
    throw (ObjectAlreadyExists) :
    Encoding(encoding, extraBits, NULL) {

    parent.setNoOperationEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent field.
 * @exception ObjectAlreadyExists If the given parent field has a NOP
 *                                encoding already or if the encoding is
 *                                ambiguous with some other
 *                                encoding in the parent field.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is erroneous.
 */
NOPEncoding::NOPEncoding(
    const ObjectState* state,
    SlotField& parent)
    throw (ObjectAlreadyExists, ObjectStateLoadingException) :
    Encoding(state, NULL) {

    const string procName = "NOPEncoding::NOPEncoding";

    if (state->name() != OSNAME_NOP_ENCODING) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    parent.setNoOperationEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
NOPEncoding::~NOPEncoding() {
    SlotField* oldParent = parent();
    assert(oldParent != NULL);
    setParent(NULL);
    oldParent->unsetNoOperationEncoding();
}


/**
 * Returns the parent source field.
 *
 * @return The parent.
 */
SlotField*
NOPEncoding::parent() const {
    InstructionField* parent = Encoding::parent();
    if (parent != NULL) {
        SlotField* sField = dynamic_cast<SlotField*>(parent);
        assert(sField != NULL);
        return sField;
    } else {
        return NULL;
    }
}


/**
 * Returns the position of the encoding within the source field.
 *
 * @return The position of the encoding.
 */
int
NOPEncoding::bitPosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::LEFT) {
        return parent()->width() - parent()->extraBits() - width();
    } else {
        return 0;
    }
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
NOPEncoding::saveState() const {
    ObjectState* state = Encoding::saveState();
    state->setName(OSNAME_NOP_ENCODING);
    return state;
}
