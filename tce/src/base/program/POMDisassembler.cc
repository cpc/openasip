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
 * @file POMDisassembler.cc
 *
 * Implementation of POMDisassembler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <boost/format.hpp>

#include "POMDisassembler.hh"
#include "Application.hh"
#include "StringTools.hh"

#include "Instruction.hh"
#include "NullInstruction.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "TerminalProgramOperation.hh"
#include "Procedure.hh"
#include "Operation.hh"
#include "GlobalScope.hh"
#include "CodeLabel.hh"
#include "UniversalMachine.hh"
#include "UniversalFunctionUnit.hh"
#include "ControlUnit.hh"
#include "UnboundedRegisterFile.hh"
#include "Guard.hh"
#include "Machine.hh"
#include "TCEString.hh"
#include "InstructionReference.hh"
#include "Move.hh"
#include "Instruction.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Immediate.hh"
#include "MoveGuard.hh"
#include "HWOperation.hh"
#include "Conversion.hh"

#include "DisassemblyMove.hh"
#include "DisassemblyRegister.hh"
#include "DisassemblyFUPort.hh"
#include "DisassemblyFUOpcodePort.hh"
#include "DisassemblyImmediate.hh"
#include "DisassemblyImmediateRegister.hh"
#include "DisassemblyIntRegister.hh"
#include "DisassemblyFPRegister.hh"
#include "DisassemblyBoolRegister.hh"
#include "DisassemblyOperand.hh"
#include "DisassemblyInstruction.hh"
#include "DisassemblyControlUnitPort.hh"
#include "DisassemblyImmediateAssignment.hh"
#include "DisassemblyLabel.hh"
#include "DisassemblyNOP.hh"
#include "DisassemblySequentialGuard.hh"
#include "NullInstructionTemplate.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

/**
 * The constructor.
 *
 * @param program Program to disassemble.
 */
POMDisassembler::POMDisassembler(const Program& program):
    Disassembler(),
    program_(program) {
}


/**
 * The destructor.
 */
POMDisassembler::~POMDisassembler() {
}


/**
 * Returns disassembly of an instruction at the given index.
 *
 * The client is responsible for destroying the instruction
 * object.
 *
 * @param index Index of the instruction to disassemble.
 * @return Disassembly of the instruction at given index.
 */
DisassemblyInstruction*
POMDisassembler::createInstruction(Word index) const {

    const Instruction& instruction = program_.instructionAt(index);
    return createInstruction(program_, instruction);
}


/**
 * Returns disassembly of an instruction at the given index.
 *
 * The client is responsible for destroying the instruction
 * object.
 *
 * @param program Parent program of the instruction.
 * @param index Index of the instruction to disassemble.
 * @return Disassembly of the instruction at given index.
 */
DisassemblyInstruction*
POMDisassembler::createInstruction(
    const Program& program, const Instruction& instruction) {

    DisassemblyInstruction* disassembly = new DisassemblyInstruction();

    const Machine::BusNavigator& navigator =
        program.targetProcessor().busNavigator();

    // The instruction disassembly is created by one bus at a time.
    for (int i = 0; i < navigator.count(); i++) {

        Bus* bus = navigator.item(i);
        string busName = bus->name();
        bool isUsed = false;

        // Check for move.
        for (int i = 0; i < instruction.moveCount(); i++) {
            if (&instruction.move(i).bus() == bus) {
                disassembly->addMove(createMove(instruction.move(i)));
                isUsed = true;
            }
        }

        if (!isUsed) {
            disassembly->addMove(new DisassemblyNOP);
        }
    }

    // Add disassembly of long immediates in immediate slots.
    for (int i = 0; i < instruction.immediateCount(); i++) {
        disassembly->addLongImmediate(
            createImmediateAssignment(instruction.immediate(i)));                
    }

    return disassembly;
}


/**
 * Creates disassembly of a move.
 *
 * @param move Move to disassemble.
 * @return Disassembly of the move.
 */
