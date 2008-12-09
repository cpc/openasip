/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file POMValidator.cc
 *
 * Implementation of POMValidator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "POMValidator.hh"
#include "POMValidatorResults.hh"
#include "Machine.hh"
#include "Socket.hh"
#include "Port.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Immediate.hh"
#include "TerminalImmediate.hh"
#include "BaseType.hh"
#include "Operation.hh"
#include "ContainerTools.hh"
#include "ControlUnit.hh"
#include "TCEString.hh"
#include "MathTools.hh"

using namespace TTAMachine;
using namespace TTAProgram;

/**
 * The Constructor.
 *
 * @param machine Machine to validate the program against.
 * @param program POM to validate.
 */
POMValidator::POMValidator(
    const TTAMachine::Machine& machine, const TTAProgram::Program& program):
    machine_(machine), program_(program), instructions_(program.instructionVector()) {

}

/**
 * The Destructor.
 */
POMValidator::~POMValidator() {
}

/**
 * Checks the program against the target machine for given error check set.
 *
 * @param errorsToCheck Set of error codes which are checked.
 * @return POMValidatorResults object contaning possible error messages.
 */
POMValidatorResults*
POMValidator::validate(const std::set<ErrorCode>& errorsToCheck) {

    POMValidatorResults* results = new POMValidatorResults();
    for (std::set<ErrorCode>::const_iterator iter = errorsToCheck.begin();
         iter != errorsToCheck.end(); iter++) {
        ErrorCode code = *iter;
        if (code == CONNECTION_MISSING) {
            checkConnectivity(*results);
        } else if (code == LONG_IMMEDIATE_NOT_SUPPORTED) {
            checkLongImmediates(*results);
        } else if (code == SIMULATION_NOT_POSSIBLE) {
            checkSimulatability(*results);
        } else if (code == COMPILED_SIMULATION_NOT_POSSIBLE) {
            checkCompiledSimulatability(*results);
        } else {
            assert(false);
        }
    }

    return results;
}


/**
 * Checks if the target machine has connectivity needed for program moves.
 *
 * @param results POMValidator results where the error messages are added.
 */
void
POMValidator::checkConnectivity(POMValidatorResults& results) {

    for (std::size_t instrI = 0; instrI < instructions_.size(); ++instrI) {
        const Instruction* instruction = instructions_.at(instrI);
        // Test all moves in the current instruction.
        for (int i = 0; i < instruction->moveCount(); i++) {
            const Move& move = instruction->move(i);

            const Bus& bus = move.bus();

            // Test move source connectivity.
            if (move.source().isGPR() ||
                move.source().isFUPort() ||
                move.source().isImmediateRegister()) {

                const Port& port = move.source().port();

                if (port.outputSocket() == NULL) {
                    // ERROR: Source port is not connected to an output socket.
                    InstructionAddress address = instruction->address().location();
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Source port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to an output socket.";

                    results.addError(CONNECTION_MISSING, errorMessage);

                } else if (!port.outputSocket()->isConnectedTo(bus)) {
                    // ERROR: Output socket of the source port is not connected
                    // to the move bus.
                    InstructionAddress address = instruction->address().location();
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Source port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to the bus.";
                    results.addError(CONNECTION_MISSING, errorMessage);
                }

            }

            // Test move target connectivity.
            if (move.destination().isGPR() ||
                move.destination().isFUPort() ||
                move.destination().isImmediateRegister()) {

                const Port& port = move.destination().port();

                if (port.inputSocket() == NULL) {
                    // ERROR: Destination port is not connected to an output
                    // socket.
                    InstructionAddress address = instruction->address().location();
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Destination port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to an input socket.";

                    results.addError(CONNECTION_MISSING, errorMessage);

                } else if (!port.inputSocket()->isConnectedTo(bus)) {
                    // ERROR: Output socket of the target port is not connected
                    // to the move bus.
                    InstructionAddress address = instruction->address().location();
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Destination port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to the bus.";
                    results.addError(CONNECTION_MISSING, errorMessage);
                }

            }
        }
    }
}

/**
 * Checks long immediate assignments.
 *
 * @param results POMValidator results where the error messages are added.
 */
