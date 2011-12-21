/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SimProgramBuilder.cc
 *
 * Definition of SimProgramBuilder class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005, 2010
 * @note rating: red
 */

#include "boost/format.hpp"

#include "SimProgramBuilder.hh"

#include "Instruction.hh"
#include "NullInstruction.hh"
#include "Immediate.hh"
#include "Terminal.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "Address.hh"
#include "FUPort.hh"
#include "StateLocator.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "ExecutableMove.hh"
#include "BuslessExecutableMove.hh"
#include "LongImmediateRegisterState.hh"
#include "LongImmediateUnitState.hh"
#include "LongImmUpdateAction.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "MachineState.hh"
#include "InlineImmediateValue.hh"
#include "RegisterFileState.hh"
#include "PortState.hh"
#include "BusState.hh"
#include "StringTools.hh"
#include "Operation.hh"
#include "Port.hh"
#include "BaseFUPort.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Guard.hh"
#include "ControlUnit.hh"
#include "UniversalMachine.hh"
#include "TerminalImmediate.hh"
#include "GuardState.hh"
#include "Procedure.hh"
#include "POMDisassembler.hh"
#include "TCEString.hh"
#include "MathTools.hh"
#include "Program.hh"

using namespace TTAMachine;
using namespace TTAProgram;

// This should be defined in case control flow operation moves should be
// visible in bus. This is not the case in real hardware (verify?). This
// define is here to be able to compare TCE bustrace with MOVE bustrace 
// easily (which makes the moves visible in transport bus).
#define GCU_OPERATION_MOVES_VISIBLE_IN_BUS true

/**
 * Constructor.
 */
SimProgramBuilder::SimProgramBuilder() : 
    sequentialProgram_(false) {
}

/**
 * Destructor.
 */
SimProgramBuilder::~SimProgramBuilder() {
}

/**
 * Helper method for processing such terminals that can be both source and
 * destination in the move.
 *
 * @param theTerminal The terminal to process.
 * @param state MachineState to fetch the state objects from.
 * @return The StateData object created from the terminal.
 * @exception IllegalProgram When the terminal is referring to illegal
 *                           machine state or the terminal itself is illegal.
 */
StateData* 
SimProgramBuilder::processBidirTerminal(
    const Terminal& theTerminal, 
    MachineState& state) throw (IllegalProgram) {

    try {
        if (theTerminal.isGPR()) {
            
            RegisterFileState& unit = state.registerFileState(
                theTerminal.registerFile().name());
            if (&unit == &NullRegisterFileState::instance()) {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, 
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_ILLEGAL_PROGRAM_RF_STATE_NOT_FOUND).str());
            }
            return &unit.registerState(theTerminal.index());
            
        } else if (theTerminal.isFUPort()) {
            
            const BaseFUPort* thePort = 
                dynamic_cast<const BaseFUPort*>(&theTerminal.port());

            if (thePort == NULL)
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, 
                    "Could not find the FU port.");

            std::string operationString = "";
            // if this is an opcode-setting port, find the
            // OpcodeSettingVirtualInputPort instead of a real port

            if (thePort->isOpcodeSetting()) {
                
                if (!theTerminal.isOpcodeSetting()) {
                    throw IllegalProgram(
                        __FILE__, __LINE__, __func__, 
                        SimulatorToolbox::textGenerator().text(
                            Texts::TXT_ILLEGAL_PROGRAM_OPCODE_NOT_FOUND).
                        str());
                }
                operationString = 
                    StringTools::stringToLower(
                        std::string(".") + theTerminal.operation().name());
            }

            PortState& port = state.portState(
                thePort->name() + operationString, 
                theTerminal.port().parentUnit()->name());

            if (&port == &NullPortState::instance()) {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, 
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_ILLEGAL_PROGRAM_PORT_STATE_NOT_FOUND).
                    str());
            }
            
            return &port;

        } else {
            abortWithError("Illegal terminal!");
        }
    } catch (const OutOfRange& e) {
        abortWithError(e.errorMessage());
    } catch (const WrongSubclass&) {
        abortWithError("Wrong sub class!");
    }

    std::string errorMsg = 
        SimulatorToolbox::textGenerator().text(
        Texts::TXT_ILLEGAL_PROGRAM_UNKNOWN_MOVE_TERMINAL_TYPE).str();
    throw IllegalProgram(__FILE__, __LINE__, __func__, errorMsg);
    return NULL;
}

/**
 * Helper method for processing source terminals.
 *
 * @param theTerminal The terminal to process.
 * @param state MachineState to fetch the state objects from.
 * @return The StateData object created from the terminal.
 * @exception IllegalProgram When the terminal is referring to illegal
 *                           machine state or the terminal itself is illegal.
 */
