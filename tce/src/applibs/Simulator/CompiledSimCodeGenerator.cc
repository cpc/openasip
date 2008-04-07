/**
 * @file CompiledSimCodeGenerator.cc
 *
 * Definition of CompiledSimCodeGenerator class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "SimulatorFrontend.hh"
#include "TTASimulationController.hh"
#include "CompiledSimCodeGenerator.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "PipelineElement.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "ExecutableInstruction.hh"
#include "NullInstruction.hh"
#include "NullRegisterFile.hh"
#include "NullImmediateUnit.hh"
#include "Move.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "TerminalFUPort.hh"
#include "Immediate.hh"
#include "Operation.hh"
#include "OperationContext.hh"
#include "OperationDAGConverter.hh"
#include "OperationPool.hh"
#include "HWOperation.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "FileSystem.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "ControlFlowGraph.hh"
#include "config.h"
#include "BaseType.hh"
#include "SymbolGenerator.hh"
#include "CompiledSimMove.hh"

using namespace TTAMachine;
using namespace TTAProgram;
using std::string;
using std::fstream;
using std::endl;
using std::vector;

/**
 * The constructor
 * 
 * @param machine The machine to run the simulation on
 * @param program The simulated program
 * @param frontend The simulator frontend
 */
CompiledSimCodeGenerator::CompiledSimCodeGenerator(
    const TTAMachine::Machine& machine,
    const TTAProgram::Program& program,
    const TTASimulationController& controller,
    bool sequentialSimulation,
    bool fuResourceConflictDetection) :
    machine_(machine), program_(program), simController_(controller),
    gcu_(*machine.controlUnit()),
    isSequentialSimulation_(sequentialSimulation),
    className_("CompiledSimulationEngine"),
    instructionNumber_(0), instructionCounter_(0),
    pendingJumpDelay_(0), lastInstructionOfBB_(0), 
    conflictDetectionGenerator_(machine_, 
    fuResourceConflictDetection) {
}

/**
 * The destructor
 */
CompiledSimCodeGenerator::~CompiledSimCodeGenerator() {    
}

/**
 * Generates compiled simulation code to the given directory.
 * 
 * @param directory Directory where all the code is to be generated at
 * @exception IOException if the directory could not be opened for writing
 */
void
CompiledSimCodeGenerator::generateToDirectory(const string& directory) {
    targetDirectory_ = directory;
    fileName_ = className_;
    headerFile_ = className_ + ".hh";
    mainFile_ = className_ + ".cc";

    CATCH_ANY(generateMakefile());
    
    generateSimulationCode();
    generateHeaderAndMainCode();
}

/**
 * Generates a Makefile for compiling the simulation engine.
 */
void
CompiledSimCodeGenerator::generateMakefile() {

    const string fileName =
        targetDirectory_ + FileSystem::DIRECTORY_SEPARATOR + "Makefile";

    std::ofstream makefile(fileName.c_str());
    std::vector<std::string> includePaths = Environment::includeDirPaths();
    std::string includes;
    for (std::vector<std::string>::iterator it = includePaths.begin(); 
        it != includePaths.end(); ++it) {
        includes += "-I" + *it + " ";
    }

    makefile 
        << "sources = $(wildcard *.cpp)" << endl
        << "objects = $(patsubst %.cpp,%.o,$(sources))" << endl
        << "dobjects = $(patsubst %.cpp,%.so,$(sources))" << endl
        << "includes = " << includes << endl
        << "soflags = -shared -fpic" << endl 
            
        // use because ccache doesn't like changing directory paths
        // (in a preprocessor comment)
        << "cppflags = -fno-working-directory -fno-enforce-eh-specs -fno-rtti "
        << "-fno-threadsafe-statics -fno-access-control" << endl << endl
        
        << "all: engine.so" << endl << endl

        << "engine.so: CompiledSimulationEngine.hh.gch $(dobjects) CompiledSimulationEngine.cc" << endl
        << "\t#@echo Compiling engine.so" << endl
        << "\t$(CC) $(soflags) $(cppflags) -O0 $(includes) CompiledSimulationEngine.cc "
        << "-c -o engine.o" << endl 
        << "\t$(CC) $(soflags) engine.o -o engine.so" << endl << endl
            
        << "$(dobjects): %.so: %.cpp CompiledSimulationEngine.hh.gch" << endl

        // compile and link phases separately to allow distributed compilation
        // thru distcc
        << "\t$(CC) -c $(soflags) $(cppflags) $(opt_flags) $(includes) $< -o $@.o" << endl
        << "\t$(CC) $(soflags) $(opt_flags) -lgcc $@.o -o $@" << endl
        << "\t@rm -f $@.so.o" << endl
        << endl
            
        // use precompiled headers for more speed
        << "CompiledSimulationEngine.hh.gch:" << endl
        << "\t$(CC) $(soflags) $(cppflags) $(opt_flags) $(includes) "
        << "-xc++-header CompiledSimulationEngine.hh" << endl
        << endl
            
        << "clean:" << endl
        << "\t@rm -f $(dobjects) engine.so CompiledSimulationEngine.hh.gch" << endl;
    
    makefile.close();
}

/**
 * Returns a list of the created .cpp filenames
 * 
 * @return a list of the created .cpp filenames
 */
CompiledSimCodeGenerator::StringSet 
CompiledSimCodeGenerator::createdFiles() const {
    return createdFiles_;
}

/**
 * Returns a list of basic blocks of the program
 * 
 * @return a list of basic blocks of the program
 */
CompiledSimCodeGenerator::BasicBlocks 
CompiledSimCodeGenerator::basicBlocks() const {
    if (bbStarts_.empty()) {
        findBasicBlocks();
    }
    return bbStarts_;
}