void
POMValidator::checkLongImmediates(POMValidatorResults& results) {

    for (std::size_t instrI = 0; instrI < instructions_.size(); ++instrI) {
        const Instruction* instruction = instructions_.at(instrI);

        const InstructionTemplate& templ =
            instruction->instructionTemplate();

        std::set<std::string> destinations;
        for (int i = 0; i < instruction->immediateCount(); i++) {

            const Immediate& immediate = instruction->immediate(i);

            // Check that the immediate destination terminal is an IU.
            if (!immediate.destination().isImmediateRegister()) {
                InstructionAddress address = instruction->address().location();
                std::string errorMessage =
                    Conversion::toString(address) + ": " +
                    "Long immediate destination terminal is not an "
                    "immediate unit.";
                results.addError(LONG_IMMEDIATE_NOT_SUPPORTED, errorMessage);
                continue;
            }

            int width = immediate.value().value().width();
            const ImmediateUnit& iu =
                immediate.destination().immediateUnit();
            int supportedWidth = templ.supportedWidth(iu);
            if (width <= INT_WORD_SIZE &&                 
                supportedWidth != width) {
                // Raw magic! If the SimValue reports width higher then
                // INT_WORD_SIZE it means immediate is floating point,
                // and should be just respected:-)
                // If width is less, the actuall width is recomputed
                // based on target as if it was integer
                // FIXME: find out how to deal with floating point
                if (iu.extensionMode() == Machine::ZERO) {
                    // Actual required width depends on target immediate unit
                    width = MathTools::requiredBits(
                        immediate.value().value().unsignedValue());
                } 
                if (iu.extensionMode() == Machine::SIGN) {
                    // Actual required width depends on target immediate unit
                    width = MathTools::requiredBitsSigned(
                        immediate.value().value().intValue());
                }
            }
            // Check that the destination IU isn't already destination
            // of a long immediate in the current instruction.
            std::pair<std::set<std::string>::iterator, bool> result =
                destinations.insert(iu.name());

            if (!result.second) {
                // Destination already in the set.
                InstructionAddress address = instruction->address().location();
                std::string errorMessage =
                    Conversion::toString(address) + ": " +
                    "Multiple long immediates with destination IU '" +
                    iu.name() + "'.";
                results.addError(LONG_IMMEDIATE_NOT_SUPPORTED, errorMessage);
                continue;
            }

            // Check that the instruction template supports long immediates
            // with the current long immediate's width and destination IU.
            if (supportedWidth < width) {
                InstructionAddress address = instruction->address().location();
                std::string errorMessage =
                    Conversion::toString(address) + ": " +
                    "Long immediate with destination IU '" +
                    iu.name() + "' has width of " +
                    Conversion::toString(width) + " bits. Instruction's " +
                    "template only supports long immediates with width of " +
                    Conversion::toString(templ.supportedWidth(iu)) +
                    " bits or less. For template " + templ.name();
                results.addError(LONG_IMMEDIATE_NOT_SUPPORTED, errorMessage);
                continue;
            }
        }
    }
}

/**
 * Checks that all operations in the program have known behaviour and
 * can be simulated.
 *
 * @param results POMValidator results where the error messages are added.
 */
void
POMValidator::checkSimulatability(POMValidatorResults& results) {
    
    std::set<std::string> errOpNames;

    for (std::size_t instrI = 0; instrI < instructions_.size(); ++instrI) {
        const Instruction* instruction = instructions_.at(instrI);

        for (int i = 0; i < instruction->moveCount(); i++) {
            Move& move = instruction->move(i);
            Terminal* destination = &move.destination();
            if (destination->isFUPort()) {
                if (destination->isOpcodeSetting() &&
                    !destination->operation().canBeSimulated() &&
                    !ContainerTools::containsValue(
                        errOpNames, destination->operation().name())) {

                    std::string opName =  destination->operation().name();
                    InstructionAddress address =
                        instruction->address().location();

                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Operation '" + opName +
                        "' cannot be simulated.";

                    results.addError(
                            SIMULATION_NOT_POSSIBLE, errorMessage);

                    errOpNames.insert(opName);
                }
            }
        }
    }
}

/**
 * Checks for problems specific to compiled simulation only.
 * 
 * Used to check if the program cannot be simulated with the compiled simulator.
 * 
 * @param results POMValidator results where the error messages are added.
 * 
 */
void 
POMValidator::checkCompiledSimulatability(POMValidatorResults& results) {    

    for (std::size_t instrI = 0; instrI < instructions_.size(); ++instrI) {
        const Instruction* instruction = instructions_.at(instrI);
        for (int i = 0; i < instruction->moveCount(); i++) {
            Move& move = instruction->move(i);
            Terminal* destination = &move.destination();
            // clocked operations
            if (destination->isFUPort()) {
                if (destination->isOpcodeSetting() && 
                    destination->operation().isClocked()) {
                    InstructionAddress address = instruction->address().location();
                    std::string errorMessage =
                        "Instruction at address: " +
                        Conversion::toString(address) +
                        "' cannot be simulated with the compiled simulator. "
                        "(Operation " + destination->operation().name() + 
                        " is a clocked operation).";
                    
                    results.addError(COMPILED_SIMULATION_NOT_POSSIBLE,
                    errorMessage);
                }
            }
        } // end for
    }
}
