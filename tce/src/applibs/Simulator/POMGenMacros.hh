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
 * @file POMGenMacros.hh
 * 
 * A macro set and helper functions for implementing the macros
 * to make creating of Program Object Models for unit tests manually 
 * easy.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */
#ifndef TTA_POMGEN_MACROS
#define TTA_POMGEN_MACROS


#include "UniversalMachine.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"

#include "SimValue.hh"
#include "OperationPool.hh"
#include "Application.hh"
#include "UniversalFunctionUnit.hh"
#include "UnboundedRegisterFile.hh"
#include "SmartHWOperation.hh"
#include "FUPort.hh"
#include "TerminalFUPort.hh"
#include "BaseFUPort.hh"
#include "Port.hh"
#include "Guard.hh"
#include "SpecialRegisterPort.hh"
#include "RegisterFile.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "AssocTools.hh"
#include "StringTools.hh"
#include "Conversion.hh"


/// define this to get the programs printed to stdout while they are
/// contructed
#ifdef PRINT_PROGRAMS
#   define PRINTOUT(STUFF) Application::logStream() << STUFF
#else
#   define PRINTOUT(STUFF)
#endif

SimValue* temporary = NULL;

/**
 * A procedure that does nothing.
 */
void null__() {
}

/**
 * Helper function which finds a register file in the machine.
 *
 * Aborts with error if the register file is not found.
 *
 * @param machine The machine to search in.
 * @param name The name of the register file.
 * @return The found register file.
 */
TTAMachine::RegisterFile& 
findRegisterFile(const TTAMachine::Machine& machine, std::string name) {

    if (dynamic_cast<const UniversalMachine*>(&machine) != NULL) {
	if (StringTools::ciEqual(name, "r")) {
	    return dynamic_cast<const UniversalMachine&>(
		machine).integerRegisterFile();
	} else 
	    if (StringTools::ciEqual(name, "f")) {
		return dynamic_cast<const UniversalMachine&>(
		    machine).doubleRegisterFile();
	    }
	abortWithError(std::string("No register file ") + name + " found.");
    } else {
        TTAMachine::Machine::RegisterFileNavigator nav = 
            machine.registerFileNavigator();
	return *nav.item(name);
    }

    abortWithError("Can't find register file " + name);
    // just to avoid compiler warnings    
    throw 1;
}

/**
 * Creates a TerminalRegister using the given register file and register.
 *
 * @param RFNAME The register file name (either 'r' or 'f' in sequential).
 * @param REGNUM The index of the register.
 */
