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
 * @file CompiledSimCodeGenerator.hh
 *
 * Declaration of CompiledSimCodeGenerator class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_CODE_GENERATOR_HH
#define COMPILED_SIM_CODE_GENERATOR_HH

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>

#include "OperationPool.hh"
#include "SimulatorConstants.hh"
#include "ConflictDetectionCodeGenerator.hh"
#include "CompiledSimSymbolGenerator.hh"

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class FUPort;
    class Port;
    class Unit;
    class HWOperation;
    class Guard;
    class RegisterFile;
    class ControlUnit;
    class Bus;
}

namespace TTAProgram {
    class Program;
    class Instruction;
    class Procedure;
    class Terminal;
    class CodeLabel;
    class GlobalScope;
    class Move;
}

class SimulatorFrontend;
class TTASimulationController;


/// A struct for tracking basic blocks and their relation to their procedures
struct ProcedureBBRelations {
    /// Procedure start per basic block starts
    std::map<InstructionAddress, InstructionAddress> procedureStart;
    
    typedef std::multimap<InstructionAddress, InstructionAddress> 
        BasicBlockStarts;
        
    /// All basic block start addresses per procedure start
    BasicBlockStarts basicBlockStarts;
    
    /// Basic block starts and their corresponding .cpp files
    std::map<InstructionAddress, std::string> basicBlockFiles;
};


/**
 * A class that generates C/C++ code from the given POM and MOM
 * 
 * Used for the compiled simulation
 * 
 */
class CompiledSimCodeGenerator {
public:
    /// A type for std::string sets
    typedef std::set<std::string> StringSet;
    /// A type for storing address-to-address combinations
    typedef std::map<InstructionAddress, InstructionAddress> AddressMap;

    CompiledSimCodeGenerator(
        const TTAMachine::Machine& machine,
        const TTAProgram::Program& program,
        const TTASimulationController& controller,
        bool sequentialSimulation,
        bool fuResourceConflictDetection,
        bool handleCycleEnd,
        bool dynamicCompilation,
        bool basicBlockPerFile = false,
        bool functionPerFile = true);

    virtual ~CompiledSimCodeGenerator();
    
    virtual void generateToDirectory(const std::string& dirName);
    virtual StringSet createdFiles() const;
    virtual AddressMap basicBlocks() const;
    virtual ProcedureBBRelations procedureBBRelations() const;

private:
    /// Copying not allowed.
    CompiledSimCodeGenerator(const CompiledSimCodeGenerator&);
    /// Assignment not allowed.
    CompiledSimCodeGenerator& operator=(const CompiledSimCodeGenerator&);
    
    /**
    * A struct for handling delayed assignments for the FU results
    */
    struct DelayedAssignment {
        /// The source symbol
        std::string sourceSymbol;
        /// The target symbol
        std::string targetSymbol;
        /// The FU result symbol
        std::string fuResultSymbol;
    };
    
    /// A type for operation symbol declarations: 1=op.name 2=op.symbol
    typedef std::multimap<std::string, std::string> OperationSymbolDeclarations;
    
    /// FU Result writes
    typedef std::multimap<int, DelayedAssignment> DelayedAssignments;
    typedef std::map<std::string, int> FUResultWrites;

    void generateConstructorParameters();
    void generateHeaderAndMainCode();
    void generateConstructorCode();
    void generateSimulationCode();
    void findBasicBlocks() const;
    void generateProcedureCode(const TTAProgram::Procedure& procedure);
    void generateShutdownCode(InstructionAddress address);
    void generateFUOutputUpdater();
    void generateSimulationGetter();
    std::string generateHaltCode(const std::string& message="");
    void generateAdvanceClockCode();
    void updateDeclaredSymbolsList();
    void updateSymbolsMap();
    void generateSymbolDeclarations();
    void generateJumpTableCode();
    void generateMakefile();
    
    void addDeclaredSymbol(const std::string& name, int width);
    void addUsedRFSymbols();
        