DisassemblyMove*
POMDisassembler::createMove(const Move& move) {

    // Disassemble move terminals.
    DisassemblyElement* destinationDis = createTerminal(move.destination());
    DisassemblyElement* sourceDis = NULL;

    if (move.source().isImmediate()) {

        // Special case: if the move source is an inline immediate, and
        // the move destination is call/jump operand, the inline immediate
        // is disassembled as label if possible.
        if (isCallOrJump(move.destination()) &&
            move.isInInstruction() && move.parent().isInProcedure() &&
            move.parent().parent().isInProgram() &&
            labelCount(
                move.parent().parent().parent(),
                move.source().value().uIntWordValue()) > 0) {

            Word immediate = move.source().value().uIntWordValue();
            std::string codelabel = label(
                move.parent().parent().parent(), immediate, 0);
            sourceDis = new DisassemblyLabel(codelabel);
        } else {
            sourceDis =
                createInlineImmediate(
                    move.source(), move.bus().signExtends());
        }
    } else {
        sourceDis = createTerminal(move.source());
    }

    DisassemblyMove* disassembly = NULL;
    if (!move.isUnconditional()) {
        DisassemblyGuard* guardDis = createGuard(move.guard());
        disassembly = new DisassemblyMove(sourceDis, destinationDis, guardDis);
    } else {
        disassembly = new DisassemblyMove(sourceDis, destinationDis);
    }
    return disassembly;
}


/**
 * Creates disassembly of an immediate assignment.
 */
DisassemblyImmediateAssignment*
POMDisassembler::createImmediateAssignment(const Immediate& immediate) {
    DisassemblyElement* destination = createTerminal(immediate.destination());
    SimValue value = immediate.value().value();
    bool signExt = immediate.destination().immediateUnit().signExtends();
    return new DisassemblyImmediateAssignment(value, signExt, destination);
}


/**
 * Creates disassembly of a move terminal.
 *
 * Determines the terminal type, an uses appropriate function to create
 * disassembly of the terminal.
 *
 * @param terminal Move terminal to disassemble.
 * @return Disassembly of the move terminal.
 * @exception WrongSubclass The terminal type was unknown.
 */
DisassemblyElement*
POMDisassembler::createTerminal(const Terminal& terminal) {
    assert (
        !terminal.isImmediate() &&
        "Should handle immediate as a special case due to depending on "
        "extension mode!");

    if (terminal.isFUPort()) {
        return createFUPort(terminal);
    }

    if (terminal.isGPR()) {
        return createRegister(terminal);
    }

    if (terminal.isImmediateRegister()) {
        return createImmediateRegister(terminal);
    }

    // Unkown terminal type.
    throw WrongSubclass(__FILE__, __LINE__, __func__,
                        "Unknown terminal type.");
}

/**
 * Creates disassembly of an immediate value.
 *
 * @param terminal Immediate value to disassemble.
 * @param signExtend If the immediate should be considered signed or unsigned.
 * @return Disassembly of the immediate value.
 */
DisassemblyImmediate*
POMDisassembler::createInlineImmediate(
    const Terminal& terminal, bool signExtend) {
    if (const TTAProgram::TerminalProgramOperation* tpo =
        dynamic_cast<const TTAProgram::TerminalProgramOperation*>(&terminal)) {
        if (!tpo->isAddressKnown())
            return new DisassemblyImmediate(NullSimValue::instance(), false);
    }
    try {
        return new DisassemblyImmediate(
            terminal.value(), signExtend);
    } catch (Exception& e) {
        return new DisassemblyImmediate(
            NullSimValue::instance(), signExtend);
    }
}


/**
 * Creates disassembly of a function unit port.
 *
 * @param terminal Function unit terminal to disassemble.
 * @return Disassembly of the function unit terminal.
 */