/**
 * Creates a header file to be used for all the .cpp files and the main
 * source file which includes the constructor and the main simulation loop.
 * 
 * Initializes all variables required by the simulation
 * 
 * Sets up new SimValue variables for all the FUs, registers, 
 * FU pipeline stages and so on. Variables are created as member variables.
 * 
 */
void 
CompiledSimCodeGenerator::generateHeaderAndMainCode() {

    // Open a new file for the header
    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    currentFile_.open((targetDirectory_ + DS + headerFile_).c_str(), fstream::out);
    os_ = &currentFile_;
    
    // Generate includes
    *os_ << "// " << fileName_ << " Generated automatically by ttasim" << endl
         << "#ifndef _AUTO_GENERATED_COMPILED_SIMULATION_H_" << endl
         << "#define _AUTO_GENERATED_COMPILED_SIMULATION_H_" << endl
         << "#include \"SimValue.hh\"" << endl
         << "#include \"Program.hh\"" << endl
         << "#include \"MemorySystem.hh\"" << endl
         << "#include \"DirectAccessMemory.hh\"" << endl
         << "#include \"SimulationEventHandler.hh\"" << endl
         << "#include \"SimulatorFrontend.hh\"" << endl
         << "#include \"OSAL.hh\"" << endl
         << "#include \"Operation.hh\"" << endl
         << "#include \"OperationPool.hh\"" << endl
         << "#include \"OperationContext.hh\"" << endl
         << "#include \"Conversion.hh\"" << endl
         << "#include \"Exception.hh\"" << endl
         << "#include \"CompiledSimulation.hh\"" << endl
         << "#include \"Instruction.hh\"" << endl 
         << conflictDetectionGenerator_.includes() << endl
         << "#include <map>" << endl
         << endl;
    
    // Open up class declaration and define some extra member variables
    *os_ << "class " << className_ << ";" << endl
         << "typedef void (" << className_ << "::*SimulateFunction)();" 
         << endl << endl;
    
    *os_ << "class " << className_ << " : public CompiledSimulation {" << endl
         << "public:" << endl
         << "/// Type for the Jump map. 1: given target address  2: target function" << endl
         << "typedef std::map<InstructionAddress, SimulateFunction> JumpMap;" << endl
         << "/// Type for the faster jump table:: index is the target address, returns target func" 
         << endl
         << "typedef std::vector<SimulateFunction> JumpTable;" << endl
         << "/// The jump table" << endl
         << "JumpTable jumpTable_;" << endl
         << "/// Next jump target as a function pointer" << endl
         << "SimulateFunction jumpTargetFunc_;" << endl;

    // Declare all FUs
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();
    for (int i = 0; i < fus.count(); ++i) {
        const FunctionUnit& fu = *fus.item(i);
        
        // FU Ports
        for (int j = 0; j < fu.operationPortCount(); ++j) {
            const FUPort& port = *fu.operationPort(j);
            addDeclaredSymbol(SymbolGenerator::portSymbol(port), port.width());    
        }                
    
        if (conflictDetectionGenerator_.conflictDetectionEnabled()) {
            *os_ << conflictDetectionGenerator_.symbolDeclaration(fu);
        }

        *os_ << "\t" << "OperationContext " 
             << SymbolGenerator::operationContextSymbol(fu) << ";" << endl;
        
        // Declare address spaces
        if (fu.addressSpace() != NULL) {
             *os_ << "\t" << "DirectAccessMemory& " 
                 << SymbolGenerator::DAMemorySymbol(*fus.item(i)) << ";" << endl;
        }
        
        // Declare all operations of the FU
        for (int j = 0; j < fu.operationCount(); ++j) {
            string opName = fu.operation(j)->name();
            *os_ << "\t" << "Operation& " 
                 << SymbolGenerator::operationSymbol(opName, fu) << ";" << endl;
            
            usedOperations_.insert(
                std::make_pair(opName, SymbolGenerator::operationSymbol(opName, fu)));
        }
        
        // FU output results
        std::vector<Port*> outPorts = fuOutputPorts(fu);
        for (size_t j = 0; j < outPorts.size(); ++j) {
            *os_ << "\t" << "FUResultType " 
                 << SymbolGenerator::FUResultSymbol(*outPorts.at(j)) 
                 << ";" << endl;
        }
    }
    *os_ << endl;
    
    // GCU
    ControlUnit* gcu = machine_.controlUnit();
    if (gcu) {
        for (int i = 0; i < gcu->specialRegisterPortCount(); ++i) {
            SpecialRegisterPort& port = *gcu->specialRegisterPort(i);
            addDeclaredSymbol(SymbolGenerator::portSymbol(port), port.width());
        }
        for (int i = 0; i < gcu->operationPortCount(); ++i) {
            FUPort& port = *gcu->operationPort(i);
            addDeclaredSymbol(SymbolGenerator::portSymbol(port), port.width());
        }
    }
    
    // Declare all IUs    
    const Machine::ImmediateUnitNavigator& ius = machine_.immediateUnitNavigator();
    for (int i = 0; i < ius.count(); ++i) {
        const ImmediateUnit& iu = *ius.item(i);
        for (int j = 0; j < iu.numberOfRegisters(); ++j) {
            addDeclaredSymbol(SymbolGenerator::immediateRegisterSymbol(iu, j), iu.width());
        }
    }
    
    // Register files
    if (!isSequentialSimulation_) {
        const Machine::RegisterFileNavigator& rfs = machine_.registerFileNavigator();
        for (int i = 0; i < rfs.count(); ++i) {
            const RegisterFile& rf = *rfs.item(i);
            for (int j = 0; j < rf.numberOfRegisters(); ++j) {
                addDeclaredSymbol(SymbolGenerator::registerSymbol(rf, j), rf.width());
            }
        }
    } else {
        addUsedRFSymbols();
    }
    
    // Buses
    const Machine::BusNavigator& buses = machine_.busNavigator();
    for (int i = 0; i < buses.count(); ++i) {
        const Bus& bus = *buses.item(i);
        addDeclaredSymbol(SymbolGenerator::busSymbol(bus), bus.width());
    }
    
    generateSymbolDeclarations();
    generateConstructorParameters();
    *os_ << ";" << endl;

    generateAdvanceClockCode();

    *os_ << "void simulateCycle();" << endl << endl << "}; // end class" 
         << endl << endl << "#endif // include once" << endl << endl;

    // header written
    currentFile_.close();
    
    // write implementations to the main functions
    generateConstructorCode();
}