    std::string handleJump(const TTAMachine::HWOperation& op);
    std::string handleOperation(const TTAMachine::HWOperation& op);
    std::string handleOperationWithoutDag(const TTAMachine::HWOperation& op);
    std::string detectConflicts(const TTAMachine::HWOperation& op);
    std::string generateGuardRead(const TTAProgram::Move& move);
    std::string generateGuardCondition(const TTAProgram::Move& move);
    void generateInstruction(const TTAProgram::Instruction& instruction);
    std::string generateTriggerCode(const TTAMachine::HWOperation& op);
    std::string generateStoreTrigger(const TTAMachine::HWOperation& op);
    std::string generateLoadTrigger(const TTAMachine::HWOperation& op);
    
    std::string generateAddFUResult(
        const TTAMachine::FUPort& resultPort, 
        const std::string& value, 
        int latency);
    
    std::string generateFUResultRead(
        const std::string& destination, 
        const std::string& resultSymbol);

    int maxLatency() const;
    
    std::vector<TTAMachine::Port*> fuOutputPorts(
        const TTAMachine::FunctionUnit& fu) const;
                
    /// The machine used for simulation
    const TTAMachine::Machine& machine_;
    /// The simulated program
    const TTAProgram::Program& program_;
    /// The simulator frontend
    const TTASimulationController& simController_;
    /// GCU
    const TTAMachine::ControlUnit& gcu_;
    
    /// Is the simulation sequential code or not
    bool isSequentialSimulation_;
    /// Should we let frontend handle each cycle end
    bool handleCycleEnd_;
    /// Is this a dynamic compiled simulation?
    bool dynamicCompilation_;
    /// Should the generator generate only one basic block per code file
    bool basicBlockPerFile_;
    /// Should the generator start with a new file after function end
    bool functionPerFile_;
    
    /// Type for SimValue symbol declarations: string=symbolname, int=width
    typedef std::map<std::string, int> SimValueSymbolDeclarations;
    /// A list of all symbols that are declared after the program code is ready
    SimValueSymbolDeclarations declaredSymbols_;
    
    /// A set of all the declared functions
    StringSet declaredFunctions_;
    /// A list of the code files created during the process
    StringSet createdFiles_;
    /// A list of used operations
    OperationSymbolDeclarations usedOperations_;
       
    /// Absolute instruction # being processed
    int instructionNumber_;
    /// Istruction counter for checking how many instructions to put per file
    int instructionCounter_;
    /// How many moves have we been through with?
    int moveCounter_;
    /// Are we at the beginning of a new procedure?
    bool isProcedureBegin_;
    /// Pointer to the current Procedure being processed
    const TTAProgram::Procedure* currentProcedure_;
    
    /// last instruction of the current basic block
    InstructionAddress lastInstructionOfBB_;
    /// name of the last used guard variable
    std::string lastGuardBool_;
    /// Temporary list of the used guard bool symbols per instruction
    std::map<std::string, std::string> usedGuardSymbols_;
    /// Program exit point addresses
    std::set<InstructionAddress> exitPoints_;

    /// The basic block map referred by start of the block as a key
    mutable AddressMap bbStarts_;
    /// The basic block map referred by end of the block as a key
    mutable AddressMap bbEnds_;
    /// Basic blocks relations to procedures and vice versa.
    ProcedureBBRelations procedureBBRelations_;
    /// Delayed FU Result assignments
    DelayedAssignments delayedFUResultWrites_;
    /// Last known FU result writes
    FUResultWrites lastFUWrites_;

    /// The operation pool
    OperationPool operationPool_;
    
    /// Directory where to write the source files of the engine.
    std::string targetDirectory_;    
    /// Name of the class to be created
    std::string className_;
    /// Header filename
    std::string headerFile_;
    /// Main source filename. This includes the constructor and the simulateCycle().
    std::string mainFile_;
    /// Current file being processed
    std::fstream currentFile_;
    /// Name of the current file being processed
    std::string currentFileName_;
    /// Current output stream i.e. the above file
    std::ostream* os_;
    
    /// The symbol generator
    CompiledSimSymbolGenerator symbolGen_;

    /// Conflict detection code generator
    ConflictDetectionCodeGenerator conflictDetectionGenerator_;

    /// Maximum number of instructions per engine source code file,
    /// computed from the instruction width (bus count) to control
    /// simulation engine code size explosion with wider simulated
    /// machines.
    unsigned maxInstructionsPerFile_;
    /// Max for each simulation function.
    unsigned maxInstructionsPerSimulationFunction_;
    
};

#endif // include once