DisassemblyElement*
POMDisassembler::createFUPort(const Terminal& terminal) {
    const FunctionUnit* fu = &terminal.functionUnit();

    const TTAProgram::TerminalFUPort& fuTerm =
        dynamic_cast<const TerminalFUPort&>(terminal);

    if (dynamic_cast<const UniversalFunctionUnit*>(fu) != NULL ||
        (fu->machine()->isUniversalMachine() &&
         dynamic_cast<const ControlUnit*>(fu) != NULL)) {

        // The terminal is a UniversalFunctionUnit operation or
        // a universal machine control unit operation.
        if (&fuTerm.hintOperation() != &NullOperation::instance()) {
            string operation = fuTerm.hintOperation().name();
            HWOperation* hwOperation = fu->operation(operation);
            FUPort* port = fu->operationPort(fuTerm.port().name());
            int operand = hwOperation->io(*port);
            return new DisassemblyOperand(
                fuTerm.hintOperation().name(), operand);
        }
        return new DisassemblyControlUnitPort(fuTerm.port().name());
    }

    // if terminal contains any information about operation
    if (fuTerm.isOpcodeSetting()) {
        // check if that osal operation was found
        if (&fuTerm.operation() == &NullOperation::instance()) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Osal operation for operation code name '%s' was not found.") 
             % fuTerm.hwOperation()->name()).str());
        }
        // The terminal is an opcode setting port of an operation or.
        // has additional info about opcode that terminal is bound.
        return new DisassemblyFUOpcodePort(
            fu->name(), fuTerm.port().name(), fuTerm.operation().name());
    }

    // The terminal is a generic function unit port.
    return new DisassemblyFUPort(fu->name(), terminal.port().name());
}

/**
 * Creates disassembly of a register.
 *
 * @param terminal Register to disassemble.
 * @return Disassembly of the register.
 * @exception WrongSubclass The terminal was not register.
 */
DisassemblyElement*
POMDisassembler::createRegister(const Terminal& terminal) {
    const RegisterFile* rf = &terminal.registerFile();

    if (rf == NULL) {
        throw WrongSubclass(__FILE__, __LINE__, __func__,
                            "Not register terminal.");
    }

    const UniversalMachine* machine =
        dynamic_cast<const UniversalMachine*>(rf->machine());

    if (machine != NULL) {

        if (rf == &machine->booleanRegisterFile()) {
            // universal bool register
            return new DisassemblyBoolRegister();
        }


        // The register is a UnboundedRegisterFile register of a universal
        // machine.
        const UnboundedRegisterFile* universalRF =
            dynamic_cast<const UnboundedRegisterFile*>(rf);

        if (universalRF == &machine->integerRegisterFile()) {
            // universal int register
            return new DisassemblyIntRegister(terminal.index());
        }
        
        if (universalRF == &machine->doubleRegisterFile()) {
            // universal floating point register
            return new DisassemblyFPRegister(terminal.index());
        }

        // Unknown universal machine register type.
        throw WrongSubclass(
            __FILE__, __LINE__, __func__,
            "Unknown universal machine register type.");
    }

    return new DisassemblyRegister(rf->name(), terminal.index());
}

/**
 * Creates disassembly of an immediate register.
 *
 * @param terminal Immediate register to disassemble.
 * @return Disassembly of the immediate register.
 */
DisassemblyImmediateRegister*
POMDisassembler::createImmediateRegister(const Terminal& terminal) {
    string immediateUnit = terminal.immediateUnit().name();
    unsigned int index = terminal.index();
    return new DisassemblyImmediateRegister(immediateUnit, index);
}


/**
 * Creates disassembly of a move guard.
 *
 * @param guard Move guard to disassemble.
 * @return Disassembly of the move guard.
 */
