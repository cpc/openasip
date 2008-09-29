/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
    machine_(machine), program_(program) {

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

    const Instruction* instruction = &program_.firstInstruction();

    // Test all isntructions in the program.
    while (instruction != &NullInstruction::instance()) {

        // Test all moves in the current instruction.
        for (int i = 0; i < instruction->moveCount(); i++) {
            const Move& move = instruction->move(i);

            const Bus& bus = move.bus();
            InstructionAddress address = instruction->address().location();

            // Test move source connectivity.
            if (move.source().isGPR() ||
                move.source().isFUPort() ||
                move.source().isImmediateRegister()) {

                const Port& port = move.source().port();

                if (port.outputSocket() == NULL) {
                    // ERROR: Source port is not connected to an output socket.
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Source port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to an output socket.";

                    results.addError(CONNECTION_MISSING, errorMessage);

                } else if (!port.outputSocket()->isConnectedTo(bus)) {
                    // ERROR: Output socket of the source port is not connected
                    // to the move bus.
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
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Destination port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to an input socket.";

                    results.addError(CONNECTION_MISSING, errorMessage);

                } else if (!port.inputSocket()->isConnectedTo(bus)) {
                    // ERROR: Output socket of the target port is not connected
                    // to the move bus.
                    std::string errorMessage =
                        Conversion::toString(address) + ": " +
                        "Destination port '" + port.parentUnit()->name() +
                        "." + port.name() + "' of the move on bus '" +
                        bus.name() + "' is not connected to the bus.";
                    results.addError(CONNECTION_MISSING, errorMessage);
                }

            }
        }

        instruction = &(program_.nextInstruction(*instruction));
    }
}

/**
 * Checks long immediate assignments.
 *
 * @param results POMValidator results where the error messages are added.
 */
void
POMValidator::checkLongImmediates(POMValidatorResults& results) {

    const Instruction* instruction = &program_.firstInstruction();

    // Test all isntructions in the program.
    while (instruction != &NullInstruction::instance()) {

        InstructionAddress address = instruction->address().location();

        const InstructionTemplate& templ =
            instruction->instructionTemplate();

        std::set<std::string> destinations;
        for (int i = 0; i < instruction->immediateCount(); i++) {

            const Immediate& immediate = instruction->immediate(i);

            // Check that the immediate destination terminal is an IU.
            if (!immediate.destination().isImmediateRegister()) {
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
        instruction = &(program_.nextInstruction(*instruction));
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
    
    Instruction* instruction = &program_.firstInstruction();
    std::set<std::string> errOpNames;

    while (instruction != &NullInstruction::instance()) {

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
        instruction = &(program_.nextInstruction(*instruction));
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
    Instruction* instruction = &program_.firstInstruction();
    while (instruction != &NullInstruction::instance()) {
        for (int i = 0; i < instruction->moveCount(); i++) {
            Move& move = instruction->move(i);
            Terminal* destination = &move.destination();
            InstructionAddress address = instruction->address().location();
#if 0
            // jump or call to a possibly unknown target
            if (move.source().isGPR() && move.isControlFlowMove()) {                    
                std::string errorMessage =
                    "Instruction at address: " + 
                    Conversion::toString(address) +
                    "' cannot be simulated with the compiled simulator. ";
                    
                results.addError(COMPILED_SIMULATION_NOT_POSSIBLE,
                    errorMessage);
            }
#endif
            // clocked operations
            if (destination->isFUPort()) {
                if (destination->isOpcodeSetting() && 
                    destination->operation().isClocked()) {
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
        instruction = &(program_.nextInstruction(*instruction));
    }
}
