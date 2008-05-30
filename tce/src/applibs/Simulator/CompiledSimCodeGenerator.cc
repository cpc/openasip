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
#include "CompiledSimCompiler.hh"

using namespace TTAMachine;
using namespace TTAProgram;
using std::string;
using std::fstream;
using std::endl;
using std::vector;

/**
 * The constructor
 * 
 * Gets the settings for compiled simulation code generation.
 * 
 * @param machine The machine to run the simulation on
 * @param program The simulated program
 * @param controller Compiled Simulation controller
 * @param sequentialSimulation Is it a sequential simulation?
 * @param fuResourceConflictDetection is the conflict detection on?
 * @param handleCycleEnd should we let frontend handle each cycle end
 * @param basicBlockPerFile Should we generate only one BB per code file?
 */
CompiledSimCodeGenerator::CompiledSimCodeGenerator(
    const TTAMachine::Machine& machine,
    const TTAProgram::Program& program,
    const TTASimulationController& controller,
    bool sequentialSimulation,
    bool fuResourceConflictDetection,
    bool handleCycleEnd,
    bool dynamicCompilation,
    bool basicBlockPerFile,
    bool functionPerFile) :
    machine_(machine), program_(program), simController_(controller),
    gcu_(*machine.controlUnit()),
    isSequentialSimulation_(sequentialSimulation),
    handleCycleEnd_(handleCycleEnd),
    dynamicCompilation_(dynamicCompilation),
    basicBlockPerFile_(basicBlockPerFile),
    functionPerFile_(functionPerFile),
    instructionNumber_(0), instructionCounter_(0),
    isProcedureBegin_(true), currentProcedure_(0),
    pendingJumpDelay_(0), lastInstructionOfBB_(0),
    className_("CompiledSimulationEngine"),
    os_(NULL),
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
    headerFile_ = className_ + ".hh";
    mainFile_ = className_ + ".cc";

    generateMakefile();
    generateSimulationCode();
    generateHeaderAndMainCode();
}

/**
 * Generates a Makefile for compiling the simulation engine.
 */
