/**
 * @file Move.cc
 *
 * Implementation of Move class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "Move.hh"
#include "Socket.hh"
#include "Port.hh"
#include "NullMoveGuard.hh"
#include "NullTerminal.hh"
#include "NullInstruction.hh"
#include "TerminalFUPort.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Move
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * Creates a guarded move.
 *
 * The ownership of the source and destination terminal objects and the
 * guard will be passed to the move.
 *
 * @param src The source of the move.
 * @param dst The destination of the move.
 * @param bus The bus on which the transport is carried.
 * @param guard The Boolean expression the move is predicated with.
 */
Move::Move(
    Terminal* src, Terminal* dst,
    Bus& bus, MoveGuard* guard):
    parent_(NULL), src_(src), dst_(dst), bus_(&bus), guard_(guard) {
}

/**
 * The constructor.
 *
 * Creates an unguarded move.
 *
 * @param src The source of the move.
 * @param dst The destination of the move.
 * @param bus The bus on which the transport is carried.
 */
Move::Move(
    Terminal* src, Terminal* dst, Bus& bus):
    parent_(NULL), src_(src), dst_(dst), bus_(&bus),
    guard_(&NullMoveGuard::instance()){
}

/**
 * The destructor.
 */
Move::~Move() {
    if (dst_ != &NullTerminal::instance()) {
        delete dst_;
        dst_ = NULL;
    }
    if (src_ != &NullTerminal::instance()) {
        delete src_;
        src_ = NULL;
    }
    if (guard_ != &NullMoveGuard::instance()) {
        delete guard_;
        guard_ = NULL;
    }
}

/**
 * Return the parent instruction of the move.
 *
 * @return The parent instruction of the move.
 * @exception IllegalRegistration if the move is independent.
 */
Instruction&
Move::parent() const throw (IllegalRegistration) {
    if (parent_ != NULL) {
        return *parent_;
    } else {
        throw IllegalRegistration(__FILE__, __LINE__, __func__,
                                  "Move is not registered.");
    }
}

/**
 * Set a new parent for the move.
 *
 * @param ins The new parent.
 */
void
Move::setParent(Instruction& ins) {
    if (&ins == &NullInstruction::instance()) {
        parent_ = NULL;
    } else {
        parent_ = &ins;
    }
}

/**
 * Return true if this move belongs to an instruction.
 *
 * @return True if this move belongs to an instruction.
 */
bool
Move::isInInstruction() const {
    return parent_ != NULL;
}

/**
 * Tells whether the move is predicated or not.
 *
 * @return true if this move is not predicated.
 */
bool
Move::isUnconditional() const {
    return (guard_ == &NullMoveGuard::instance());
}

/**
 * Tells whether the move triggers a jump operation.
 *
 * @return true if this move triggers a jump operation.
 */
bool
Move::isJump() const {

    if (dst_->isFUPort()) {
        const TerminalFUPort* tfup = dynamic_cast<const TerminalFUPort*>(dst_);
        if (tfup->isOpcodeSetting()) {
            if (StringTools::ciEqual(tfup->operation().name(), "JUMP")) {
                return true;
            }
        }
    }
    return false;
}
    
/**
 * Tells whether the move triggers a call operation.
 *
 * @return true if this move triggers a call operation.
 */