/**
 * Generates the parameter list for the constructor.
 */
void
CompiledSimCodeGenerator::generateConstructorParameters() {
    *os_ << className_ 
         << "(const TTAMachine::Machine& machine," << endl
         << "const TTAProgram::Program& program," << endl
         << "SimulatorFrontend& frontend," << endl
         << "MemorySystem& memorySystem)";
}

/**
 * Generates code for the class constructor in the main .cpp file.
 */
void 
CompiledSimCodeGenerator::generateConstructorCode() {

    const string DS = FileSystem::DIRECTORY_SEPARATOR;    
    currentFile_.open((targetDirectory_ + DS + mainFile_).c_str(), fstream::out);
    createdFiles_.insert(mainFile_);
    os_ = &currentFile_;

    *os_ << "#include \"" << headerFile_ << "\"" << endl << endl;
    *os_ << className_ << "::";
    
    generateConstructorParameters();
        
    *os_ << " : " << endl         
         << "CompiledSimulation(machine, program, frontend, memorySystem), "
         << endl;
    
    updateDeclaredSymbolsList();
    *os_ << "\t" << "jumpTargetFunc_(0)" << endl;
    
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();
    for (int i = 0; i < fus.count(); i++) {
        const FunctionUnit & fu = *fus.item(i);
        if (fu.addressSpace() != NULL) {
            *os_ << "," << endl;
            *os_ << "\t" << SymbolGenerator::DAMemorySymbol(fu) 
                 << "(FUMemory(\"" << fu.name() << "\"))";
        }
    }
    
    *os_ << " {" << endl;
    updateSymbolsMap();
    
    for (int i = 0; i < fus.count(); i++) {
        const FunctionUnit& fu = *fus.item(i);
        std::string context = SymbolGenerator::operationContextSymbol(fu);
        // Create a state for each operation
        for (int j = 0; j < fu.operationCount(); ++j) {
            std::string operation = SymbolGenerator::operationSymbol(
                fu.operation(j)->name(), fu);
            
            *os_ << "\t" << operation
                << ".createState(" << context << ");"
                << endl;
        }

        // Set a Memory for the context
        if (fu.addressSpace() != NULL) {
                 *os_ << "\t" << context << ".setMemory(&" 
                      << SymbolGenerator::DAMemorySymbol(fu) << ");" << endl;
            }
    }
    
    generateJumpTableCode();
    
    *os_ << conflictDetectionGenerator_.extraInitialization()
         << "}" << endl << endl;
}

/**
 * Generates the simulateCycle() code that is called outside the .so file
 */
void
CompiledSimCodeGenerator::generateSimulationCode() {
    
    findBasicBlocks();
    
    exitPoints_ = simController_.findProgramExitPoints(program_, machine_);

    // generate code for all procedures
    for (int i = 0; i < program_.procedureCount(); ++i) {
        generateProcedureCode(program_.procedure(i));
    }
    
    // Create the simulateCycle() function 
    *os_ << "// Simulation code:" << endl
         << "void " << className_  << "::simulateCycle() {" << endl << endl;

    // Create a jump dispatcher for accessing each basic block start
    *os_ << "// jump dispatcher" << endl
         << "\tjumpTargetFunc_ = jumpTable_[jumpTarget_];" << endl
         << "\t(this->*jumpTargetFunc_)();" << endl << endl;
    
    *os_ << conflictDetectionGenerator_.notifyOfConflicts()
         << "}" << endl << endl;

    generateSimulationGetter(); // generate getter in the last file
    
    // Close the last file
    currentFile_.close();
    os_ = NULL;
}

/**
 * Finds all basic blocks of the program and stores them in two std::maps
 */
void
CompiledSimCodeGenerator::findBasicBlocks() const {
    for (int i = 0; i < program_.procedureCount(); ++i) {
        ControlFlowGraph cfg(program_.procedure(i));
        for (int i = 0; i < cfg.nodeCount(); ++i) {
            BasicBlockNode& node = cfg.node(i);
            const InstructionAddress start = node.originalStartAddress();
            const InstructionAddress end = node.originalEndAddress();
            if (end != 0) {
                bbEnds_[end] = start;
                bbStarts_[start] = end;
            }
        }
    }
}

/** 
 * Generates code for each instruction in a procedure
 * 
 * @param proc the procedure to generate code from
 * @exception InstanceNotFound if the first instruction wasn't found
 */
void
CompiledSimCodeGenerator::generateProcedureCode(const Procedure& proc) {
    const Instruction* instruction = &proc.firstInstruction();
    while (instruction != &NullInstruction::instance()) {
        generateInstruction(*instruction);
        instruction = &(proc.nextInstruction(*instruction));
        instructionNumber_++;
        instructionCounter_--;
    }
}

/**
 * Generates shutdown code for the given instruction address
 * 
 * @param address Address location of the instruction. Will be saved in the PC
 * 
 */
