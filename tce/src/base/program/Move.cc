/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file Move.cc
 *
 * Implementation of Move class.
 *
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <climits>

#include "Move.hh"
#include "Socket.hh"
#include "Port.hh"
#include "NullInstruction.hh"
#include "TerminalFUPort.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "TCEString.hh"
#include "POMDisassembler.hh"
#include "MoveGuard.hh"
#include "Guard.hh"

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
    const Bus& bus, MoveGuard* guard):
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
    Terminal* src, Terminal* dst, const Bus& bus):
    parent_(NULL), src_(src), dst_(dst), bus_(&bus),
    guard_(NULL) {
}

/**
 * The destructor.
 */
Move::~Move() {
    delete dst_;
    dst_ = NULL;

    delete src_;
    src_ = NULL;

    if (guard_ != NULL) {
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
Move::parent() const {
    if (parent_ != NULL && parent_ != &NullInstruction::instance()) {
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
    return parent_ != NULL && parent_ != &NullInstruction::instance();
}

/**
 * Tells whether the move is predicated or not.
 *
 * @return true if this move is not predicated.
 */
bool
Move::isUnconditional() const {
    return (guard_ == NULL);
}

/**
 * Tells whether the move triggers a jump operation.
 *
 * @return true if this move triggers a jump operation.
 */
bool
Move::isJump() const {

    if (dst_->isFUPort()) {
        // there is already check. no need for double-check.
        const TerminalFUPort* tfup = static_cast<const TerminalFUPort*>(dst_);
        if (tfup->isOpcodeSetting()) {
            if (tfup->operation().isBranch()) {
                return true;
            }            
        } else {
            Operation* tmpOp = &tfup->hintOperation();
            if (tmpOp != &NullOperation::instance() &&
                tfup->hintOperation().isBranch()) {
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
        // there is already check. no need for double-check.
        const TerminalFUPort* tfup = static_cast<const TerminalFUPort*>(dst_);
        if (tfup->isOpcodeSetting()) {
            if (tfup->operation().isCall()) {
                return true;
            }            
        } else {
            Operation* tmpOp = &tfup->hintOperation();
            if (tmpOp != &NullOperation::instance() &&
                tfup->hintOperation().isCall()) {
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
        } else {
            Operation* tmpOp = &tfup->hintOperation();
            if (tmpOp != &NullOperation::instance() &&
                tfup->hintOperation().isControlFlowOperation()) {
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
 */
bool
Move::isReturn() const {

    if (!isJump()) {
        return false;
    }

    // direct gcu.ra -> gcu.jump.1 is considered a return
    if (source().isRA()) {
        return true;
    }

    if (hasAnnotations(ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN)) {
        return true;
    }
    
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
    delete src_;
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
    delete dst_;
    dst_ = dst;
}

/**
 * Returns the Boolean expression that guards this move.
 *
 * @return the Boolean expression that guards this move.
 * @exception InvalidData if the move is not predicated.
 */
MoveGuard&
Move::guard() const {
    if (guard_ != NULL) {
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
    if (guard_ != NULL) {
        delete guard_;
    }
    guard_ = guard;
}

/**
 * Returns the bus on which the move is carried.
 *
 * @return the bus on which the move is carried.
 */
const Bus&
Move::bus() const {
    return *bus_;
}

/**
 * Sets a new bus for the move.
 *
 * @param bus The new bus.
 */
void
Move::setBus(const TTAMachine::Bus& bus) {
    bus_ = &bus;
}

Socket&
Move::destinationSocket() const {
    return *dst_->port().inputSocket();
}

Socket&
Move::sourceSocket() const {
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
std::shared_ptr<Move>
Move::copy() const {

    std::shared_ptr<Move> newMove = NULL;
    if (isUnconditional()) {
        newMove = std::make_shared<Move>(src_->copy(), dst_->copy(), *bus_);
    } else {
        newMove = std::make_shared<Move>(
            src_->copy(), dst_->copy(), *bus_, guard_->copy());
    }

    for (int i = 0; i < annotationCount(); i++ ) {
        newMove->addAnnotation(annotation(i));
    }

    newMove->setParent(NullInstruction::instance());

    return newMove;
}

/**
 * Returns the disassembly of the move.
 */
std::string
Move::toString() const {
    return POMDisassembler::disassemble(*this);
}

/**
 * Returns true in case at least one source code line number
 * is known for this move.
 */
bool
Move::hasSourceLineNumber() const {
    return sourceLineNumber() != -1;
}

/**
 * Returns one source code line number for this move.
 *
 * There can be at most two source code line numbers associated
 * to a move in case it's bypassed. This method returns the smaller of
 * them.
 * 
 * Returns -1 if no source code line info has been set.
 */
int
Move::sourceLineNumber() const {
    int lineNumber =  -1;
    const TTAProgram::ProgramAnnotation::Id id = 
        TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_LINE;
    if (hasAnnotations(id)) {
        lineNumber = INT_MAX;
        for (int i = 0; i < annotationCount(id); ++i) {            
            lineNumber = std::min(lineNumber, annotation(i, id).intValue());
        }
    }
    return lineNumber;
}


/**
 * Returns true in case at least one source code filename
 * is known for this move.
 */
bool
Move::hasSourceFileName() const {
    return sourceFileName() != "";
}

/**
 * Returns one source code filename for this move.
 * 
 * Returns "" if no source code line info has been set.
 */
std::string
Move::sourceFileName() const {
    std::string fileName = "";
    const TTAProgram::ProgramAnnotation::Id id = 
        TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_PATH;
    if (hasAnnotations(id)) {		
        fileName=annotation(0,id).stringValue();
    }
    return fileName; 
  
}
/**
 * Returns the total guard latency of the guard of given move,
 * or 0 if the move is unconditional.
 */
int
Move::guardLatency() const {
    if (!isUnconditional()) {
        
        const TTAMachine::Guard& g = guard().guard();
        const TTAMachine::RegisterGuard* rg =
        dynamic_cast<const TTAMachine::RegisterGuard*>(&g);
        if (rg != NULL) {
            const TTAMachine::RegisterFile& rf =
            *rg->registerFile();
            return rf.guardLatency() +
            rf.machine()->controlUnit()->globalGuardLatency();
        } else {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Scheduling FU output port guards is not yet supported.");
        }
    } else {
        return 0;
    }
}

}