DisassemblyGuard*
POMDisassembler::createGuard(const MoveGuard& moveGuard) {

    const Guard* guard = &moveGuard.guard();

    DisassemblyElement* guarded = NULL;

    // Check if the guarded element is a fu port.
    const PortGuard* portGuard = dynamic_cast<const PortGuard*>(guard);
    if (portGuard != NULL) {
        string fuName = portGuard->port()->parentUnit()->name();
        string portName = portGuard->port()->name();
        guarded = new DisassemblyFUPort(fuName, portName);
    }

    // Check if the guarded element is a register.
    const RegisterGuard* registerGuard =
        dynamic_cast<const RegisterGuard*>(guard);
    if (registerGuard != NULL) {

        // Check if the register file is bool rf of a universal machine.
        const RegisterFile* rf = registerGuard->registerFile();
        const UniversalMachine* machine =
            dynamic_cast<const UniversalMachine*>(rf->machine());

        if (machine != NULL && rf == &machine->booleanRegisterFile()) {
            return new DisassemblySequentialGuard(moveGuard.isInverted());
        } else {
            string rfName = registerGuard->registerFile()->name();
            int index = registerGuard->registerIndex();
            guarded = new DisassemblyRegister(rfName, index);
        }
    }

    // Check if the guard is unconditional.
    if (moveGuard.isUnconditional()) {
        // TODO: Disassemble unconditional guards.
        assert(false);
    }


    return new DisassemblyGuard(guarded, moveGuard.isInverted());
}

/**
 * Returns start address of the program.
 *
 * @return Start address of the program.
 */
Word
POMDisassembler::startAddress() const {
    unsigned int address = program_.startAddress().location();
    return address;
}


/**
 * Returns number of instructions in the program.
 *
 * @return Instruction count.
 */
Word
POMDisassembler::instructionCount() const {
    Word count = 0;
    unsigned procedureCount = program_.procedureCount();
    for (unsigned i = 0; i < procedureCount; i++) {
        const Procedure& procedure = program_.procedure(i);
        count = count + procedure.instructionCount();
    }
    return count;
}

/**
 * Returns number of labels at the given address.
 *
 * @param address Instruction address.
 * @return Instruction label count.
 */
int
POMDisassembler::labelCount(Word address) const {
    return labelCount(program_, address);
}

/**
 * Returns number of labels at the given address.
 *
 * @param address Instruction address.
 * @return Instruction label count.
 */
int
POMDisassembler::labelCount(const TTAProgram::Program& program, Word address) {
    
    Address iaddr(address, program.startAddress().space());
    const GlobalScope& scope = program.globalScopeConst();
    unsigned labelCount = scope.globalCodeLabelCount(iaddr);

    if (labelCount > 0) {
        return labelCount;
    }

    for (int i = 0; i < program.procedureCount(); i++) {
        if (program.procedure(i).startAddress().location() == address) {
            return 1;
        }
    }

    return 0;
}

/**
 * Returns instruction label with the given index.
 *
 * @param address Instruction address.
 * @return Instruction label with the given index and address.
 * @exception OutOfRange The address or index is out of range.
 */
std::string
POMDisassembler::label(Word address, int index) const {
    return label(program_, address, index);
}

/**
 * Returns instruction label with the given index.
 *
 * @param address Instruction address.
 * @param program Program to search label for.
 * @return Instruction label with the given index and address.
 * @exception OutOfRange The address or index is out of range.
 */
std::string
POMDisassembler::label(
    const TTAProgram::Program& program, Word address, int index) {
    if (index == 0 && labelCount(program, address) == 1) {
        for (int i = 0; i < program.procedureCount(); i++) {
            if (program.procedure(i).startAddress().location() == address) {
                return program.procedure(i).name();
            }
        }
    }

    if (index > labelCount(program, address)) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__, "No label in requested index: " +
            Conversion::toString(index));
    }

    Address iaddr(address, program.startAddress().space());

    const CodeLabel& label =
        program.globalScopeConst().globalCodeLabel(iaddr, index);

    return label.name();
}

/**
 * Checks if the terminal is a control unit jump or call operand.
 *
 * @param terminal Terminal to check.
 * @return True, if the terminal is a control unit jump or call operand,
 *         false otherwise.
 */
bool
POMDisassembler::isCallOrJump(const Terminal& terminal) {

    if (!terminal.isFUPort()) {
        return false;
    }

    const FunctionUnit* fu = &terminal.functionUnit();

    if (dynamic_cast<const ControlUnit*>(fu) == NULL) {
        return false;
    }

    if (!terminal.isOpcodeSetting()) {
        return false;
    }

    string operationName =
        StringTools::stringToLower(terminal.operation().name());

    if (operationName == "call" || operationName == "jump") {
        return true;
    }

    return false;
}