void
CompiledSimCodeGenerator::generateShutdownCode(InstructionAddress address) {
    *os_ << "/* PROGRAM EXIT */" << endl
         << "programCounter_ = " << address << ";" << endl
         << "isFinished_ = true; return;" << endl;
}

/**
 * Generates code for a C function that returns an instance of the compiled sim
 */
void 
CompiledSimCodeGenerator::generateSimulationGetter() {
    // use C-style functions only! (C++ name mangling complicates stuff)
    *os_ << "/* Class getter function */" << endl
         << "extern \"C\" CompiledSimulation* getSimulation("
         << "const TTAMachine::Machine& machine," << endl
         << "const TTAProgram::Program& program," << endl
         << "SimulatorFrontend& frontend," << endl
         << "MemorySystem& memorySystem) {" << endl
         << "return new " << className_
         << "(machine, program, frontend, memorySystem);" 
         << endl << "}" << endl << endl; // 2x end-of-line in the end of file
}

/**
 * Generates code for halting the simulation
 * 
 * @param message a message describing the reason for the halt
 */
void
CompiledSimCodeGenerator::generateHaltCode(const string& message) {
    *os_ << "\t"
       << "throw SimulationExecutionError(__FILE__, __LINE__, __func__, \""
       << message << "\");" << endl;
}

/**
 * Generates code for advancing clocks of various items per cycle
 */
void CompiledSimCodeGenerator::generateAdvanceClockCode() {
    *os_ << "void inline advanceClocks() {" << endl
         << conflictDetectionGenerator_.advanceClockCode()
         << endl << "}" << endl;
}

/**
 * Updates the declared symbols list after the program code is generated.
 * 
 * At this point, we know what registers are used and what are not. We only
 * declare the used registers.
 */
void 
CompiledSimCodeGenerator::updateDeclaredSymbolsList() {

    for (SimValueSymbolDeclarations::iterator it = declaredSymbols_.begin();
         it != declaredSymbols_.end(); ++it) {
        *os_ << "\t" << it->first
             << "(" << Conversion::toString(it->second) << ")," << endl;
    }
    *os_ << endl;
        
    for (OperationSymbolDeclarations::iterator it = usedOperations_.begin();
         it != usedOperations_.end(); ++it) {
        *os_ << "\t" << it->second
             << "(operationPool_.operation(\"" << it->first << "\"))," 
             << endl;
    }
    *os_ << endl;
    
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();    
    for (int i = 0; i < fus.count(); ++i) {
        const FunctionUnit& fu = *fus.item(i);
        std::vector<Port*> outPorts = fuOutputPorts(fu);
        for (size_t j = 0; j < outPorts.size(); ++j) {
            *os_ << "\t" << SymbolGenerator::FUResultSymbol(*outPorts.at(j)) 
                << "(" << Conversion::toString(fu.maxLatency()) << ")," << endl;
        }
    }
    *os_ << endl
         << conflictDetectionGenerator_.updateSymbolDeclarations() << endl;
}

/**
 * Updates the Symbols map of the CompiledSimulation class
 */
void 
CompiledSimCodeGenerator::updateSymbolsMap() {
    for (SimValueSymbolDeclarations::const_iterator it =
        declaredSymbols_.begin(); it != declaredSymbols_.end(); ++it) {
        string symbolName = it->first;
        *os_ << "\t" << "symbols_[\"" << symbolName << "\"] = &" 
             << symbolName << ";" << endl;
    }
}

/**
 * Generates declarations for all the symbols in the declared symbols -list 
 * 
 * (SimValues, bool guards...)
 */
void 
CompiledSimCodeGenerator::generateSymbolDeclarations() {
    for (SimValueSymbolDeclarations::const_iterator it = declaredSymbols_.begin();
         it != declaredSymbols_.end(); ++it) {
             *os_ << "\t" << "SimValue " << it->first << ";" << endl;
    }
    
    *os_ << endl;
    
    for (StringSet::const_iterator it = declaredFunctions_.begin();
         it != declaredFunctions_.end(); ++it) {
             *os_ << "\t" << "void " << *it << "();" << endl;
    }
}

/**
 * Generates code that updates the jump table and finds out all the basic blocks
 */
void
CompiledSimCodeGenerator::generateJumpTableCode() {    

    *os_ << "\t" << "jumpTable_.resize(program_.lastInstruction().address()." 
         << "location() + 1, NULL);" << endl;

    for (BasicBlocks::iterator it = bbStarts_.begin(); it != bbStarts_.end();
        ++it) {
        *os_ << "\t" << "jumpTable_[" << it->first << "] = &" << className_ 
             << "::" << SymbolGenerator::basicBlockSymbol(it->first) 
             << ";" << endl;
    }
}

/**
 * Updates the declared symbols list if needed
 * 
 * @param name name of the symbol
 * @param width SimValue width
 */
void 
CompiledSimCodeGenerator::addDeclaredSymbol(
    const string& name, int width) {
    declaredSymbols_[name] = width;
}

/**
 * Finds the used RF symbols from the sequential simulation
 */
void 
CompiledSimCodeGenerator::addUsedRFSymbols() {
    if (!isSequentialSimulation_) {
        return;
    }
    
    // Loop all moves of the program and find the used RFs
    const Instruction* instruction = &program_.firstInstruction();
    while (instruction != &NullInstruction::instance()) {
        for (int i = 0; i < instruction->moveCount(); ++i) {
            const Move& move = instruction->move(i);
            if (move.source().isGPR()) {
                addDeclaredSymbol(SymbolGenerator::registerSymbol(
                move.source()), move.source().port().width());
            }
            if (move.destination().isGPR()) {
                addDeclaredSymbol(SymbolGenerator::registerSymbol(
                move.destination()), move.destination().port().width());
            }
        }
        instruction = &(program_.nextInstruction(*instruction));
    }
}