void
CompiledSimCodeGenerator::generateMakefile() {

    currentFileName_ =
        targetDirectory_ + FileSystem::DIRECTORY_SEPARATOR + "Makefile";

    std::ofstream makefile(currentFileName_.c_str());
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
        << "soflags = " << CompiledSimCompiler::COMPILED_SIM_SO_FLAGS << endl 
            
        // use because ccache doesn't like changing directory paths
        // (in a preprocessor comment)
        << "cppflags = " << CompiledSimCompiler::COMPILED_SIM_CPP_FLAGS << endl
        << endl
        
        << "all: CompiledSimulationEngine.so" << endl << endl

        << "CompiledSimulationEngine.so: CompiledSimulationEngine.hh.gch $(dobjects) CompiledSimulationEngine.cc" << endl
        << "\t#@echo Compiling CompiledSimulationEngine.so" << endl
        << "\t$(CC) $(soflags) $(cppflags) -O0 $(includes) CompiledSimulationEngine.cc "
        << "-c -o CompiledSimulationEngine.o" << endl 
        << "\t$(CC) $(soflags) CompiledSimulationEngine.o -o CompiledSimulationEngine.so"
        << endl << endl
            
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
        << "\t@rm -f $(dobjects) CompiledSimulationEngine.so CompiledSimulationEngine.hh.gch" << endl;
    
    makefile.close();
    currentFileName_.clear();
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
CompiledSimCodeGenerator::AddressMap 
CompiledSimCodeGenerator::basicBlocks() const {
    if (bbStarts_.empty()) {
        findBasicBlocks();
    }
    return bbStarts_;
}

/**
 * Returns a struct of basic blocks and their corresponding code files
 * 
 * @return a struct of basic blocks and their corresponding code files
 */
ProcedureBBRelations 
CompiledSimCodeGenerator::procedureBBRelations() const {
    return procedureBBRelations_;
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
    currentFileName_ = targetDirectory_ + DS + headerFile_; 
    currentFile_.open(currentFileName_.c_str(), fstream::out);
    os_ = &currentFile_;
    
    // Generate includes
    *os_ << "// " << className_ << " Generated automatically by ttasim" << endl
         << "#ifndef _AUTO_GENERATED_COMPILED_SIMULATION_H_" << endl
         << "#define _AUTO_GENERATED_COMPILED_SIMULATION_H_" << endl
         << "#include \"SimValue.hh\"" << endl
         << "#include \"DirectAccessMemory.hh\"" << endl
         << "#include \"OSAL.hh\"" << endl
         << "#include \"Operation.hh\"" << endl
         << "#include \"OperationPool.hh\"" << endl
         << "#include \"OperationContext.hh\"" << endl
         << "#include \"CompiledSimulation.hh\"" << endl
         << "#include \"BaseType.hh\"" << endl
         << conflictDetectionGenerator_.includes() << endl
         << endl;
    
    // Open up class declaration and define some extra member variables
    *os_ << "class " << className_ << ";" << endl << endl;
    *os_ << "class " << className_ << " : public CompiledSimulation {" << endl
         << "public:" << endl;

    // Declare all FUs
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();
    for (int i = 0; i < fus.count(); ++i) {
        const FunctionUnit& fu = *fus.item(i);
        
        // FU Ports
        for (int j = 0; j < fu.operationPortCount(); ++j) {
            const FUPort& port = *fu.operationPort(j);
            addDeclaredSymbol(SymbolGenerator::portSymbol(port), port.width());    
        }                
    
        // Conflict detectors
        if (conflictDetectionGenerator_.conflictDetectionEnabled()) {
            *os_ << conflictDetectionGenerator_.symbolDeclaration(fu);
        }

        // Operation contexts
        *os_ << "\t" << "OperationContext " 
             << SymbolGenerator::operationContextSymbol(fu) << ";" << endl;
        
        // Address spaces
        if (fu.addressSpace() != NULL) {
             *os_ << "\t" << "DirectAccessMemory& " 
                  << SymbolGenerator::DAMemorySymbol(*fus.item(i)) << ";" 
                  << endl;
        }
        
        // All operations of the FU
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
    const Machine::ImmediateUnitNavigator& ius = 
        machine_.immediateUnitNavigator();
    for (int i = 0; i < ius.count(); ++i) {
        const ImmediateUnit& iu = *ius.item(i);
        for (int j = 0; j < iu.numberOfRegisters(); ++j) {
            addDeclaredSymbol(SymbolGenerator::immediateRegisterSymbol(iu, j), 
                iu.width());
        }
    }
    
    // Register files
    if (!isSequentialSimulation_) {
        const Machine::RegisterFileNavigator& rfs = 
            machine_.registerFileNavigator();
        for (int i = 0; i < rfs.count(); ++i) {
            const RegisterFile& rf = *rfs.item(i);
            for (int j = 0; j < rf.numberOfRegisters(); ++j) {
                addDeclaredSymbol(SymbolGenerator::registerSymbol(rf, j), 
                    rf.width());
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
    
    // Generate dummy destructor
    *os_ << "EXPORT virtual ~" << className_ << "() { }" << endl << endl;

    *os_ << "EXPORT virtual void simulateCycle();" << endl << endl << "}; // end class"
         << endl << endl << "#endif // include once" << endl << endl;

    // header written
    currentFile_.close();
    currentFileName_.clear();
    os_ = NULL;

    generateConstructorCode();
}

/**
 * Generates the parameter list for the constructor.
 */
void
CompiledSimCodeGenerator::generateConstructorParameters() {
    *os_ << className_ 
         << "(const TTAMachine::Machine& machine," << endl
         << "InstructionAddress entryAddress," << endl
         << "InstructionAddress lastInstruction," << endl
         << "SimulatorFrontend& frontend," << endl
         << "MemorySystem& memorySystem," << endl
         << "bool dynamicCompilation,"
         << "ProcedureBBRelations& procedureBBRelations)";
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
    *os_ << "EXPORT " << className_ << "::";
    
    generateConstructorParameters();
        
    *os_ << " : " << endl
         << "CompiledSimulation(machine, entryAddress, lastInstruction, "
         << "frontend, memorySystem, dynamicCompilation, procedureBBRelations),"
         << endl;
    
    updateDeclaredSymbolsList();
    updateSymbolsMap();
    
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();    
    for (int i = 0; i < fus.count(); i++) {
        const FunctionUnit& fu = *fus.item(i);
        std::string context = SymbolGenerator::operationContextSymbol(fu);
        
        // Create a state for each operation
        for (int j = 0; j < fu.operationCount(); ++j) {
            std::string operation = SymbolGenerator::operationSymbol(
                fu.operation(j)->name(), fu);
            
            *os_ << "\t" << operation << ".createState(" << context << ");" 
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
    
    // generate simulateCycle() method
    *os_ << "// Simulation code:" << endl
         << "EXPORT void " << className_  << "::simulateCycle() {" << endl << endl;

    // Create a jump dispatcher for accessing each basic block start
    *os_ << "\t// jump dispatcher" << endl
         << "\tjumpTargetFunc_ = getSimulateFunction(jumpTarget_);" << endl
         << "\t(this->*jumpTargetFunc_)();" << endl << endl;
    
    *os_ << conflictDetectionGenerator_.notifyOfConflicts()
         << "}" << endl << endl;
    
    generateSimulationGetter(); // generate the simulation getter
}

/**
 * Generates the simulateCycle() function that is called outside the .so file
 * 
 * The generated function simulates one basic block at a time by calling 
 * the according basic block methods.
 */
void
CompiledSimCodeGenerator::generateSimulationCode() {
    
    findBasicBlocks();
    exitPoints_ = simController_.findProgramExitPoints(program_, machine_);

    // generate code for all procedures
    for (int i = 0; i < program_.procedureCount(); ++i) {
        generateProcedureCode(program_.procedure(i));
    }

    // Close the last file
    currentFile_.close();
    currentFileName_.clear();
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
 * @param procedure the procedure to generate code from
 * @exception InstanceNotFound if the first instruction wasn't found
 */
void
CompiledSimCodeGenerator::generateProcedureCode(const Procedure& procedure) {
    const Instruction* instruction = &procedure.firstInstruction();
    currentProcedure_ = &procedure;
    isProcedureBegin_ = true;
    while (instruction != &NullInstruction::instance()) {
        generateInstruction(*instruction);
        instruction = &(procedure.nextInstruction(*instruction));
        instructionNumber_++;
        instructionCounter_--;
        isProcedureBegin_ = false;
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
         << "extern \"C\" EXPORT CompiledSimulation* getSimulation(" << endl
         << "\tconst TTAMachine::Machine& machine," << endl
         << "\tInstructionAddress entryAddress," << endl
         << "\tInstructionAddress lastInstruction," << endl
         << "\tSimulatorFrontend& frontend," << endl
         << "\tMemorySystem& memorySystem," << endl
         << "\tbool dynamicCompilation," << endl
         << "\tProcedureBBRelations& procedureBBRelations) {" << endl
         << "\treturn new " << className_
         << "(machine, entryAddress, lastInstruction, frontend, memorySystem, "
         << "dynamicCompilation, procedureBBRelations); " 
         << endl << "}" << endl << endl; // 2x end-of-line in the end of file
}

/**
 * Generates code for halting the simulation
 * 
 * @param message Reason for the halt
 * @return generated code for halting the simulation
 */
string
CompiledSimCodeGenerator::generateHaltCode(const string& message) {
    return std::string("\thaltSimulation(__FILE__, __LINE__, __FUNCTION__, \""
       + message + "\");\n");
}

/**
 * Generates code for advancing clocks of various items per cycle
 */
void CompiledSimCodeGenerator::generateAdvanceClockCode() {
    *os_ << endl << "void inline advanceClocks() {" << endl
         << conflictDetectionGenerator_.advanceClockCode()
         << endl << "}" << endl;
}

/**
 * Updates the declared symbols list after the program code is generated.
 * 
 * Generates constructor calls for each declared symbol.
 */
void 
CompiledSimCodeGenerator::updateDeclaredSymbolsList() {
    
    // Constructor calls for SimValues
    for (SimValueSymbolDeclarations::iterator it = declaredSymbols_.begin();
        it != declaredSymbols_.end(); ++it) {
        *os_ << "\t";
        if (it != declaredSymbols_.begin()) {
            *os_ << ",";
        }
        *os_ << it->first << "(" << Conversion::toString(it->second) 
             << ")" << endl;
    }
    if (!declaredSymbols_.empty()) {
        *os_ << endl;
    }

    // Operations
    for (OperationSymbolDeclarations::iterator it = usedOperations_.begin();
         it != usedOperations_.end(); ++it) {
        *os_ << "\t," << it->second
             << "(operationPool_.operation(\"" << it->first << "\"))" << endl;
    }
    if (!usedOperations_.empty()) {
        *os_ << endl;
    }
    
    // FU result symbols
    const Machine::FunctionUnitNavigator& fus = machine_.functionUnitNavigator();    
    for (int i = 0; i < fus.count(); ++i) {
        const FunctionUnit& fu = *fus.item(i);
        std::vector<Port*> outPorts = fuOutputPorts(fu);
        for (size_t j = 0; j < outPorts.size(); ++j) {
            *os_ << "\t," << SymbolGenerator::FUResultSymbol(*outPorts.at(j)) 
                 << "(" << Conversion::toString(fu.maxLatency()) << ")";
            *os_ << endl;
        }
    }
    if (fus.count() > 0) {
        *os_ << endl;
    }
    // Conflict detectors
    *os_ << conflictDetectionGenerator_.updateSymbolDeclarations() << endl;

    // DirectAccessMemories
    for (int i = 0; i < fus.count(); i++) {
        const FunctionUnit & fu = *fus.item(i);
        if (fu.addressSpace() != NULL) {
            *os_ << "\t," << SymbolGenerator::DAMemorySymbol(fu) 
                 << "(FUMemory(\"" << fu.name() << "\"))" << endl;
        }
    }
    *os_ << " {" << endl;
}

/**
 * Updates the Symbols map of the CompiledSimulation class
 */
void 
CompiledSimCodeGenerator::updateSymbolsMap() {
    for (SimValueSymbolDeclarations::const_iterator it =
        declaredSymbols_.begin(); it != declaredSymbols_.end(); ++it) {
        string symbolName = it->first;
        *os_ << "\t" << "addSymbol(\"" << symbolName << "\", " 
             << symbolName << ");" << endl;
    }
}

/**
 * Generates declarations for all the symbols in the declared symbols -list 
 * 
 * (SimValues, bool guards...)
 */
void 
CompiledSimCodeGenerator::generateSymbolDeclarations() {
    for (SimValueSymbolDeclarations::const_iterator it = 
        declaredSymbols_.begin(); it != declaredSymbols_.end(); ++it) {
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
    *os_ << "\t" << "resizeJumpTable(lastInstruction_ + 1);" << endl;

    // If static simulation, set all jump targets in the constructor.
    if (!dynamicCompilation_) {
        for (AddressMap::const_iterator it = bbStarts_.begin();
            it != bbStarts_.end(); ++it) {
            *os_ << "\t" << "setJumpTargetFunction(" << it->first << ", &"
                << className_  << "::"
                << SymbolGenerator::basicBlockSymbol(it->first) << ");" << endl;
        }
    }
}

/**
 * Adds a new declared symbol to the map
 * 
 * @param name name of the symbol
 * @param width SimValue width
 */
void 
CompiledSimCodeGenerator::addDeclaredSymbol(const string& name, int width) {
    declaredSymbols_[name] = width;
}

/**
 * Finds the used RF symbols in sequential simulation
 */
void 
CompiledSimCodeGenerator::addUsedRFSymbols() {
    assert (isSequentialSimulation_);
    
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
    if (op.name() == "call") { // save return address
        ss << SymbolGenerator::returnAddressSymbol(gcu_) << ".value_.uIntWord = " 
           << instructionNumber_ + pendingJumpDelay_ << "u;" << endl;
    }
    return ss.str();
}

/**
 * Generates code for a triggered operation.
 * 
 * @param op The triggered operation
 * @return A std::string containing generated code for the operation call
 */
string
CompiledSimCodeGenerator::handleOperation(const TTAMachine::HWOperation& op) {
    std::stringstream ss;
                     
    ss << endl << "/* Operation: " << op.name() << ", latency: "
       << op.latency() << " */" << endl;
    
    if (operationPool_.operation(op.name().c_str()).dagCount() <= 0) {
        ss << handleOperationWithoutDag(op);
    } else {
        /// @todo maybe use IsCall & IsControlFlowOperation.
        if (op.name() != "jump" && op.name() != "call") {
            ss << "#define context "
               << SymbolGenerator::operationContextSymbol(*op.parentUnit())
               << endl
               << "#define opPool_ operationPool_" << endl
               << generateTriggerCode(op) << endl
               << "#undef context" << endl
               << "#undef opPool_" << endl
               << endl;
        } else { // simulate a jump
            ss << handleJump(op);
        }
    }
    return ss.str();
}

/**
 * Generates code for a triggered operation that has no DAG available
 * 
 * @param op The triggered operation
 * @return A std::string containing generated code for the operation call
 */
string
CompiledSimCodeGenerator::handleOperationWithoutDag(
    const TTAMachine::HWOperation& op) {
    std::stringstream ss;        

    std::vector<string> operandSymbols;
    string operandTable = SymbolGenerator::generateTempVariable();
    
    // Generate symbols for each operand, and temporaries for output operands
    for (int i = 1; op.port(i) != NULL; ++i) {
        if (op.port(i)->isOutput()) {
            string outputSymbol = SymbolGenerator::generateTempVariable();
            operandSymbols.push_back(outputSymbol);
            ss << "static SimValue " << outputSymbol << ";" << endl;
        } else { // input port
            string inputSymbol = SymbolGenerator::portSymbol(*op.port(i));
            operandSymbols.push_back(inputSymbol);
        }
    }
    
    // Add operand symbols to the operand table
    ss << "SimValue* " << operandTable << "[] = {";
    for (size_t i = 0; i < operandSymbols.size(); ++i) {
      ss << "&" << operandSymbols.at(i);
      if (i < operandSymbols.size() - 1) {
          ss << ",";
      }
      ss << " ";
    }
    ss << "};";
    
    // call simulateTrigger with the previously generated operand table
    /// @todo maybe use IsCall & IsControlFlowOperation.
    if (op.name() != "jump" && op.name() != "call") {
        ss << SymbolGenerator::operationSymbol(op.name(), *op.parentUnit())
           << ".simulateTrigger(" << operandTable << ", "
           << SymbolGenerator::operationContextSymbol(*op.parentUnit())
           << "); ";
               
        // add output values as delayed assignments
        for (int i = 1; op.port(i) != NULL; ++i) {
            if (op.port(i)->isOutput()) {
                ss << generateAddFUResult(*op.port(i), operandSymbols.at(i - 1), 
                    op.latency());
            }
        }
    } else { // simulate a jump
        ss << handleJump(op);
    }

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
    
    // Find out the guard type
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
        
    // Make sure to create only one bool per guard read and store the symbol
    if (usedGuardSymbols_.find(guardSymbolName) == usedGuardSymbols_.end()) {
        lastGuardBool_ = SymbolGenerator::guardBoolSymbol();
        usedGuardSymbols_[guardSymbolName] = lastGuardBool_;
        
        ss << "const bool " << lastGuardBool_ << " = !(" 
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
            
    // Handle inverted guards
    ss << endl << "if (";
    if (guard.isInverted()) {
        lastGuardBool_ = "!" + lastGuardBool_;
    }
    ss << lastGuardBool_ << ") { ";   
    
    return ss.str();
}

/**
 * Generates simulation code of the given instruction
 * 
 * @param instruction instruction to generate the code from
 */
void
CompiledSimCodeGenerator::generateInstruction(const Instruction& instruction) {

    InstructionAddress address = instruction.address().location();
    usedGuardSymbols_.clear();
    
    // Are we at the start of a new basic block?
    if (bbStarts_.find(address) != bbStarts_.end()) {
        // Should we start with a new file?
        if (instructionCounter_ <= 0 || basicBlockPerFile_ ||
            (functionPerFile_ && isProcedureBegin_)) {
            instructionCounter_ = MAX_INSTRUCTIONS_PER_FILE;
            
            if (currentFile_.is_open()) {
                currentFile_.close();
            }
            
            // Generate a new file to begin to work with
            const string DS = FileSystem::DIRECTORY_SEPARATOR;
            currentFileName_ = targetDirectory_ + DS 
                + SymbolGenerator::basicBlockSymbol(address) + ".cpp";
            currentFile_.open(currentFileName_.c_str(), fstream::out);
            createdFiles_.insert(currentFileName_);
            os_ = &currentFile_;
            *os_ << "// " << className_ 
                 << " Generated automatically by ttasim" << endl
                 << "#include \"" << headerFile_ << "\"" << endl << endl;
        }
        
        lastInstructionOfBB_ = bbStarts_.find(address)->second;
        declaredFunctions_.insert(SymbolGenerator::basicBlockSymbol(address));
        
        // Save basic block<->procedure related information
        InstructionAddress procedureStart = currentProcedure_->
            firstInstruction().address().location();
        procedureBBRelations_.procedureStart[address] = procedureStart;
        procedureBBRelations_.basicBlockFiles[address] = currentFileName_;
        procedureBBRelations_.basicBlockStarts.insert(std::make_pair(
            procedureStart, address));
        
        // Setup a jump target setter function
        *os_ << endl << "extern \"C\" EXPORT void "
             << SymbolGenerator::jumpTargetSetterSymbol(address)
             << "(CompiledSimulation& compiledSimulation) {" << endl
             << "\tsetJumpTargetFunction(compiledSimulation, " << address 
             << ", &" << className_ << "::" 
             << SymbolGenerator::basicBlockSymbol(address) << ");"
             << endl << "}" << endl;
        
        // Start a new C++ function for the basic block
        if (isProcedureBegin_) {
            *os_ << "/* Procedure " << currentProcedure_->name() 
                 << " */" << endl;
        }
        *os_ << endl << "EXPORT void " << className_ << "::" 
             << SymbolGenerator::basicBlockSymbol(address) << "() {" << endl;
        
        lastFUWrites_.clear();
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

        *os_ << SymbolGenerator::immediateRegisterSymbol(immediate.destination());
        int value = immediate.value().value().unsignedValue();
        *os_  << ".value_.uIntWord = " << value << "u;";
    }
    
    // Get FU Results if there are any ready
    std::set<std::string> gotResults; // used to get FU results only once/instr.
    DelayedAssignments::iterator it= delayedFUResultWrites_.find(
        instructionNumber_);
    while (it != delayedFUResultWrites_.end()) {
        *os_ << "clearFUResults(" << it->second.fuResultSymbol << ");" << endl;
        *os_ << it->second.targetSymbol << " = " << it->second.sourceSymbol 
             << ";" << std::endl;        
        gotResults.insert(it->second.targetSymbol);
        delayedFUResultWrites_.erase(it);
        it = delayedFUResultWrites_.find(instructionNumber_);
    }
    
    bool endGuardBracket = false;
    
    // Do moves
    std::vector<CompiledSimMove> lateMoves; // moves with buses
    for (int i = 0; i < instruction.moveCount(); ++i) {
        const Move& move = instruction.move(i);
        string moveSource = SymbolGenerator::moveOperandSymbol(
            move.source(), move);
        string moveDestination = SymbolGenerator::moveOperandSymbol(
            move.destination(), move);
        
        lastGuardBool_.clear();
        if (!move.isUnconditional()) { // has a guard?
            *os_ << handleGuard(move.guard().guard(), move.isControlFlowMove());
            endGuardBracket = true;
        }

        if (move.source().isFUPort() && gotResults.find(moveSource) == gotResults.end() &&  
            dynamic_cast<const ControlUnit*>(&move.source().functionUnit()) == NULL) {
            *os_ << generateFUResultRead(moveSource,
                SymbolGenerator::FUResultSymbol(move.source().port())) << endl;
            gotResults.insert(moveSource);
        }
        
        // Find all moves that depend on others i.e. those moves that have to
        // be done last.
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
            *os_ << moveDestination << " = " << moveSource << "; ";
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
        
        string moveSource = SymbolGenerator::moveOperandSymbol(
            move.source(), move);
        string moveDestination = SymbolGenerator::moveOperandSymbol(
            move.destination(), move);
        
        if (!move.isUnconditional()) { // has a guard?
            *os_ << handleGuard(move.guard().guard(), move.isControlFlowMove());
            endGuardBracket = true;
        }
        
        if (move.source().isFUPort() && gotResults.find(moveSource) == 
            gotResults.end() && dynamic_cast<const ControlUnit*>(
                &move.source().functionUnit()) == NULL) {
            *os_ << generateFUResultRead(moveDestination,
                SymbolGenerator::FUResultSymbol(move.source().port()))
                 << endl;

            gotResults.insert(SymbolGenerator::moveOperandSymbol(move.source(),
                move));
        }
        
        const TerminalFUPort& tfup = 
            dynamic_cast<const TerminalFUPort&>(move.destination());
        HWOperation& hwOperation = *tfup.hwOperation();

        *os_ << handleOperation(hwOperation)
             << conflictDetectionGenerator_.detectConflicts(hwOperation);

        if (endGuardBracket) {
            *os_ << "}";
            endGuardBracket = false;
        }
    } // end for
    
    // Do immediate assignments for everything else
    for (int i = 0; i < instruction.immediateCount(); ++i) {
        const Immediate& immediate = instruction.immediate(i);
           
        if (immediate.destination().isFUPort()) {
            continue;
        }
            
        *os_ << SymbolGenerator::immediateRegisterSymbol(immediate.destination());
        unsigned int value = immediate.value().value().unsignedValue();
        *os_  << ".value_.uIntWord = " << value << "u;";
    }
    
    // Do bus moves
    for (std::vector<CompiledSimMove>::const_iterator it = lateMoves.begin();
        it != lateMoves.end(); ++it) {
            *os_ << it->copyFromBusCode();
    }

    // No operation?
    if (instruction.moveCount() == 0 && instruction.immediateCount() == 0) {
        *os_ << "/* NOP */" << endl;
    }
    
    // Let frontend handle cycle end?
    if (handleCycleEnd_) {
        *os_ << "cycleEnd();" << endl;
    }

    *os_ << "cycleCount_++;" << endl;
    
    AddressMap::iterator bbEnd = bbEnds_.find(address);
    
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
        
        // Generate shutdown code after the last instruction
        if (address == program_.lastInstruction().address().location()) {
            generateShutdownCode(address);
        }
        
         *os_ << endl << "} /* end function */" << endl << endl;
    }
}

/**
 * Generates code for calling triggered operations
 * 
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
    OperationDAG* dag = &operationPool_.operation(op.name().c_str()).dag(0);
    string simCode = OperationDAGConverter::createSimulationCode(*dag, &operands);
        
    // add output values as delayed assignments
    while (simCode.find("outputvalue = ") != string::npos) {
        std::size_t begin = simCode.find("outputvalue = ");
        std::size_t end = simCode.find(";", begin);
        simCode.erase(begin, end);
    }
    
    for (int i = 1, tmp=1; op.port(i) != NULL; ++i) {
        if (op.port(i)->isOutput()) {
            string tempVariable = SymbolGenerator::generateTempVariable();
            string ioSymbol = "tmp" + Conversion::toString(tmp);
            
            if (simCode.find(ioSymbol) == string::npos) {
                continue;
            }

            while (simCode.find(ioSymbol) != string::npos) {
                string::iterator it = simCode.begin() + simCode.find(ioSymbol);
                simCode.replace(it, it + ioSymbol.length(), tempVariable);
            }
            
            simCode.append("\n" + generateAddFUResult(*op.port(i), 
                tempVariable, op.latency()));
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
    string address = SymbolGenerator::portSymbol(*op.port(1)) + ".uIntWordValue()";
    string dataToWrite = SymbolGenerator::portSymbol(*op.port(2)) + ".uIntWordValue()";
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
    string address = SymbolGenerator::portSymbol(*op.port(1)) + ".uIntWordValue()";
    string memory = SymbolGenerator::DAMemorySymbol(op.parentUnit()->name());
    string MAUSize = Conversion::toString(fu.addressSpace()->width());
    string method;
    string extensionMode="SIGN_EXTEND";
    string resultSignExtend;
    string temp = SymbolGenerator::generateTempVariable();
    
    if (op.name() == "ldqu" || op.name() == "ldhu") {
        extensionMode = "ZERO_EXTEND";
    }
    
    if (op.name() == "ldq" || op.name() == "ldqu") {
        method = "fastReadMAU";
        resultSignExtend = temp + " = " + extensionMode 
            + "(" + temp + ", " + MAUSize + ");";
    } else if (op.name() == "ldh" || op.name() == "ldhu") {
        method = "fastRead2MAUs";
        resultSignExtend = temp + " = " + extensionMode 
            + "(" + temp + ", (" + MAUSize + "*2));";
    } else if (op.name() == "ldw") {
        method = "fastRead4MAUs";
    }
    
    ss << "static UIntWord " << temp << "; " << memory + "." << method << "("
       << address << ", " << temp << "); ";

    ss << resultSignExtend << " ";

    ss << generateAddFUResult(*op.port(2), temp, op.latency());
    
    return ss.str();
}

/**
 * Generates code for adding a result to FU's output port
 * 
 * Handles static latency simulation in case it is possible, otherwise
 * reverts back to older dynamic model.
 * 
 * The case this function tracks is the following:
 * 
 * 1) Instruction address > basic block start + latency
 * 2) Trigger not inside a guard
 * 3) Result must be ready in the same basic block
 * 4) No overlapping writes of any kind
 * 
 * @param resultPort FU result port to set the value to
 * @param value value to set as a result
 * @param latency latency of the operation
 * @return the generated code for putting the results.
 */
std::string 
CompiledSimCodeGenerator::generateAddFUResult(
    const TTAMachine::FUPort& resultPort,
    const std::string& value,  
    int latency) {
        
    std::stringstream ss;
    const FunctionUnit& fu = *resultPort.parentUnit();
    const int writeTime = instructionNumber_ + latency;
    
    AddressMap::iterator bbEnd = bbEnds_.lower_bound(instructionNumber_);
    
    const bool resultInSameBasicBlock = bbEnd->first ==
        bbEnds_.lower_bound(writeTime)->first;
    
    const int bbStart = bbEnd->second;
    bool staticSimulationPossible = false;
    const std::string destination = SymbolGenerator::portSymbol(resultPort);
    
    int lastWrite = 0;
    if (lastFUWrites_.find(destination) != lastFUWrites_.end()) {
        lastWrite = lastFUWrites_[destination];
    }

    // If no more pending results are coming outside or inside the basic block,
    // no guard of any kind and result will be ready in the same basic block,
    // then static latency simulation can be done.
    if ((writeTime >= bbStart + fu.maxLatency())
        && lastGuardBool_.empty() && resultInSameBasicBlock 
        && (writeTime > lastWrite)) {
        staticSimulationPossible = true;
        
        for (int i = instructionNumber_ + 1; i < writeTime 
             && staticSimulationPossible; ++i) {

            Instruction& instr = program_.instructionAt(i);
            for (int j = 0; j < instr.moveCount(); ++j) {
                if ((instr.move(j).isTriggering() && fu.name() 
                    == instr.move(j).destination().functionUnit().name()) || 
                     (instr.move(j).source().isGPR() && 
                     instr.move(j).source().port().name()==resultPort.name())) {
                    staticSimulationPossible = false;
                    break;
                }
            }
        }
    }   

    if (staticSimulationPossible) { // Add a new delayed assignment
        DelayedAssignment assignment = { value, destination, 
            SymbolGenerator::FUResultSymbol(resultPort) };
        delayedFUResultWrites_.insert(std::make_pair(writeTime, assignment));
    } else { // revert to old dynamic FU result model
        ss << "addFUResult(" << SymbolGenerator::FUResultSymbol(resultPort)
           << ", " << "cycleCount_, " << value << ", " << latency << ");";
        if (writeTime > lastWrite) {
            lastFUWrites_[destination] = writeTime;
        }
    }
    
    return ss.str();
}

/**
 * Generates code for reading FU results from result symbol to result port
 * 
 * @param destination destination port symbol
 * @param resultSymbol results symbol
 * @return generated code for getting the result
 */
std::string
CompiledSimCodeGenerator::generateFUResultRead(
    const std::string& destination, 
    const std::string& resultSymbol) {
    std::stringstream ss;
    
    ss << "FUResult(" << destination << ", " << resultSymbol
       << ", cycleCount_);" << endl;
    
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
