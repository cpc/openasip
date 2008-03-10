/**
 * @file NullInstructionField.cc
 *
 * Implementation of NullInstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "NullInstructionField.hh"
#include "Application.hh"

using std::string;

NullInstructionField NullInstructionField::instance_;

/**
 * The constructor.
 */
NullInstructionField::NullInstructionField() : InstructionField(NULL) {
}


/**
 * The destructor.
 */
NullInstructionField::~NullInstructionField() {
}


/**
 * Returns the only instance of NullInstructionField.
 *
 * @return The only instance of NullInstructionField.
 */
NullInstructionField&
NullInstructionField::instance() {
    return instance_;
}


/**
 * Always returns 0.
 *
 * @return 0.
 */
int
NullInstructionField::childFieldCount() const {
    return 0;
}


/**
 * Always throws OutOfRange exception.
 *
 * @return Never returns.
 * @exception OutOfRange Always thrown.
 */
InstructionField&
NullInstructionField::childField(int) const
    throw (OutOfRange) {

    const string procName = "NullInstructionField::childField";
    throw OutOfRange(__FILE__, __LINE__, procName);
}


/**
 * Always returns 0.
 *
 * @return 0.
 */
int
NullInstructionField::width() const {
    return 0;
}