/**
 * Generates code for a jump operation
 * 
 * @param op A jump or call operation
 * @return A std::string containing generated code for the jump call
 */
string
CompiledSimCodeGenerator::handleJump(const TTAMachine::HWOperation& op) {
    assert(op.name() == "call" || op.name() == "jump");
    
    std::stringstream ss;
    pendingJumpDelay_ = gcu_.delaySlots() + 1;
    ss << "jumpTarget_ = " 
       << SymbolGenerator::portSymbol(*op.port(1)) << ".value_.sIntWord;";
    if (op.name() == "call") {
        ss << SymbolGenerator::returnAddressSymbol(gcu_) << ".value_.uIntWord = " 
           << instructionNumber_ + gcu_.delaySlots() + 1 << "u;" << endl;
    }
    return ss.str();
}

/**
 * Generates code for a triggered operation.
 * 
 * @param op the triggered operation
 * @return A std::string containing generated code for the operation call
 */
string
CompiledSimCodeGenerator::handleOperation(const TTAMachine::HWOperation& op) {
    std::stringstream ss;
                     
    if (op.name() != "jump" && op.name() != "call") {
        string simCode = generateTriggerCode(op);
        
        ss << endl << "{ " << endl;
        ss << "#define context "                
           << SymbolGenerator::operationContextSymbol(*op.parentUnit()) << endl;
        ss << "#define opPool_ operationPool_" << endl;
        ss << simCode << endl;
        ss << "#undef context" << endl;
        ss << "#undef opPool_" << endl;
        ss << "}" << endl;
    } else { // simulate a jump
        ss << handleJump(op);
    }
    
    ss << "/* trigger " << op.name() << " : " << op.latency() << " */" << endl;

    return ss.str();
}

/// @TODO GET RID OF THIS!
string
CompiledSimCodeGenerator::handleOperationOld(
    const TTAMachine::HWOperation& op) {
    std::stringstream ss;        
    
    // grab all operands and initialize them to the operand table
    for (int i = 1; op.port(i) != NULL; ++i) {
        if (op.port(i)->isInput()) {
            ss << "operandTable_[" << i - 1 << "] = " << "&"
               << SymbolGenerator::portSymbol(*op.port(i)) << "; ";
        } else {
            // output operands' values are stored temporarily in here
            ss << "operandTable_[" << i - 1 << "] = " << "&"
               << "outOperands_[" << i - 1 << "]; ";
        }
    }
    
    // call simulateTrigger
    /// @todo maybe use IsCall & IsControlFlowOperation.
    if (op.name() != "jump" && op.name() != "call") {
        ss << SymbolGenerator::operationSymbol(op.name(), *op.parentUnit())
           << ".simulateTrigger(operandTable_, "
           << SymbolGenerator::operationContextSymbol(*op.parentUnit())
           << "); ";
               
        // add output values as delayed assignments
        for (int i = 1; op.port(i) != NULL; ++i) {
            if (op.port(i)->isOutput()) {
                ss << "addFUResult("
                   << SymbolGenerator::FUResultSymbol(*op.port(i)) 
                   << ", cycleCount_, outOperands_[" << i - 1 << "], "
                   << op.latency() << ");";
            }
        }
                
    } else { // simulate a jump
        ss << handleJump(op);
    }
    ss << "/* trigger " << op.name() << " : " << op.latency() << " */" << endl;

    return ss.str();
}

/**
 * Generates code for a guard
 * 
 * @param guard the guard
 * @param isJumpGuard Is this a guard with jump or call instruction in it?
 * @return a std::string containing generated code for the guard check
 */
string 
CompiledSimCodeGenerator::handleGuard(
    const TTAMachine::Guard& guard,
    bool isJumpGuard) {
    std::stringstream ss;
    string guardSymbolName;
    
    if (dynamic_cast<const RegisterGuard*>(&guard) != NULL) {
        const RegisterGuard& rg = dynamic_cast<const RegisterGuard&>(guard);
        RegisterFile& rf = *rg.registerFile();
        guardSymbolName = SymbolGenerator::registerSymbol(rf, rg.registerIndex());        
    } else if (dynamic_cast<const PortGuard*>(&guard) != NULL) {
        const PortGuard& pg = dynamic_cast<const PortGuard&>(guard);
        guardSymbolName = SymbolGenerator::portSymbol(*pg.port());
    } else {
        ss << endl << "#error unknown guard type!" << endl;
    }
    
    lastGuardBool_ = "";
        
    if (usedGuardSymbols_.find(guardSymbolName) == usedGuardSymbols_.end()) {
        lastGuardBool_ = SymbolGenerator::guardBoolSymbol();        
        usedGuardSymbols_[guardSymbolName] = lastGuardBool_;
        
        ss << "bool " << lastGuardBool_ << " = !(" 
           << guardSymbolName << ".value_.uIntWord == 0u);";
        if (isJumpGuard) {
            lastJumpGuardBool_ = "";
            if (guard.isInverted()) { 
                lastJumpGuardBool_ = "!";
            }
            lastJumpGuardBool_ += lastGuardBool_;
        }
    } else {
        lastGuardBool_ = usedGuardSymbols_[guardSymbolName];
    }
            
    ss << endl << "if (";
    if (guard.isInverted()) {
        lastGuardBool_ = "!" + lastGuardBool_;
    }
    ss << lastGuardBool_ << ") { ";   
    
    return ss.str();
}

/**
 * Generates code from the given instruction
 * 
 * @param instruction instruction to generate the code from
 * @return a std::string containing the generated code
 */
