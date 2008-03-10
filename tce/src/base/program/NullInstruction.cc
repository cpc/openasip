/**
 * @file NullInstruction.cc
 *
 * Implementation of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullInstruction.hh"
#include "NullInstructionTemplate.hh"
#include "NullProcedure.hh"
#include "NullImmediate.hh"
#include "NullMove.hh"
#include "NullAddress.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullInstruction
/////////////////////////////////////////////////////////////////////////////

NullInstruction NullInstruction::instance_;

/**
 * The constructor.
 */
NullInstruction::NullInstruction() :
    Instruction(TTAMachine::NullInstructionTemplate::instance()) {
}

/**
 * The destructor.
 */
NullInstruction::~NullInstruction() {
}


/**
 * Returns an instance of NullInstruction class (singleton).
 *
 * @return Singleton instance of NullInstruction class.
 */
NullInstruction&
NullInstruction::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null procedure.
 * @exception IllegalRegistration never.
 */
Procedure&
NullInstruction::parent() const throw (IllegalRegistration) {
    abortWithError("parent()");
    return NullProcedure::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::setParent(const Procedure&) {
    abortWithError("setParent()");
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullInstruction::isInProcedure() const {
    abortWithError("isInProcedure()");
    return false;
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::addMove(Move&)
    throw (ObjectAlreadyExists) {

    abortWithError("addMove()");
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int NullInstruction::moveCount() {
    abortWithError("addMove()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return A null move.
 */
const Move&
NullInstruction::move(int) const throw (OutOfRange) {
    abortWithError("addMove()");
    return NullMove::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullInstruction::addImmediate(Immediate&)
    throw (ObjectAlreadyExists) {
    abortWithError("addMove()");
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int NullInstruction::immediateCount() {
    abortWithError("addMove()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return A null immediate.
 */
Immediate&
NullInstruction::immediate(int) throw (OutOfRange) {
    abortWithError("addMove()");
    return NullImmediate::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return A null address.
 */
Address&
NullInstruction::address() const throw (IllegalRegistration) {
    abortWithError("address()");
    return NullAddress::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int
NullInstruction::size() const {
    abortWithError("size()");
    return -1;
}

}
