/**
 * @file CompiledSimCodeGenerator.hh
 *
 * Declaration of CompiledSimCodeGenerator class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_CODE_GENERATOR_HH
#define COMPILED_SIM_CODE_GENERATOR_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <map>

#include "OperationPool.hh"
#include "SimulatorConstants.hh"
#include "ConflictDetectionCodeGenerator.hh"

//#define DEBUG_SIMULATION 1

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
class SimulationController;



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
    /// A type for storing basic block entries and exits
    typedef std::map<InstructionAddress, InstructionAddress> BasicBlocks;

    CompiledSimCodeGenerator(
        const TTAMachine::Machine& machine,
        const TTAProgram::Program& program,
        const SimulationController& controller,
        bool sequentialSimulation,
        bool fuResourceConflictDetection);

    virtual ~CompiledSimCodeGenerator();
    
    virtual void generateToDirectory(const std::string& dirName);
    virtual StringSet createdFiles() const;
    virtual BasicBlocks basicBlocks() const;

private:
    /// Copying not allowed.
    CompiledSimCodeGenerator(const CompiledSimCodeGenerator&);
    /// Assignment not allowed.
    CompiledSimCodeGenerator& operator=(const CompiledSimCodeGenerator&);
    
    /// A type for operation symbol declarations: 1=op.name 2=op.symbol
    typedef std::multimap<std::string, std::string> OperationSymbolDeclarations;

    void generateConstructorParameters();
    void generateHeaderAndMainCode();
    void generateConstructorCode();
    void generateSimulationCode();
    void findBasicBlocks() const;
    void generateProcedureCode(const TTAProgram::Procedure& proc);
    void generateShutdownCode(InstructionAddress address);
    void generateSimulationGetter();
    void generateHaltCode(const std::string& message="");
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
    std::string handleOperationOld(const TTAMachine::HWOperation& op);
    std::string detectConflicts(const TTAMachine::HWOperation& op);
    std::string handleGuard(const TTAMachine::Guard& guard, bool isJumpGuard);
    void generateInstruction(const TTAProgram::Instruction& instruction);
    std::string generateTriggerCode(const TTAMachine::HWOperation& op);
    std::string generateStoreTrigger(const TTAMachine::HWOperation& op);
    std::string generateLoadTrigger(const TTAMachine::HWOperation& op);
    
    int maxLatency() const;
    std::vector<TTAMachine::Port*> fuOutputPorts(
        const TTAMachine::FunctionUnit& fu) const;
                
    /// The machine used for simulation
    const TTAMachine::Machine& machine_;
    /// The simulated program
    const TTAProgram::Program& program_;
    /// The simulator frontend
    const SimulationController& simController_;    
    /// GCU
    const TTAMachine::ControlUnit& gcu_;
    
    /// Is the simulation sequential code or not
    bool isSequentialSimulation_;    
    /// Name of the class to be created
    std::string className_;

    /// Directory where to write the source files of the engine.
    std::string targetDirectory_;    
    
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
    
    /// number of cycles after jump-code is to be generated
    int pendingJumpDelay_;
    /// last instruction of the current basic block
    InstructionAddress lastInstructionOfBB_;
    /// name of the bool variable used for guard check - needed for guarded jumps with latency
    std::string lastJumpGuardBool_;
    /// name of the last used guard variable
    std::string lastGuardBool_;
    /// Temporary list of the used guard bool symbols per instruction
    std::map<std::string, std::string> usedGuardSymbols_;

    /// The basic block map referred by start of the block as a key
    mutable BasicBlocks bbStarts_;
    /// The basic block map referred by end of the block as a key
    mutable BasicBlocks bbEnds_;

    /// The operation pool
    OperationPool operationPool_;
    /// File to generate the code
    std::string fileName_;   
    /// Header filename
    std::string headerFile_;
    /// Main source filename. This includes the constructor and the simulateCycle().
    std::string mainFile_;
    /// Current file being processed
    std::fstream currentFile_;
    /// Current output stream (usually the above file)
    std::ostream* os_;

    /// Conflict detection code generator
    ConflictDetectionCodeGenerator conflictDetectionGenerator_;
    
    // Compiled Simulator Code Generator constants:
    /// Maximum amount of instructions per code file
    static const int MAX_INSTRUCTIONS_PER_FILE = 500;
};


#endif // include once