void
CompiledSimCodeGenerator::generateInstruction(const Instruction& instruction) {

    InstructionAddress address = instruction.address().location();
    usedGuardSymbols_.clear();
    
    // Are we at the start of a new basic block?
    if (bbStarts_.find(address) != bbStarts_.end()) {
        if (instructionCounter_ <= 0) { // Should we start with a new file?
            instructionCounter_ = MAX_INSTRUCTIONS_PER_FILE;
            currentFile_.close();
            const string DS = FileSystem::DIRECTORY_SEPARATOR;
            string fileName = targetDirectory_ + DS 
                + SymbolGenerator::basicBlockSymbol(address) + ".cpp";
            currentFile_.open(fileName.c_str(), fstream::out);
            createdFiles_.insert(fileName);
            os_ = &currentFile_;
            *os_ << "// " << fileName_ 
                 << " Generated automatically by ttasim" << endl
                 << "#include \"" << headerFile_ << "\"" << endl << endl;
        }
        
        lastInstructionOfBB_ = bbStarts_.find(address)->second;
        
        // Start a new C++ function for the basic block
        declaredFunctions_.insert(SymbolGenerator::basicBlockSymbol(address));
        *os_ << endl << "void " << className_ 
             << "::" << SymbolGenerator::basicBlockSymbol(address) << "() {"
             << endl;
    }
    
    *os_ << endl << "/* Instruction " << instructionNumber_ << " */" << endl;
    
    // Advance clocks of the conflict detectors
    if (conflictDetectionGenerator_.conflictDetectionEnabled()) {
        *os_ << "advanceClocks();" << endl;  
    }
    
    // Do immediate assignments per instruction for FU ports
    for (int i = 0; i < instruction.immediateCount(); ++i) {
            const Immediate& immediate = instruction.immediate(i);
            
            if (!immediate.destination().isFUPort()) {
                continue;
            }
            
            *os_ << "\t"
                 << SymbolGenerator::immediateRegisterSymbol(immediate.destination());
            int value = immediate.value().value().unsignedValue();
            *os_  << ".value_.uIntWord = " << value << "u;";
    }
    
    bool endGuardBracket = false;
    
    // Do moves
    std::set<std::string> gotResults; // used to get FU results only once/instr.
    std::vector<CompiledSimMove> lateMoves; // moves with buses
    for (int i = 0; i < instruction.moveCount(); ++i) {
        const Move& move = instruction.move(i);
        string moveSource = SymbolGenerator::moveOperandSymbol(move.source(), move);
        string moveDestination = SymbolGenerator::moveOperandSymbol(move.destination(), move);
        
        lastGuardBool_ = "";
        if (!move.isUnconditional()) { // has a guard?
            *os_ << handleGuard(move.guard().guard(), move.isControlFlowMove());
            endGuardBracket = true;
        }
        if (move.isControlFlowMove()) {
            *os_ << "\t" << "/*control flow move*/";
        }
        
        if (move.source().isFUPort() && gotResults.find(moveSource) == gotResults.end() &&  
            dynamic_cast<const ControlUnit*>(&move.source().functionUnit()) == NULL) {
            *os_ <<
            "FUResult(" << moveSource << ", " 
             << SymbolGenerator::FUResultSymbol(move.source().port())
             << ", cycleCount_);" << endl;
            gotResults.insert(moveSource);
        }
        
        // Find all moves that depend on others
        bool dependingMove = false;
        for (int j = instruction.moveCount() - 1; j > 0 && i != j; --j) {
            if (moveDestination == SymbolGenerator::moveOperandSymbol(
                instruction.move(j).source(), move)) {
                dependingMove = true;
                CompiledSimMove lateMove(move, lastGuardBool_);
                lateMoves.push_back(lateMove);
                *os_ << lateMove.copyToBusCode();
            }
        }
        
        // Assign the values directly instead of through 
        // the SimValue assignment in case:
        if ((move.source().isGPR() || move.source().isFUPort() 
             || move.source().isImmediateRegister()) &&
            (move.source().port().width() <= static_cast<int>(sizeof(UIntWord)*8)) &&
            move.source().port().width() == move.destination().port().width()) {
                moveDestination += ".value_.uIntWord";
                moveSource += ".value_.uIntWord";
            } else if (move.source().isImmediate()) {
                moveDestination += ".value_.uIntWord";
            }        

        if (!dependingMove) {
            *os_ << "\t" << moveDestination << " = " << moveSource << "; ";
        }
        
        if (endGuardBracket) {
            *os_ << "}";
            endGuardBracket = false;
        }
    }
    
    endGuardBracket = false;
    // Do moves with triggers
    for (int i = 0; i < instruction.moveCount(); ++i) {
        const Move& move = instruction.move(i);
        if (!move.isTriggering()) {
            continue;
        }
        if (!move.isUnconditional()) { // has a guard?
            *os_ << handleGuard(move.guard().guard(), move.isControlFlowMove());
            endGuardBracket = true;
        }
        
        if (move.source().isFUPort() && gotResults.find(SymbolGenerator::moveOperandSymbol(move.source(), move)) == gotResults.end() &&
            dynamic_cast<const ControlUnit*>(&move.source().functionUnit()) == NULL) {
            *os_ <<
            "FUResult(" << SymbolGenerator::moveOperandSymbol(move.destination(), move) << ", " 
             << SymbolGenerator::FUResultSymbol(move.source().port()) << ", "
             << "cycleCount_);" << endl;
            gotResults.insert(SymbolGenerator::moveOperandSymbol(move.source(), move));
        }
        
        const TerminalFUPort& tfup = 
            dynamic_cast<const TerminalFUPort&>(move.destination());
            
        /// @NOTE broken operations are commented out
        /// disabled until the additional overhead of this code has been
        /// fixed, now the simulation slowed down dramatically, probably
        /// due to the additional copies (which probably cause dcache misses)
        if ((tfup.hwOperation()->name() == "add" || tfup.hwOperation()->name() == "sub" || tfup.hwOperation()->name() == "shl"
            || tfup.hwOperation()->name() == "shr" || tfup.hwOperation()->name() == "stdout" || tfup.hwOperation()->name() == "gt"
            || tfup.hwOperation()->name() == "gtu" || tfup.hwOperation()->name() == "std" || tfup.hwOperation()->name() == "stq" 
            || tfup.hwOperation()->name() == "div" || tfup.hwOperation()->name() == "divu" || tfup.hwOperation()->name() == "addf" 
            || tfup.hwOperation()->name() == "subf" || tfup.hwOperation()->name() == "mod" || tfup.hwOperation()->name() == "modu"
            || tfup.hwOperation()->name() == "neg" || tfup.hwOperation()->name() == "mul" || tfup.hwOperation()->name() == "ldw"
            || tfup.hwOperation()->name() == "ldq" || tfup.hwOperation()->name() == "sth" || tfup.hwOperation()->name() == "ior"
            || tfup.hwOperation()->name() == "min" || tfup.hwOperation()->name() == "max" || tfup.hwOperation()->name() == "call"
            || tfup.hwOperation()->name() == "jump" || tfup.hwOperation()->name() == "cfi" || tfup.hwOperation()->name() == "cif"
            || tfup.hwOperation()->name() == "cfd" || tfup.hwOperation()->name() == "cdf" || tfup.hwOperation()->name() == "eqf"
            || tfup.hwOperation()->name() == "xor" || tfup.hwOperation()->name() == "eq" || tfup.hwOperation()->name() == "stw"
            || tfup.hwOperation()->name() == "ldh" || tfup.hwOperation()->name() == "ldqu" || tfup.hwOperation()->name() == "ldhu" 
            || tfup.hwOperation()->name() == "and" || tfup.hwOperation()->name() == "or") 
            && operationPool_.operation(tfup.hwOperation()->name()).dagCount() > 0) {
                *os_ << handleOperation(*tfup.hwOperation());
            } else {
                *os_ << handleOperationOld(*tfup.hwOperation());
            }
                *os_ << conflictDetectionGenerator_.detectConflicts(
                        *tfup.hwOperation());
        if (endGuardBracket) {
            *os_ << "}";
            endGuardBracket = false;
        }
    }
    
    // Do immediate assignments for everything else
    for (int i = 0; i < instruction.immediateCount(); ++i) {
            const Immediate& immediate = instruction.immediate(i);
            
            if (immediate.destination().isFUPort()) {
                continue;
            }
            
            *os_ << "\t"
                << SymbolGenerator::immediateRegisterSymbol(immediate.destination());
                int value = immediate.value().value().unsignedValue();
            *os_  << ".value_.uIntWord = " << value << "u;";
    }
    
    for (std::vector<CompiledSimMove>::iterator it = lateMoves.begin();
        it != lateMoves.end(); ++it) {
            *os_ << it->copyFromBusCode();        
    }

    // No operation?
    //if (instruction.moveCount() == 0 && instruction.immediateCount() == 0) {
        //ss << "cout << \" NOP \" << endl;" << endl;
    //}

#ifdef DEBUG_SIMULATION
    *os_ << "lastExecutedInstruction_ = programCounter_;" << endl;
    *os_ << "programCounter_++;" << endl;
    *os_ << "frontend().eventHandler().handleEvent(SimulationEventHandler::SE_CYCLE_END);" << endl;    
#endif
    *os_ << "cycleCount_++;" << endl;
    
    BasicBlocks::iterator bbEnd = bbEnds_.find(address);
    
    // generate exit code if this is a return instruction
    if (exitPoints_.find(address) != exitPoints_.end()) {
        generateShutdownCode(address);
    }

    // Create code for a possible exit after the basic block
    if (bbEnd != bbEnds_.end()) {
        *os_ << "if (cycleCount_ >= cyclesToSimulate_) {" << endl
           << "\t" << "programCounter_ = " << bbEnd->first + 1 << "; "
           << "stopRequested_ = true;" << endl << "}" << endl;
    }
    
    // Is there a jump waiting for execution?
    if (pendingJumpDelay_ > 0) {
        pendingJumpDelay_--;
        if (pendingJumpDelay_ == 0) {
            if (lastJumpGuardBool_ != "") { // is it a conditional jump?
                *os_ << "if (" << lastJumpGuardBool_ << ") ";
            }
            *os_ << "{ programCounter_ = jumpTarget_; "
                 << "lastExecutedInstruction_ = " << address << "; return; }" 
                 << endl;
            lastJumpGuardBool_ = "";
        }
    }
   
    if (bbEnd != bbEnds_.end()) {
        *os_ << "jumpTarget_ = " << address + 1 << ";" << endl
             << "lastExecutedInstruction_ = " << address << ";" << endl;
        
        // Generate some additional code after the last instruction
        if (address == program_.lastInstruction().address().location()) {
            generateShutdownCode(address);
        }
        
         *os_ << endl << "} /* end function */" << endl << endl;
    }
}