/**
 * Static helper function to create disassembly string from a move.
 *
 * @param move Move to disassemble.
 * @return Disassembly of the instruction.
 */
std::string
POMDisassembler::disassemble(const TTAProgram::Move& move) {
   
    // special handling for calls: find out the procedure name to make
    // the disassembly a bit more readable
    if (move.isCall()) {
        if (move.source().isInstructionAddress()) {
            
            DisassemblyMove* dMove = createMove(move);
            std::string disasm = "";
            disasm = dMove->toString();
            
            Procedure* proc = NULL;
            try {
                proc = dynamic_cast<TTAProgram::Procedure*>(
                    &move.source().instructionReference().instruction().parent());
            } catch (...) {
                // proc stays NULL if something goes wrong.
            }
            
            std::string procName = 
                proc != NULL ? proc->name() : "unknown_proc";
            return (boost::format("%s -> %s.call.1") 
                    % procName
                    % move.destination().functionUnit().name()).str();
        } 
        
        if (move.source().isCodeSymbolReference()) {
            return (boost::format("%s -> %s.call.1")
                    % move.source().toString()
                    % move.destination().functionUnit().name()).str();
        }
    }

    DisassemblyMove* dMove = createMove(move);
    std::string disasm = "";
    disasm = dMove->toString();
    delete dMove;
    return disasm;
}

/**
 * Static helper function to create disassembly string from an instruction.
 *
 * @param instruction Instruction to disassemble.
 * @param indices Print the instruction's index as comment.
 * @return Disassembly of the instruction.
 */
std::string 
POMDisassembler::disassemble(
    const TTAProgram::Instruction& instruction, bool indices, int addr) {
    
    std::string disasm = "";
    if (!instruction.isInProcedure() ||
        !instruction.parent().isInProgram()) {

        // a "free instruction" that is not connected to any program
        // traverse the moves instead of the buses

        DisassemblyInstruction* dInstruction = new DisassemblyInstruction();
        // The instruction disassembly is created by one move at a time.
        for (int i = 0; i < instruction.moveCount(); i++) {

            const Move& move = instruction.move(i);
            const Bus* bus = &move.bus();
            string busName = move.bus().name();
            bool isUsed = false;

            // Check for long immediate.
            for (int i = 0; i < instruction.immediateCount(); i++) {
                if (&instruction.instructionTemplate() != 
                    &NullInstructionTemplate::instance() &&
                    instruction.instructionTemplate().usesSlot(busName)) {
                    dInstruction->addMove(
                        createImmediateAssignment(instruction.immediate(i)));
                    isUsed = true;
                }
            }

            // Check for move.
            for (int i = 0; i < instruction.moveCount(); i++) {
                if (&instruction.move(i).bus() == bus) {
                    dInstruction->addMove(createMove(instruction.move(i)));
                    isUsed = true;
                }
            }

            if (!isUsed) {
                dInstruction->addMove(new DisassemblyNOP);
            }
        }

        // Add disassembly of long immediates in immediate slots.
        for (int i = 0; i < instruction.immediateCount(); i++) {
            dInstruction->addLongImmediate(
                createImmediateAssignment(instruction.immediate(i)));                
        }
        
        disasm = dInstruction->toString() + " # 'free instruction'";
        delete dInstruction;
        dInstruction = NULL;
    } else {

        const Program& program = instruction.parent().parent();
        DisassemblyInstruction* dInstruction = 
            createInstruction(program, instruction);

        disasm = dInstruction->toString();
        delete dInstruction;
        dInstruction = NULL;

        // check for partially scheduled code, that is, moves that refer to
        // the unversal machine even though the target machine is not 
        // a universal machine
        const TTAMachine::Machine* mach = 
            dynamic_cast<UniversalMachine*>(&program.targetProcessor());

        if (mach == NULL) {
            // we have either mixed code or parallel code, let's check if this
            // instruction has a move scheduled to universal machine's bus, 
            // in which case it's mixed code and the sequential move did not
            // get printed with createInstruction()
            for (int i = 0; i < instruction.moveCount(); ++i) {
                const TTAProgram::Move& m = instruction.move(i);
                if (dynamic_cast<UniversalMachine*>(m.bus().machine()) != NULL) {
                    // found unscheduled move, add it only as a comment to the
                    // disassembly to make the output compilable with TCEAssembler
                    disasm += "\t# unscheduled: ";
                    disasm += POMDisassembler::disassemble(m);
                }
            }
        } else {
            // a sequential program
        }

        if (indices) {
            disasm += 
                "\t# @" + 
                Conversion::toString(
                    addr != -1 ? addr : instruction.address().location());
        }

//to display source filename and line number
//check for source code filename, we just use the file filename, 
//assume the all the move belong the same file
        TCEString fileNameStr = "";
        for (int i = 0; i < instruction.moveCount(); ++i) {
            const TTAProgram::Move& m = instruction.move(i);
            if (m.hasSourceFileName()) {
                fileNameStr += m.sourceFileName();
                break;
            }
        }	

	if (fileNameStr != "")
            disasm += "\t# file: " + fileNameStr;        
	
        // check for soure code line number info
        TCEString lineNumberStr = "";
        for (int i = 0; i < instruction.moveCount(); ++i) {
            const TTAProgram::Move& m = instruction.move(i);
            if (m.hasSourceLineNumber()) {
                if (lineNumberStr != "") {
                    lineNumberStr += ", ";
                }
                lineNumberStr += 
                    Conversion::toString(m.sourceLineNumber());
            }
        }

        if (lineNumberStr != "")
            disasm += "\t# slines: " + lineNumberStr;        
    }
    return disasm;
}

