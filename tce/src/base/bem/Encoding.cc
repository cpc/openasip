/**
 * @file Encoding.cc
 *
 * Implementation of Encoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "Encoding.hh"
#include "ObjectState.hh"
#include "MathTools.hh"

using std::string;

const std::string Encoding::OSNAME_ENCODING = "encoding";
const std::string Encoding::OSKEY_ENCODING = "encoding";
const std::string Encoding::OSKEY_EXTRA_BITS = "extra_bits";

/**
 * The constructor.
 *
 * @param encoding The encoding.
 * @param extraBits The number of extra bits.
 * @param parent The parent pointer.
 */
Encoding::Encoding(
    unsigned int encoding,
    unsigned int extraBits,
    InstructionField* parent) : 
    encoding_(encoding), extraBits_(extraBits), parent_(parent) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent pointer.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
Encoding::Encoding(const ObjectState* state, InstructionField* parent)
    throw (ObjectStateLoadingException) :
    encoding_(0), extraBits_(0), parent_(parent) {

    const string procName = "Encoding::Encoding";

    try {
        encoding_ = state->intAttribute(OSKEY_ENCODING);
        extraBits_ = state->intAttribute(OSKEY_EXTRA_BITS);
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * The destructor.
 */
Encoding::~Encoding() {
}


/**
 * Returns the parent instruction field.
 *
 * @return The parent instruction field.
 */
InstructionField*
Encoding::parent() const {
    return parent_;
}


/**
 * Returns the encoding.
 *
 * @return The encoding.
 */
unsigned int
Encoding::encoding() const {
    return encoding_;
}


/**
 * Returns the number of extra zero bits in the encoding.
 *
 * @return The number of extra bits.
 */
unsigned int
Encoding::extraBits() const {
    return extraBits_;
}


/**
 * Returns the bit width required by the encoding.
 *
 * @return The bit width.
 */
int
Encoding::width() const {
    return MathTools::requiredBits(encoding()) + extraBits();
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
Encoding::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_ENCODING);
    state->setAttribute(OSKEY_ENCODING, encoding());
    state->setAttribute(OSKEY_EXTRA_BITS, extraBits());
    return state;
}


/**
 * Sets the parent pointer.
 *
 * @param parent The parent pointer.
 */
void
Encoding::setParent(InstructionField* parent) {
    parent_ = parent;
}