/**
 * Generates code for calling triggered operations
 * @param op The operation to be triggered
 * @return A string containing the generated C++ code
 */
string
CompiledSimCodeGenerator::generateTriggerCode(
    const TTAMachine::HWOperation& op) {
    
    vector<string> operands;
    string source = "EXEC_OPERATION(";
       
    // grab all operands and initialize them to the operand table
    for (int i = 1; op.port(i) != NULL; ++i) {
        if (op.port(i)->isInput()) {
            operands.push_back(SymbolGenerator::portSymbol(*op.port(i)));
        } else {
            operands.push_back("outputvalue");
        }
    }
    
    if (op.name() == "stw" || op.name() == "sth" || op.name() == "stq") {
        return generateStoreTrigger(op);
    } else if (op.name() == "ldw" || op.name() == "ldh" || op.name() == "ldq"
        || op.name() == "ldhu" || op.name() == "ldqu") {
        return generateLoadTrigger(op);
    }
    OperationDAG* dag = &operationPool_.operation(op.name()).dag(0);
    string simCode = OperationDAGConverter::createSimulationCode(*dag, &operands);
        
    // add output values as delayed assignments
    std::vector<Port*> outPorts = fuOutputPorts(*op.parentUnit());
    int portIndex = 0;
    while (simCode.find("outputvalue = ") != string::npos) {
        string ioSymbol = "outputvalue = ";
        string adderCode = "addFUResult(" 
            + SymbolGenerator::FUResultSymbol(*outPorts.at(portIndex))
            + ", cycleCount_, ";
            
        string::iterator it = simCode.begin() + simCode.find(ioSymbol);
            simCode.replace(it, it + ioSymbol.length(), adderCode);
        portIndex++;
    }
    
    for (int i = 1, tmp=1; op.port(i) != NULL; ++i) {
        if (op.port(i)->isOutput()) {
            string ioSymbol = ", tmp" + Conversion::toString(tmp) + ";";
            string newSymbol = ", tmp" + Conversion::toString(tmp) + ", " 
                + Conversion::toString(op.latency()) + ");";
                
            if (simCode.find(ioSymbol) == string::npos) {
                break;
            }
                
            string::iterator it = simCode.begin() + simCode.find(ioSymbol);
            simCode.replace(it, it + ioSymbol.length(), newSymbol);
            tmp++;
        } // end isOutput
    } // end for
    
    return simCode;
}