#define REG(RFNAME, REGNUM) \
    new TerminalRegister(\
            *findRegisterFile(machine__, #RFNAME).port(0), \
            REGNUM);\
    PRINTOUT(#RFNAME "" #REGNUM)
/**
 * Shortcut to an integer register in sequential code.
 *
 * @param REGNUM The integer register index.
 */
#define IREG(REGNUM) \
    REG(r, REGNUM)

/**
 * Helper function which finds a function unit in the machine.
 *
 * Aborts with error if the function unit is not found.
 *
 * @param machine The machine to search in.
 * @param name The name of the function unit. Can be empty in sequential.
 * @return The found register file.
 */
TTAMachine::FunctionUnit& 
findFunctionUnit(const TTAMachine::Machine& machine, std::string name) {
    if (dynamic_cast<const UniversalMachine*>(&machine) != NULL) {
        return dynamic_cast<const UniversalMachine&>(
            machine).universalFunctionUnit();
    } else {
        TTAMachine::Machine::FunctionUnitNavigator nav = 
            machine.functionUnitNavigator();
        return *nav.item(name);
    }
}

/**
 * Creates terminal of given parameters.
 */
TTAProgram::TerminalFUPort*
createTerminalFUPort(
    const TTAMachine::Machine& machine, 
    std::string fuName, std::string pName, std::string opName) {
    
    TTAMachine::FunctionUnit& fu = findFunctionUnit(machine, fuName);        
    TTAMachine::HWOperation *op = NULL;
    
    if (fu.hasOperation(opName)) {
        op = fu.operation(opName);
    } else {
        op = new TTAMachine::HWOperation(opName, fu);
    }
    
    int index = 0;
     
    try {
        index = Conversion::toInt(pName);        
    } catch ( ... ) {
        index = op->io(*fu.operationPort(pName));
    }

    return new TTAProgram::TerminalFUPort(*op, index);
}

/**
 * Helper function which finds a port in a function unit.
 *
 * Aborts with error if the port is not found.
 *
 * @param machine The machine to search in.
 * @param fuName The name of the function unit. The operation, if sequential.
 * @param portName The name of the port. The operand, if sequential code.
 * @return The found port.
 */
TTAMachine::Port& 
findFUPort(
    const TTAMachine::Machine& machine, 
    std::string fuName, 
    std::string portName) {
    
    if (dynamic_cast<const UniversalMachine*>(&machine) != NULL) {
        return *dynamic_cast<const UniversalMachine&>(machine).
            universalFunctionUnit().operation(fuName)->port(
                Conversion::toInt(portName));
    } else {
        return *findFunctionUnit(machine, fuName).port(portName);
    }
}

/**
 * Creates a TerminalFUPort using the given name strings.
 *
 * @param FUNAME The function unit or operation name (in sequential).
 * @param PORTNAME The port name or operand index (in sequential).
 */
#define PORT(FUNAME, PORTNAME) \
    new TerminalFUPort( \
          dynamic_cast<BaseFUPort&>( \
              findFUPort(machine__, #FUNAME, #PORTNAME)));\
    PRINTOUT(#FUNAME "." #PORTNAME)

/**
 * Creates a TerminalFUPort using the given name strings.
 *
 * @param FUNAME The function unit or operation name (in sequential).
 * @param PORTNAME The port name or operand index (in sequential).
 * @param OPERATION The name of the operation.
 */
#define OPORT(FUNAME, PORTNAME, OPERATION) \
    createTerminalFUPort( \
        machine__, #FUNAME, #PORTNAME, #OPERATION);\
    PRINTOUT(#FUNAME "." #OPERATION "." #PORTNAME)


/**
 * A shortcut to an operand in sequential code.
 *
 * @param OPERATION The name of the operation.
 * @param OPERAND_INDEX Index of the operand.
 */
#define OPERAND(OPERATION, OPERAND_INDEX) \
    OPORT(OPERATION, OPERAND_INDEX, OPERATION)
    
/**
 * Creates a temporary SimValue to be used in TerminalImmediate construction.
 *
 * @param bitWidth The bit width of the SimValue.
 * @param intValue The integer value of the SimValue.
 * @return A SimValue reference (owned by the function).
 */
SimValue&
tempSimValue(unsigned int bitWidth, SIntWord intValue) {
    delete temporary;
    temporary = new SimValue(bitWidth);
    *temporary = intValue;
    return *temporary;
}

/**
 * Helper function which finds a bus in a machine.
 *
 * Aborts with error if the bus is not found.
 *
 * @param machine The machine to search in.
 * @param busName The name of the bus. Unused in case of UniversalMachine.
 * @return The found bus.
 */
TTAMachine::Bus&
findBus(const TTAMachine::Machine& machine, std::string busName) {
    if (dynamic_cast<const UniversalMachine*>(&machine) != NULL) {
        return dynamic_cast<const UniversalMachine&>(machine).universalBus();
    } else {
        TTAMachine::Machine::BusNavigator nav = machine.busNavigator();
        return *nav.item(busName);
    }
}
 
/**
 * Creates a TerminalImmediate by width and value.
 *
 * Expects the creator to manage deallocation. 
 *
 * @param WIDTH The bit width of the value.
 * @param VALUE The value of the immediate.
 */
#define IMM(WIDTH, VALUE) \
     new TerminalImmediate(tempSimValue(WIDTH, VALUE)); \
     PRINTOUT("#" #VALUE)

/**
 * Creates a TerminalFUPort that writes to the GCU operand port, thus 
 * triggering a control flow operation.
 *
 * @param OPERATION The name of the operation to trigger.
 * @param OPNUM The operand number.
 */
#define CONTROL_OPERAND(OPERATION, OPNUM) \
    new TerminalFUPort( \
        *machine__.controlUnit()->operation(#OPERATION), \
        OPNUM);\
    PRINTOUT(#OPERATION "." #OPNUM)

/**
 * Helper function which finds the boolean register file in the machine.
 *
 * Aborts with error if the boolean register file is not found.
 *
 * @param machine The machine to search in.
 * @return The found register file.
 */
TTAMachine::RegisterFile& 
findBooleanRegisterFile(const TTAMachine::Machine& machine) {
    
    TTAMachine::Machine::RegisterFileNavigator navigator = 
        machine.registerFileNavigator();
    
    // go through all the register files in the machine
    for (int i = 0; i < navigator.count(); ++i) {
        TTAMachine::RegisterFile& rf = *navigator.item(i);
        
        // expect the boolean register to be the first register file with
        // only one register of width 1
        if (rf.width() == 1 && rf.numberOfRegisters() == 1) {
            return rf;
        }
    }
    // just to avoid compiler warnings
    abortWithError("Can't find boolean register file from machine!.");
    throw 1;
}

/**
 * Helper function which finds either normal or inverted guard pointing 
 * to boolean register 
 *
 * Aborts with error if the boolean register file is not found.
 *
 * @param machine The machine to search in.
 * @param isInverted Do we look for inverted guard.
 * @return The found Guard
 */
TTAMachine::Guard& 
findBooleanGuard(const TTAMachine::Machine& machine, bool isInverted) {

    TTAMachine::Bus *bus = machine.busNavigator().item(0);
    
    // go through all the register files in the machine
    for (int i = 0; i < bus->guardCount(); ++i) {
	TTAMachine::RegisterGuard *regGuard = 
	    dynamic_cast<TTAMachine::RegisterGuard*>(bus->guard(i));
	
	if (regGuard == NULL) {
	    continue;
	}
    
	if (regGuard->isInverted() == isInverted &&       
	    regGuard->registerFile()->width() == 1 && 
	    regGuard->registerFile()->numberOfRegisters() == 1) {
	    return *regGuard;
	}
    }

    // just to avoid compiler warnings
    abortWithError("Can't find BooleanGuard!");
    throw 1;
}

/**
 * Creates a terminal pointing to the boolean register.
 */
#define BOOL() \
    new TerminalRegister(\
           *findBooleanRegisterFile(machine__).port(0), 0);\
    PRINTOUT("bool")

/**
 * Returns RegisterGuard pointing to boolean register
 */
#define BOOLGUARD() \
    findBooleanGuard(machine__, false);\
    PRINTOUT("boolguard")

/**
 * Returns Inverted RegisterGuard pointing to boolean register
 */
#define INVBOOLGUARD() \
    findBooleanGuard(machine__, true);\
    PRINTOUT("invboolguard")

/**
 * Creates a terminal pointing to the return address port of the GCU.
 */
#define RA() \
   new TerminalFUPort(\
        dynamic_cast<BaseFUPort&>( \
            *machine__.controlUnit()->returnAddressPort()));\
    PRINTOUT("ra")


/**
 * A shortcut which creates a 32 bit TerminalImmediate.
 *
 * @param VALUE The value of the immediate.
 */
#define IMM32(VALUE) \
     IMM(32, VALUE)

/**
 * Creates an instruction and adds it to a procedure.
 *
 * @param ADD_MOVES A (list of) move creation statement(s).
 */
#define INSTRUCTION(ADD_MOVES) \
{\
    Instruction* tempInstruction__ = new Instruction(); \
    int moveCounter__ = 1; \
    PRINTOUT("\t" << currentIP__ << ": ");\
    ADD_MOVES; \
    PRINTOUT("\n");\
    proc->add(tempInstruction__); \
} currentIP__ += incrementOfIP__

/**
 * A helper macro to create different types of moves.
 *
 * @param SRC The source terminal.
 * @param DST The destination terminal.
 * @param MOVE_CONSTRUCTOR A constructor of the move to create.
 */
#define MOVE_TEMPLATE(SRC, DST, CONSTRUCT_MOVE) \
{\
    Terminal* src__ = SRC;\
    PRINTOUT(" -> "); \
    Terminal* dst__ = DST;\
    PRINTOUT(" "); \
    Bus* bus__ = &findBus(\
        machine__, std::string("bus") + Conversion::toString(moveCounter__));\
    std::shared_ptr<Move> tempMove = NULL;                                              \
    CONSTRUCT_MOVE;\
    tempInstruction__->addMove(tempMove);\
} moveCounter__++;

/**
 * Creates a basic move without any guards.
 */
#define BASIC_MOVE_CONSTRUCTION tempMove = std::make_shared<Move>(src__, dst__, *bus__)

/**
 * Creates a basic move and adds it to the instruction this macro is 
 * invoked in.
 *
 * Expects that the bus names of the machine are named by appending the
 * index of the bus to the word 'bus', i.e., 'bus1', 'bus2', etc.
 *
 * @param SRC The source terminal.
 * @param DST The destination terminal.
 */
#define MOVE(SRC, DST) MOVE_TEMPLATE(SRC, DST, BASIC_MOVE_CONSTRUCTION);

/**
 * A no-operation move.
 *
 * Just skips the move slot by incrementing the move counter.
 */
#define NOP PRINTOUT("nop "); moveCounter__++;

/**
 * Gets the variable connected to a label.
 *
 * Using LABEL and SET_LABEL macros one can make implementing backward
 * jumps easier.
 * 
 * @param LABEL_NAME The name of the label to refer to.
 */
#define LABEL(LABEL_NAME) iaddress_##LABEL_NAME

/**
 * Sets a label at the current instruction pointer position.
 *
 * Using LABEL and SET_LABEL macros one can make implementing backward
 * jumps easier.
 * 
 * @param LABEL_NAME The name of the label to refer to.
 */
#define SET_LABEL(LABEL_NAME) \
    PRINTOUT(#LABEL_NAME ":\n");\
    unsigned int LABEL(LABEL_NAME) = currentIP__; \
    LABEL(LABEL_NAME) = currentIP__;

/**
 * Forces a new value to the instruction pointer.
 *
 * @param VALUE The new value to assign to the instruction pointer counter.
 */
#define SET_IP(VALUE) currentIP__ = VALUE

/**
 * Sets the amount how much instruction pointer should be incremented after
 * adding an instruction.
 *
 * @param COUNT The increment.
 */
#define SET_IP_INCREMENT(COUNT) incrementOfIP__ = COUNT

/**
 * Creation of guarded move.
 *
 * @param TERMINAL The terminal the guard should watch.
 * @param INVERT Is the guard inverted? true if it is.

#define GUARDED_MOVE_CONSTRUCTION(TERMINAL, INVERT) \
    PRINTOUT(" || only if ");\
    Terminal* term__ = TERMINAL;\
    PRINTOUT(" != " #INVERT); \
    PRINTOUT(" ||"); \
    MoveGuard* guard__ = new MoveGuard(term__, INVERT); \
    tempMove = new Move(src__, dst__, *bus__, guard__); 
 */

#define GUARDED_MOVE_CONSTRUCTION(ADFGUARD) \
    TTAMachine::Guard &adfGuard__ = ADFGUARD; \
    MoveGuard* guard__ = new MoveGuard(adfGuard__); \
    tempMove = std::make_shared<Move>(src__, dst__, *bus__, guard__);

  
/**
 * Creates a guarded move and adds it to the instruction this macro is 
 * invoked in.
 *
 * Non-inverted version. 
 *
 * @param SRC The source terminal.
 * @param DST The destination terminal.
 * @param GTERM The termimal that is watched by this guard.
 #define GMOVE(SRC, DST, GTERM) \
 MOVE_TEMPLATE(SRC, DST, GUARDED_MOVE_CONSTRUCTION(GTERM, false));
 */

#define GMOVE(SRC, DST, GUARD) \
    MOVE_TEMPLATE(SRC, DST, GUARDED_MOVE_CONSTRUCTION(GUARD));

/**
 * Creates a guarded move and adds it to the instruction this macro is 
 * invoked in.
 *
 * Inverted version.s
 *
 * @param SRC The source terminal.
 * @param DST The destination terminal.
 * @param GTERM The termimal that is watched by this guard.
 #define IGMOVE(SRC, DST, GTERM) \
 MOVE_TEMPLATE(SRC, DST, GUARDED_MOVE_CONSTRUCTION(GTERM, true));
*/

/**
 * Initializes the POM generator macros. 
 *
 * This should be stated before using the above macros.
 *
 * @PROGRAM The name of the program.
 * @MACHINE The target processor of the program.
 */
#define CREATE_PROGRAM(PROGRAM, MACHINE) \
\
    Application::initialize(); \
    OperationPool operationPool; \
    unsigned int currentIP__ = 0; \
    currentIP__ = 0; \
    unsigned int incrementOfIP__ = 1; \
    incrementOfIP__ = 1; \
    Procedure* proc = NULL;\
    const AddressSpace& programSpace__ = \
        *MACHINE.controlUnit()->addressSpace();\
    Program PROGRAM(programSpace__);\
    Program& program__ = PROGRAM; \
    const Machine& machine__ = MACHINE;\
    PRINTOUT("\n# program '" #PROGRAM "':\n")

/**
 * Creates a new procedure to add instructions to.
 *
 * Sets a label to the creation position.
 */
#define CREATE_PROCEDURE(PROCNAME) \
    SET_LABEL(PROCNAME); \
    Procedure* PROCNAME = \
        new Procedure("proc_"#PROCNAME, programSpace__, LABEL(PROCNAME));\
    program__.addProcedure(PROCNAME);\
    proc = PROCNAME

/**
 * Frees the resources allocated by the POM generator macros.
 *
 * Currently does basically nothing. Code is there just to avoid compiler
 * warnings/errors.
 */
#define END_PROGRAM \
    int foooooO__ = 1; foooooO__++;\
    PRINTOUT("\n# program ends\n")

#endif