StateData* 
SimProgramBuilder::processSourceTerminal(
    const Terminal& theTerminal, 
    MachineState& state)
    throw (IllegalProgram) {

    try {
        if (theTerminal.isImmediateRegister()) {
            
            LongImmediateUnitState& unit = state.longImmediateUnitState(
                theTerminal.immediateUnit().name());
            if (&unit == &NullLongImmediateUnitState::instance()) {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, 
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND).str());
            }
            return &unit.immediateRegister(theTerminal.index());
        } else {
            return processBidirTerminal(theTerminal, state);
        }
    } catch (const OutOfRange&) {
    } catch (const WrongSubclass&) {
    }
    throw IllegalProgram(
        __FILE__, __LINE__, __func__, 
        SimulatorToolbox::textGenerator().text(
            Texts::TXT_ILLEGAL_PROGRAM_UNKNOWN_MOVE_TERMINAL_TYPE).str());
    return NULL;
}

/**
 * Helper method for finding the state object the given guard targets.
 *
 * @param guard The guard object from machine object model.
 * @param state The machine state model.
 * @return The state object represeting the current value of the guard.
 * @exception IllegalProgram In case a state model for given guard cannot be
 *            found.
 */
const ReadableState& 
SimProgramBuilder::findGuardModel(
    const TTAMachine::Guard& guard, 
    MachineState& state) 
    throw (IllegalProgram) {
    
    try {
        GuardState& guardModel = state.guardState(guard);
        if (&guardModel != &NullGuardState::instance()) 
            return guardModel;
    } catch (const Exception& e) {
    }
    
    throw IllegalProgram(
        __FILE__, __LINE__, __func__, 
        "Program references an illegal guard.");

    // should never return from here
    return NullGuardState::instance();
}

/**
 * Processes a POM Move and produces an ExecutableMove.
 *
 * @param move The move to process.
 * @param state The MachineState the program refers to.
 * @return The processed move.
 * @exception IllegalProgram If the input move was illegal.
 */
ExecutableMove*
SimProgramBuilder::processMove(
    const Move& move, 
    MachineState& state) 
    throw (IllegalProgram) {

    ReadableState* source = NULL;
    InlineImmediateValue* immediateSource = NULL;

    if (move.source().isImmediate()) {
        // extend the immediate so we don't have to extend it
        // during runtime every time the move is simulated
        immediateSource = new InlineImmediateValue(move.bus().width());
        int immediate = move.source().value().sIntWordValue();

        if (move.isControlFlowMove()) {
            int targetWidth = move.destination().port().width();
            if (targetWidth < MathTools::requiredBits(immediate)) {
                TCEString errorMsg = 
                    (boost::format(
                        "Immediate of jump '%s' gets clipped due to the target "
                        "port being too narrow. Wrong execution would occur.") 
                     % move.toString()).str();
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, errorMsg);

            }
        }

        if (move.bus().signExtends()) {
            immediate = 
                MathTools::signExtendTo(
                    immediate, move.bus().immediateWidth());
        } else {
            immediate = 
                MathTools::zeroExtendTo(
                    immediate, move.bus().immediateWidth());
        }  
     
        SimValue val(immediate, move.bus().width());
        immediateSource->setValue(val);
    }  else {
        source = processSourceTerminal(move.source(), state);
    }
    
    BusState& bus = state.busState(move.bus().name());
    if (&bus == &NullBusState::instance()) {
        throw IllegalProgram(
	    __FILE__, __LINE__, __func__, 
	    SimulatorToolbox::textGenerator().text(
            Texts::TXT_ILLEGAL_PROGRAM_BUS_STATE_NOT_FOUND).str());	
    }
    
    WritableState& destination = 
        *processBidirTerminal(move.destination(), state);

    // in case this is a sequential move, or a move that executes a
    // control flow operation with immediate or control unit port source (RA)
    // it does not utilize any bus
    bool buslessMove = sequentialProgram_;
    if (!GCU_OPERATION_MOVES_VISIBLE_IN_BUS && !sequentialProgram_) {
        if (move.destination().isFUPort()) {
            if (dynamic_cast<const TTAMachine::ControlUnit*>(
                    &move.destination().functionUnit())) {
                if (move.source().isImmediate() || 
                    (move.source().isFUPort() &&
                     dynamic_cast<const TTAMachine::ControlUnit*>(
                         &move.source().functionUnit()) != NULL)) {
                    buslessMove = true;
                }
            }
        }
    }

    // handle guards
    if (move.isUnconditional()) {
        // create a guardless execmove
        if (move.source().isImmediate()) {
            assert(immediateSource != NULL);
            if (buslessMove) {
                return new BuslessExecutableMove(immediateSource, destination);
            } else {
                return new ExecutableMove(immediateSource, bus, destination);
            }
        } else {
            assert(source != NULL);
            if (buslessMove) {
                return new BuslessExecutableMove(*source, destination);
            } else {
                return new ExecutableMove(*source, bus, destination);
            }
        }
    } else {
        // create a guarded move
        MoveGuard& guard = move.guard();
        
        if (move.source().isImmediate()) {
            assert(immediateSource != NULL);
            if (buslessMove) {
                return new BuslessExecutableMove(
                    immediateSource, destination, 
                    findGuardModel(guard.guard(), state), 
                    guard.guard().isInverted());
            } else {
                return new ExecutableMove(
                    immediateSource, bus, destination, 
                    findGuardModel(guard.guard(), state), 
                    guard.guard().isInverted());
            }
        } else {
            assert(source != NULL);
            if (buslessMove) {
                return new BuslessExecutableMove(
                    *source, destination,
                    findGuardModel(guard.guard(), state), 
                    guard.guard().isInverted());                
            } else {
                return new ExecutableMove(
                    *source, bus, destination,
                    findGuardModel(guard.guard(), state), 
                    guard.guard().isInverted());
            }
        }
    }

    // should never end up here
    assert(false);
    
    return NULL;
}