/**
 * Generates a faster version of specific store triggers
 * @param o The store operation (either stw, sth or stq)
 */
string 
CompiledSimCodeGenerator::generateStoreTrigger(
    const TTAMachine::HWOperation& op) {
    string address = SymbolGenerator::portSymbol(*op.port(1)) + ".value_.uIntWord";
    string dataToWrite = SymbolGenerator::portSymbol(*op.port(2)) + ".value_.uIntWord";
    string memory = SymbolGenerator::DAMemorySymbol(op.parentUnit()->name());
    string method;
    
    if (op.name() == "stq") {
        method = "fastWriteMAU";
    } else if (op.name() == "sth") {
        method = "fastWrite2MAUs";
    } else if (op.name() == "stw") {
        method = "fastWrite4MAUs";
    }
    
    return memory + "." + method + "(" + address + ", " + dataToWrite + ");";
}

/**
 * Generates a faster version of specific load triggers
 * @param op The load operation (either ldw, ldq, ldh, ldhu, or ldqu)
 */
string 
CompiledSimCodeGenerator::generateLoadTrigger(
    const TTAMachine::HWOperation& op) {
    const FunctionUnit& fu = *op.parentUnit();
    std::stringstream ss;
    string address = SymbolGenerator::portSymbol(*op.port(1)) + ".value_.uIntWord";
    string memory = SymbolGenerator::DAMemorySymbol(op.parentUnit()->name());
    string MAUSize = Conversion::toString(fu.addressSpace()->width());
    string method;
    string extensionMode="SIGN_EXTEND";
    string resultSignExtend;
    
    if (op.name() == "ldqu" || op.name() == "ldhu") {
        extensionMode = "ZERO_EXTEND";
    }
    
    if (op.name() == "ldq" || op.name() == "ldqu") {
        method = "fastReadMAU";
        resultSignExtend = "read_data_tmp = " + extensionMode 
            + "(read_data_tmp, " + MAUSize + ");";
    } else if (op.name() == "ldh" || op.name() == "ldhu") {
        method = "fastRead2MAUs";
        resultSignExtend = "read_data_tmp = " + extensionMode 
            + "(read_data_tmp, (" + MAUSize + "*2));";
    } else if (op.name() == "ldw") {
        method = "fastRead4MAUs";
    }
    
    ss << " { static UIntWord read_data_tmp; " << memory + "." << method << "("
       << address << ", read_data_tmp); ";

    ss << resultSignExtend << " ";

    ss << "addFUResult(" << SymbolGenerator::FUResultSymbol(*op.port(2)) 
       << ", " << "cycleCount_, read_data_tmp, " << op.latency() << "); }";
    
    return ss.str();
}

/**
 * Returns the maximum possible latency from the FUs & GCU
 * 
 * @return the maximum possible latency from the FUs & GCU
 */
int
CompiledSimCodeGenerator::maxLatency() const {    
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();    
    int maxLatency = machine_.controlUnit()->globalGuardLatency();
    for (int i = 0; i < fus.count(); ++i) {
        if (maxLatency < fus.item(i)->maxLatency()) {
            maxLatency = fus.item(i)->maxLatency();
        }
    }
    return maxLatency;
}

/**
 * Returns the output ports of a function unit
 * 
 * @param fu The function unit
 * 
 * @return A vector of output ports of a function unit
 */
std::vector<TTAMachine::Port*> 
CompiledSimCodeGenerator::fuOutputPorts(
    const TTAMachine::FunctionUnit& fu) const {
    std::vector<TTAMachine::Port*> ports;
    for (int i = 0; i < fu.portCount(); ++i) {
        if (fu.port(i)->isOutput()) {
            ports.push_back(fu.port(i));
        }
    }
    return ports;
}