/**
 * Static helper function to create a disassembly string from a program.
 *
 * @param program Program to disassemble.
 * @param indices Print the instruction indices as comment.
 * @return Disassembly of the program in a string.
 * @exception Exception Can leak exceptions if the traversed program is
 * malformed, etc.
 */
std::string
POMDisassembler::disassemble(const TTAProgram::Program& program, bool indices) {
    std::stringstream stringStream;

    for (int procIndex = 0; procIndex < program.procedureCount(); 
         ++procIndex) {
        const Procedure& proc = program.procedureAtIndex(procIndex);
        stringStream << POMDisassembler::disassemble(proc, indices);
    }
    return stringStream.str();
}

/**
 * Static helper function to create a disassembly string from a procedure.
 *
 * @param proc Procedure to disassemble.
 * @param indices Print the instruction indices as comment.
 * @return Disassembly of the procedure in a string.
 * @exception Exception Can leak exceptions if the traversed program is
 * malformed, etc.
 */
std::string
POMDisassembler::disassemble(const TTAProgram::Procedure& proc, bool indices) {
    std::stringstream stringStream;

    const TTAProgram::Instruction* currentInstruction = NULL;

    // proc.instructionCount() is O(n) operation so do it only once.
    
    stringStream << ":procedure " << proc.name() << ";" << std::endl;

    for (int instrIndex = 0, iCount = proc.instructionCount(); 
         instrIndex < iCount; ++instrIndex) {
        currentInstruction = 
            &proc.instructionAtIndex(instrIndex);
        
        InstructionAddress addr = proc.startAddress().location() + instrIndex;
//currentInstruction->address().location();

        const int lc = POMDisassembler::labelCount(proc.parent(), addr); 
        for (int labelIndex = 0; labelIndex < lc; ++labelIndex) {

            stringStream << POMDisassembler::label(
                proc.parent(), addr, labelIndex) << ":" << std::endl;
        }
        stringStream << "\t" << POMDisassembler::disassemble(
            *currentInstruction, indices, addr) << std::endl;                
    }
    return stringStream.str();
}