/**
 * Processes a POM Instruction and produces an ExecutableInstruction.
 *
 * @param instruction The instruction to process.
 * @param state The MachineState the program refers to.
 * @return The processed instruction.
 * @exception IllegalProgram If the input instruction was illegal.
 */
ExecutableInstruction*
SimProgramBuilder::processInstruction(
    const Instruction& instruction, 
    MachineState& state) 
    throw (IllegalProgram) {
    
    ExecutableInstruction* processedInstruction = new ExecutableInstruction();

    // process long immediates
    for (int i = 0; i < instruction.immediateCount(); ++i) {
        assert(instruction.immediate(i).destination().isImmediateRegister());

        // first fetch the LongImmediateUnitState, then fetch the
        // LongImmediateRegisterState from it, because there is no such 
        // object in MOM as LongImmediateRegister, nor any single registers
        LongImmediateRegisterState* destination = NULL;
        try {
            LongImmediateUnitState& targetUnit = state.longImmediateUnitState(
                instruction.immediate(i).destination().immediateUnit().name());

            destination = &targetUnit.immediateRegister(
                instruction.immediate(i).destination().index());

            if (&targetUnit == &NullLongImmediateUnitState::instance()) {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, 
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND).str());
            }
        } catch (const IllegalParameters&) {
            throw IllegalProgram(
                __FILE__, __LINE__, __func__, 
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND).str());
        } catch (const OutOfRange&) {
            throw IllegalProgram(
                __FILE__, __LINE__, __func__, 
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_ILLEGAL_PROGRAM_IU_STATE_NOT_FOUND).str());
        }

        processedInstruction->addLongImmediateUpdateAction(
            new LongImmUpdateAction(instruction.immediate(i).value().value(),
                                    *destination));
    }
    
    // process all the moves in the instruction
    for (int j = 0; j < instruction.moveCount(); ++j) {
        try {
            processedInstruction->addExecutableMove(
                processMove(instruction.move(j), state));
        } catch (const IllegalProgram& e) {
            IllegalProgram ip(
                e.fileName(), e.lineNum(), e.procedureName(),
                e.errorMessage() + " Illegal move: " + 
                POMDisassembler::disassemble(instruction.move(j)));
            throw ip;
        }
    }
    return processedInstruction;
}

/**
 * Builds the model of instruction memory ready to be simulated.
 *
 * @param prog Program from which the contents of instruction memory is built.
 * @param state The MachineState the program refers to.
 * @return The created instruction memory, owned by client.
 * @exception IllegalProgram If building fails for some reason. The exception
 *                           message describes the reason. The message can be 
 *                           displayed in user interfaces as it's generated by
 *                           SimulatorTextGenerator.
 */
InstructionMemory*
SimProgramBuilder::build(
    const Program& prog, 
    MachineState& state) 
    throw (IllegalProgram) {

    sequentialProgram_ = 
        (dynamic_cast<const UniversalMachine*>(
            &prog.targetProcessor()) != NULL);
    const Address& programStartAddress = prog.startAddress();

    InstructionMemory* memory = 
        new InstructionMemory(programStartAddress.location());

    const TTAProgram::Instruction* currentInstruction = NULL;
    try {

        // Traverse all instructions of the Program quickly.
        for (int procIndex = 0; procIndex < prog.procedureCount(); 
             ++procIndex) {
            const Procedure& proc = prog.procedureAtIndex(procIndex);
            for (int instrIndex = 0; instrIndex < proc.instructionCount(); 
                 ++instrIndex) {
                currentInstruction = 
                    &proc.instructionAtIndex(instrIndex);
                ExecutableInstruction* processedInstruction = 
                    processInstruction(*currentInstruction, state);
                memory->addExecutableInstruction(processedInstruction);
            }
        }
    } catch (const Exception& e) {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__, 
            (boost::format("Error while processing instruction %d "
                           "(with moves %s): %s") 
             % currentInstruction->address().location()
             % POMDisassembler::disassemble(*currentInstruction) 
             % e.errorMessage()).str());
    }

    return memory;
}