bool
Move::isCall() const {

    if (dst_->isFUPort()) {
        const TerminalFUPort* tfup = dynamic_cast<const TerminalFUPort*>(dst_);
        if (tfup->isOpcodeSetting()) {
            if (StringTools::ciEqual(tfup->operation().name(), "CALL")) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Tells whether the move triggers an operation that controls
 * the program flow.
 *
 * @return true if this move triggers a jump/call operation.
 */
bool
Move::isControlFlowMove() const {

    if (dst_->isFUPort()) {
        const TerminalFUPort* tfup = dynamic_cast<const TerminalFUPort*>(dst_);
        if (tfup->isOpcodeSetting()) {
            if (tfup->operation().isControlFlowOperation()) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Tells whether the move is used to return from a procedure.
 *
 * Note: the detection is not always reliable, this method can return false
 * even in case the move actually is a return in some cases.
 *
 * @return true if this move is known to be a procedure return.
 */
bool
Move::isReturn() const {

    if (!isJump())
        return false;

    // direct ra -> jump.1 is considered a jump
    if (source().isFUPort() && 
        source().functionUnit().name() == 
        source().functionUnit().machine()->controlUnit()->name() &&
        source().port().name() ==
        source().functionUnit().machine()->controlUnit()->
        returnAddressPort()->name())
        return true;

    if (hasAnnotations(ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN))
        return true;
    
    return false;
}


/**
 * Tells whether the move triggers an operation.
 *
 * @return true if this move triggers an operation.
 */
bool
Move::isTriggering() const {

    if (dst_->isFUPort()) {
        const TerminalFUPort* tfup = dynamic_cast<const TerminalFUPort*>(dst_);
        if (tfup->isTriggering()) {
            return true;
        }
    }
    return false;
}


/**
 * Returns the source of this move.
 *
 * @return the source of this move.
 */
Terminal&
Move::source() const {
    return *src_;
}

/**
 * Sets a new source for the move and deletes the old one.
 *
 * @param src The new source.
 */
void
Move::setSource(Terminal* src) {
    if (src_ != &NullTerminal::instance()) {
        delete src_;
    }
    src_ = src;
}

/**
 * Returns the destination of this move.
 *
 * @return the destination of this move.
 */
Terminal&
Move::destination() const {
    return *dst_;
}

/**
 * Sets a new destination for the move and deletes the old one.
 *
 * @param dst The new destination.
 */
void
Move::setDestination(Terminal* dst) {
    if (dst_ != &NullTerminal::instance()) {
        delete dst_;
    }
    dst_ = dst;
}

/**
 * Returns the Boolean expression that guards this move.
 *
 * @return the Boolean expression that guards this move.
 * @exception InvalidData if the move is not predicated.
 */
MoveGuard& Move::guard() const throw (InvalidData) {
    if (guard_ != &NullMoveGuard::instance()) {
        return *guard_;
    } else {
        throw InvalidData(
            __FILE__, __LINE__, "Move::guard()", "Move is not predicated.");
    }
}

/**
 * Sets a new guard for the move and deletes the old one.
 *
 * @param guard The new guard.
 */
void
Move::setGuard(MoveGuard* guard) {
    if (guard_ != &NullMoveGuard::instance()) {
        delete guard_;
    }
    guard_ = guard;
}

/**
 * Returns the bus on which the move is carried.
 *
 * @return the bus on which the move is carried.
 */
Bus&
Move::bus() const {
    return *bus_;
}

/**
 * Sets a new bus for the move.
 *
 * @param bus The new bus.
 */
void
Move::setBus(TTAMachine::Bus& bus) {
    bus_ = &bus;
}

Socket&
Move::destinationSocket() const {
    return *dst_->port().inputSocket();
}

Socket&
Move::sourceSocket() const 
    throw (WrongSubclass) {
    if (src_->isImmediate()) {
        throw WrongSubclass(
            __FILE__, __LINE__, "Move::sourceSocket()",
            "Move source is an immediate.");
    } else {
        return *src_->port().outputSocket();
    }
}

/**
 * Makes a copy of the move.
 *
 * The copy is identical, except that it is not registered to the
 * instruction of the original move (and therefore, any address it
 * refers to is not meaningful).
 *
 * @return A copy of the move.
 */
Move*
Move::copy() const {

    Move* newMove = NULL;
    if (isUnconditional()) {
        newMove = new Move(src_->copy(), dst_->copy(), *bus_);
    } else {
        newMove = new Move(
            src_->copy(), dst_->copy(), *bus_, guard_->copy());
    }

    for (int i = 0; i < annotationCount(); i++ ) {
        newMove->addAnnotation(annotation(i));
    }

    newMove->setParent(NullInstruction::instance());

    return newMove;
}

}
