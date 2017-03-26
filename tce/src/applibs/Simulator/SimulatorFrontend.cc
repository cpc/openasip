/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file SimulatorFrontend.cc
 *
 * Implementation of SimulatorFrontend class
 *
 * @author Pekka J��skel�inen 2005,2010-2012 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: red
 */

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wkeyword-macro")
#include <boost/regex.hpp>
POP_CLANG_DIAGS
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/version.hpp>

#include "Binary.hh"
#include "BinaryReader.hh"
#include "BinaryStream.hh"
#include "ADFSerializer.hh"
#include "SimulatorFrontend.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "Conversion.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "ProcessorConfigurationFile.hh"
#include "SimulationController.hh"
#include "UniversalMachine.hh"
#include "UniversalFunctionUnit.hh"
#include "HWOperation.hh"
#include "RegisterFileState.hh"
#include "StringTools.hh"
#include "MachineState.hh"
#include "UnboundedRegisterFile.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "GCUState.hh"
#include "NullMachine.hh"
#include "SpecialRegisterPort.hh"
#include "TPEFProgramFactory.hh"
#include "POMDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "MemorySystem.hh"
#include "TPEFBaseType.hh"
#include "Program.hh"
#include "NullProgram.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "ExecutionTracker.hh"
#include "ExecutionTrace.hh"
#include "SimulatorConstants.hh"
#include "StopPointManager.hh"
#include "TPEFTools.hh"
#include "UtilizationStats.hh"
#include "SimulationStatistics.hh"
#include "RFAccessTracker.hh"
#include "BusTracker.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "ProcedureTransferTracker.hh"
#include "POMValidator.hh"
#include "POMValidatorResults.hh"
#include "Exception.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "CompiledSimController.hh"
#include "TCEDBGController.hh"
#include "CustomDBGController.hh"
#include "CompiledSimUtilizationStats.hh"
#include "SimulationEventHandler.hh"
#include "MachineInfo.hh"
#include "DirectAccessMemory.hh"
#include "IdealSRAM.hh"
#include "RemoteMemory.hh"
#include "MemoryProxy.hh"
#include "DisassemblyFUPort.hh"

using namespace TTAMachine;
using namespace TTAProgram;
using namespace TPEF;


/**
 * Constructor.
 */
SimulatorFrontend::SimulatorFrontend(SimulationType backendType) : 
    currentMachine_(NULL), machineState_(NULL), simCon_(NULL),
    machineOwnedByFrontend_(false), currentProgram_(NULL), 
    programFileName_(""), programOwnedByFrontend_(false), 
    currentBackend_(backendType),
    disassembler_(NULL), traceFileName_(""), executionTracing_(false),
    busTracing_(false), 
    rfAccessTracing_(false), procedureTransferTracing_(false), 
    saveProfileData_(false), saveUtilizationData_(false),
    traceDB_(NULL), lastTraceDB_(NULL),
    executionTracker_(NULL), busTracker_(NULL),
    rfAccessTracker_(NULL), procedureTransferTracker_(NULL),
    stopPointManager_(NULL),  utilizationStats_(NULL), tpef_(NULL),
    fuResourceConflictDetection_(true),
    printNextInstruction_(true), printSimulationTimeStatistics_(false),
    staticCompilation_(true), traceFileNameSetByUser_(false), outputStream_(0),
    memoryAccessTracking_(false), eventHandler_(NULL), lastRunCycleCount_(0),
    lastRunTime_(0.0), simulationTimeout_(0), leaveCompiledDirty_(false),
    memorySystem_(NULL), zeroFillMemoriesOnReset_(true) {

    if (backendType == SIM_COMPILED) {
        setFUResourceConflictDetection(false); // disabled by default
        SimulatorToolbox::textGenerator().generateCompiledSimTexts();
    } 
}

/**
 * Destructor.
 */
SimulatorFrontend::~SimulatorFrontend() {

    finishSimulation();

    if (machineOwnedByFrontend_) {
        delete currentMachine_;
        currentMachine_ = NULL;
    }

    if (programOwnedByFrontend_) {
        delete currentProgram_;
        currentProgram_ = NULL;
        programFileName_ = "";
    }

    delete disassembler_;
    disassembler_ = NULL;
    delete simCon_;
    simCon_ = NULL;
    delete stopPointManager_;
    stopPointManager_ = NULL;
    delete tpef_;
    tpef_ = NULL;
    delete lastTraceDB_;
    lastTraceDB_ = NULL;
    delete eventHandler_;
    eventHandler_ = NULL;
    delete simCon_;
    simCon_ = NULL;
    if (memorySystem_ != NULL)
        memorySystem_->deleteSharedMemories();
    delete memorySystem_;
    memorySystem_ = NULL;

    clearProgramErrorReports();
}

/**
 * Loads a new program to be simulated.
 *
 * The loaded program is not owned by SimulatorFrontend and thus won't be
 * deleted by it when not needed anymore. Simulation is initialized using
 * the previously loaded machine. This function assumes that machine is 
 * loaded before calling it. Program is aborted otherwise.
 *
 * @param program The program to be loaded.
 * @exception IOException If tracing is enabled and could not be initialized.
 * @exception Exception Any exception thrown while building the simulation
 * models are passed on.
 * @todo Throw when machine is not loaded.
 * @todo Implement checking for already running simulation.
 */
void 
SimulatorFrontend::loadProgram(const Program& program) {

    if (currentMachine_ == NULL)
        throw Exception(
            __FILE__, __LINE__, __func__,
            "Cannot load a program without loading a machine first.");

    if (programOwnedByFrontend_) {
        delete currentProgram_;
        currentProgram_ = NULL;
    }
    currentProgram_ = &program;
    programOwnedByFrontend_ = false;

    initializeSimulation();
    initializeDataMemories();
    initializeTracing();
}

/**
 * Loads a new machine to be simulated.
 *
 * The loaded machine is not owned by SimulatorFrontend and thus won't be
 * deleted by it when not needed anymore. 
 *
 * @param program The machine to be loaded.
 */
void 
SimulatorFrontend::loadMachine(const Machine& machine)
    throw (SimulationStillRunning, IllegalProgram) {

    if (currentMachine_ != NULL && machineOwnedByFrontend_) {
        delete currentMachine_;
        currentMachine_ = NULL;
    }
    currentMachine_ = &machine;
    machineOwnedByFrontend_ = false;

    finishSimulation();
    if (programOwnedByFrontend_) {
        delete currentProgram_;
        currentProgram_ = NULL;
    }
    delete simCon_;
    simCon_ = NULL;

    delete memorySystem_;
    memorySystem_ = NULL;

    // compiled sim does not handle long guard latencies correctly.
    // remove when fixed.
    if (isCompiledSimulation() && 
        machine.controlUnit()->globalGuardLatency() > 1) {
        setCompiledSimulation(false);
        // TODO: warn about this, when the warning can be ignored
        // by tests.
    }
    initializeMemorySystem();
}

/**
 * Returns a read-only reference to the currently loaded machine.
 *
 * @return A read-only reference to the currently loaded machine, if none,
 *         returns a NullMachine instance.
 */
const Machine&
SimulatorFrontend::machine() const {
    if (currentMachine_ == NULL) 
        return NullMachine::instance();
    return *currentMachine_;
}

/**
 * Returns a read-only reference to the currently loaded program.
 *
 * @return A read-only reference to the currently loaded program, if none,
 *         returns a NullProgram instance.
 */
const Program&
SimulatorFrontend::program() const {
    if (currentProgram_ == NULL) 
        return NullProgram::instance();
    return *currentProgram_;
}

/**
 * Loads a new program to be simulated from a TPEF file.
 *
 * The loaded program will be owned by SimulatorFrontend and thus be
 * deleted by it when not needed anymore. 
 *
 * @param fileName The name of the TPEF file to be loaded.
 * @exception FileNotFound If the file cannot be found.
 * @exception SimulationStillRunning If an old simulation is still running.
 * @exception IllegalProgram If the TPEF was erroneus or the program invalid.
 * @exception Exception Any exception thrown while building the simulation
 * models are passed on.
 * @todo Implement after Program builder is done.
 * @todo Throw when machine is not loaded.
 * @todo Implement checking for already running simulation.
 */
void 
SimulatorFrontend::loadProgram(const std::string& fileName) {

    if (currentMachine_ == NULL)
        throw Exception(
            __FILE__, __LINE__, __func__,
            "Cannot load a program without loading a machine first.");

    SimulatorTextGenerator& textGen = SimulatorToolbox::textGenerator();
    
    if (!FileSystem::fileExists(fileName)) {
        throw FileNotFound(
            __FILE__, __LINE__, __func__, 
            textGen.text(Texts::TXT_FILE_NOT_FOUND).str());
    }

    if (!FileSystem::fileIsReadable(fileName)) {
        throw IOException(__FILE__, __LINE__, __func__, "File not readable.");
    }

    BinaryStream binaryStream(fileName);

    const Program* oldProgram = currentProgram_;

    try {
        // read to TPEF Handler Module
        tpef_ = BinaryReader::readBinary(binaryStream);

        assert(tpef_ != NULL);
        assert(currentMachine_ != NULL);

        // convert the loaded TPEF to POM
        TPEFProgramFactory factory(*tpef_, *currentMachine_);
        currentProgram_ = factory.build();       
    } catch (const Exception& e) {
        delete tpef_;
        tpef_ = NULL;
        delete simCon_;
        simCon_ = NULL;
        std::string errorMsg = textGen.text(
            Texts::TXT_UNABLE_TO_LOAD_PROGRAM).str();

        if (e.errorMessage() != "")
            errorMsg += " " + e.errorMessage();
            
        IllegalProgram illegp(__FILE__, __LINE__, __func__, errorMsg);
        illegp.setCause(e);
        throw illegp;
    }

    // Validate the program against the current machine using POMValidator.
    // TODO: this should be refactored -- the loadProgram(Program) is not
    // checking these?
    POMValidator validator(*currentProgram_);
    std::set<POMValidator::ErrorCode> checks;
    checks.insert(POMValidator::CONNECTION_MISSING);
    checks.insert(POMValidator::LONG_IMMEDIATE_NOT_SUPPORTED);
    checks.insert(POMValidator::SIMULATION_NOT_POSSIBLE);

    if (isCompiledSimulation()) {
        checks.insert(POMValidator::COMPILED_SIMULATION_NOT_POSSIBLE);
    }

#if (!defined(HAVE_CXX11) && !defined(HAVE_CXX0X))
    std::auto_ptr<POMValidatorResults> results(validator.validate(checks));
#else
    std::unique_ptr<POMValidatorResults> results(validator.validate(checks));
#endif

    if (results->errorCount() > 0) {
        std::string errorMsg = textGen.text(
            Texts::TXT_UNABLE_TO_LOAD_PROGRAM).str();
        
        for (int i = 0; i < results->errorCount(); i++) {
            errorMsg += "\n" + results->error(i).second;
        }
        
        if (isCompiledSimulation()) {
            // Attempt without compiled simulator
            setCompiledSimulation(false);
            checks.erase(POMValidator::COMPILED_SIMULATION_NOT_POSSIBLE);
            results.reset(validator.validate(checks));
        }
              
        if (results->errorCount() > 0) {
            delete tpef_;
            tpef_ = NULL;
            delete simCon_;
            simCon_ = NULL;
            
            for (int i = 0; i < results->errorCount(); i++) {
                errorMsg += "\n" + results->error(i).second;
            }

            throw IllegalProgram(__FILE__, __LINE__, __func__, errorMsg);   
        } else {
            outputStream()
                << errorMsg << std::endl
                << "Warning! Reverting to interpretive simulation engine." 
                << std::endl;
        }
    }

    if (programOwnedByFrontend_) {
        delete oldProgram;
        oldProgram = NULL;
    }

    delete disassembler_;
    disassembler_ = NULL;

    programOwnedByFrontend_ = true;

    initializeSimulation();
    initializeDataMemories();

    // Dump simulation traces in the same directory as loaded program file
    // or user-defined directory 
    std::string traceDir = Environment::simTraceDirPath();
    if (traceDir == "") {
        programFileName_ = fileName;
    } else {
        programFileName_ =
            traceDir + FileSystem::DIRECTORY_SEPARATOR +
            FileSystem::fileOfPath(fileName);
    }

    // tracing can't be enabled before loading program so try to initialize
    // the tracing after program is loaded
    initializeTracing();
}

/**
 * Resets and writes initial data to the memory system stored in simulation 
 * controller from loaded TPEF.
 *
 * @param onlyOne initialize the data memory of the given address space only.
 *                If onlyOne is NULL, tries to intialize all data memories.
 */
void
SimulatorFrontend::initializeDataMemories(
    const TTAMachine::AddressSpace* onlyOne) {

    // we need tpef to get the initialization data and simcon to fetch
    // the memory system from
    if (currentProgram_ == NULL || simCon_ == NULL)
        return;

    simCon_->memorySystem().resetAllMemories();
    if (zeroFillMemoriesOnReset_)
        simCon_->memorySystem().fillAllMemoriesWithZero();

    const int dataSections = currentProgram_->dataMemoryCount();

    // data memory initialization
    if (dataSections > 0) {
        for (int i = 0; i < dataSections; ++i) {

            // initialize the data memory
            const DataMemory& data = currentProgram_->dataMemory(i);
            const std::string addressSpaceName = 
                data.addressSpace().name();

            try {
                MemorySystem::MemoryPtr dataMemory =
                    simCon_->memorySystem().memory(addressSpaceName);

                const AddressSpace& addressSpace =
                    simCon_->memorySystem().addressSpace(
                        addressSpaceName);

                if (onlyOne != NULL && &addressSpace != onlyOne) continue;

                for (int d = 0; d < data.dataDefinitionCount(); ++d) {
                    const DataDefinition& def = data.dataDefinition(d);
                    if (!def.isInitialized()) {
                        continue;
                    }

                    Address startAddress = def.startAddress();
                    // Check that the defined data is inside the address
                    // space.
                    if (def.startAddress().space().name() !=
                        addressSpaceName ||
                        startAddress.location() < addressSpace.start() ||
                        (startAddress.location() + def.size() - 1)
                        > addressSpace.end()) {

                        throw IllegalProgram(
                            __FILE__, __LINE__, __func__,
                            std::string("Initialization data for ") +
                            addressSpace.name() +
                            " is out of address space bounds.");
                    } 
                    for (int m = 0; m < def.size(); m++) {
                        dataMemory->write(
                            startAddress.location() + m, def.MAU(m));
                    }
                }

            } catch (const InstanceNotFound& inf) {
                std::string errorMsg = 
                    SimulatorToolbox::textGenerator().text(
                        Texts::TXT_ILLEGAL_INPUT_FILE).str();
                if (inf.errorMessage() != "")
                    errorMsg += " Reason: " + inf.errorMessage();
                delete tpef_;
                tpef_ = NULL;
                delete simCon_;
                simCon_ = NULL;
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, errorMsg);
            }
        }
    }
 
}

/**
 * Loads a new machine to be simulated from an ADF file.
 *
 * The loaded machine will be owned by SimulatorFrontend and will be
 * deleted by it when not needed anymore. 
 *
 * @param fileName The name of the ADF file to be loaded.
 * @exception FileNotFound If the file cannot be found.
 * @exception SimulationStillRunning If an old simulation is still running.
 * @exception IllegalMachine If the ADF was erroneus.
 *
 */
void 
SimulatorFrontend::loadMachine(const std::string& fileName)
    throw (FileNotFound, IOException, SimulationStillRunning, IllegalMachine) {

    SimulatorTextGenerator& textGen = SimulatorToolbox::textGenerator();

    if (!FileSystem::fileExists(fileName)) {
        throw FileNotFound(
            __FILE__, __LINE__, __func__, 
            (textGen.text(Texts::TXT_FILE_X_NOT_FOUND) % fileName).str());
    }

    if (!FileSystem::fileIsReadable(fileName)) {
        throw IOException(__FILE__, __LINE__, __func__, "File not readable.");
    }

    ADFSerializer serializer;
    serializer.setSourceFile(fileName);

    const Machine* oldMachine = currentMachine_;
    try {
        currentMachine_ = serializer.readMachine();	
    } catch (const Exception& e) {
        IllegalMachine ime(
            __FILE__, __LINE__, __func__, 
            textGen.text(Texts::TXT_ILLEGAL_ADF_FILE).str());
        ime.setCause(e);
        throw ime;
    }
    if (machineOwnedByFrontend_) {
        delete oldMachine;
        oldMachine = NULL;
    }
    machineOwnedByFrontend_ = true;

    finishSimulation();
    if (programOwnedByFrontend_) {
        delete currentProgram_;
        currentProgram_ = NULL;
    }
    delete simCon_;
    simCon_ = NULL;

    delete memorySystem_;
    memorySystem_ = NULL;

    // compiled sim does not handle long guard latencies correctly.
    // remove when fixed.
    if (isCompiledSimulation() && 
        currentMachine_->controlUnit()->globalGuardLatency() > 1) {
        setCompiledSimulation(false);
        // TODO: warn about this, when the warning can be ignored
        // by tests.
    }
    initializeMemorySystem();
}

/**
 * Loads a new machine to be simulated from a processor configuration file.
 *
 * The loaded machine will be owned by SimulatorFrontend and thus be
 * deleted by it when not needed anymore. 
 *
 * @param fileName The name of the PCF to be loaded.
 * @exception FileNotFound If the file cannot be found.
 * @exception SimulationStillRunning If an old simulation is still running.
 * @exception IllegalMachine If the ADF was erroneus.
 *
 */
void 
SimulatorFrontend::loadProcessorConfiguration(const std::string& fileName)
    throw (FileNotFound, IOException, SimulationStillRunning, IllegalMachine) {

    SimulatorTextGenerator& textGen = SimulatorToolbox::textGenerator();

    if (!FileSystem::fileExists(fileName)) {
        throw FileNotFound(
            __FILE__, __LINE__, __func__, 
            textGen.text(Texts::TXT_FILE_NOT_FOUND).str());
    }

    if (!FileSystem::fileIsReadable(fileName)) {
        throw IOException(__FILE__, __LINE__, __func__, "File not readable.");
    }

    std::ifstream inputFile(fileName.c_str());
    ProcessorConfigurationFile pcf(inputFile);
    pcf.setPCFDirectory(FileSystem::directoryOfPath(fileName));

    std::string adfName = "";
    try {
        adfName = pcf.architectureName();
    } catch (const KeyNotFound&) {
        throw FileNotFound(
            __FILE__, __LINE__, __func__, 
            textGen.text(Texts::TXT_NO_ADF_DEFINED_IN_PCF).str());
    }
    
    loadMachine(adfName);
}


/* Because memory models are initialized before the controller,
 * and RemoteMemory accesses its physical memory via the controller, 
 * RemoteMemories must be further initialized here.
 */
void 
SimulatorFrontend::setControllerForMemories(RemoteController* con) {

    int num_mems = memorySystem_->memoryCount();    
    for (int i = 0; i < num_mems; i++) {
        MemorySystem::MemoryPtr memptr = memorySystem_->memory(i);
        boost::shared_ptr<RemoteMemory> rmem = 
            boost::static_pointer_cast<RemoteMemory>(memptr);
        assert(rmem != NULL && "not a RemoteMemory!");
        rmem->setController(con);    
    }
}

/**
 * Initializes a new simulation.
 *
 * Creates the SimulationController and initializes the program counter to
 * point to the first executed instruction. Assumes the simulation is not
 * running and the possible old simulation data is freed.
 *
 * @exception Exception There can be several types of exceptions thrown
 * when building the simulation machine model or the preprocessed program.
 */
void 
SimulatorFrontend::initializeSimulation() {

    delete simCon_;
    simCon_ = NULL;
    switch(currentBackend_) {
        case SIM_REMOTE:    
            simCon_ = 
                new TCEDBGController( 
                    *this, *currentMachine_, *currentProgram_);
            setControllerForMemories(dynamic_cast<RemoteController*>(simCon_));
            break;
        case SIM_CUSTOM:
            simCon_ = 
                new CustomDBGController(
                    *this, *currentMachine_, *currentProgram_);
            setControllerForMemories(dynamic_cast<RemoteController*>(simCon_));
            break;
        case SIM_COMPILED:
            simCon_ = 
                new CompiledSimController(
                    *this, *currentMachine_, *currentProgram_, 
                    leaveCompiledDirty_);
            break;
        case SIM_NORMAL:
        default:
            simCon_ = 
             new SimulationController(
                    *this, *currentMachine_, *currentProgram_, 
                    fuResourceConflictDetection_, detailedSimulation_);
            machineState_ = 
                &(dynamic_cast<SimulationController*>(simCon_)->machineState());

    }
        
    delete stopPointManager_;
    stopPointManager_ = new StopPointManager(*simCon_, eventHandler());
}

/**
 * Finds the state connected to the boolean register.
 *
 * Currently boolean register is expected to be the first register in
 * a register file with only one register of width 1.
 *
 * @todo Improve evaluation when the correct way is known.
 *
 * @return State connected to the bool register.
 * @exception InstanceNotFound If bool register cannot be found.
 */
StateData&
SimulatorFrontend::findBooleanRegister() 
    throw (InstanceNotFound) {

    assert(currentMachine_ != NULL);

    Machine::RegisterFileNavigator navigator = 
        currentMachine_->registerFileNavigator();
    
    // go through all the register files in the machine
    for (int i = 0; i < navigator.count(); ++i) {
        RegisterFile* rf = navigator.item(i);
        if (rf->width() == 1 && rf->numberOfRegisters() == 1) {
            RegisterFileState& rfState = 
                machineState_->registerFileState(rf->name());
            return rfState.registerState(0);
        }
    }

    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, "No boolean register found.");
}

/**
 * Returns a string containing the value(s) of the register file
 * 
 * @param rfName name of the register file to search for
 * @param registerIndex index of the register. if -1, all registers are listed
 * @return A string containing the value(s) of the register file
 * @exception InstanceNotFound If the register cannot be found.
 */
std::string 
SimulatorFrontend::registerFileValue(
    const std::string& rfName, int registerIndex) {
    
    return simCon_->registerFileValue(rfName, registerIndex);
}

/**
 * Returns the current value of a IU register
 * 
 * @param iuName name of the immediate unit
 * @param index index of the register
 * @return Current value of a IU register
 */
SimValue 
SimulatorFrontend::immediateUnitRegisterValue(
    const std::string& iuName, int index) {
    assert(currentMachine_ != NULL);

    return simCon_->immediateUnitRegisterValue(iuName, index);
}

/**
 * Returns the current value of a FU port
 * 
 * @param fuName name of the function unit
 * @param portName name of the FU port
 * @return Current value of a FU port
 */
SimValue
SimulatorFrontend::FUPortValue(
    const std::string& fuName, const std::string& portName) {
    assert(currentMachine_ != NULL);

    return simCon_->FUPortValue(fuName, portName);
}

/**
 * Finds the state connected to a register.
 *
 * @todo Improve evaluation when the parallel assembler syntax is known.
 *
 * @param rfName The name of the register file.
 * @param registerIndex Index of the register in the register file.
 * @return State connected to the register.
 * @exception InstanceNotFound If the register cannot be found.
 */
StateData&
SimulatorFrontend::findRegister(
    const std::string& rfName, int registerIndex) 
    throw (InstanceNotFound) {

    assert(currentMachine_ != NULL);

    Machine::RegisterFileNavigator navigator = 
        currentMachine_->registerFileNavigator();

    std::string regFileName = rfName;
    if (!navigator.hasItem(rfName)) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, 
            "Register file " + rfName + " not found.");
    }
    try {
        return machineState_->registerFileState(
            regFileName).registerState(registerIndex);
    } catch (const OutOfRange&) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, 
            "Register index out of range");
    }
}

/**
 * Finds the port using a search string.
 *
 * The search string supported currently is of format 
 * {function unit name].{port name}.
 *
 * @param fuName The name of the operation or the function unit.
 * @param portName The name of the operand or the port.
 * @return State connected to the register.
 * @exception InstanceNotFound If the register cannot be found.
 */
StateData&
SimulatorFrontend::findPort(
    const std::string& fuName, const std::string& portName)
    throw (InstanceNotFound) {

    assert(currentMachine_ != NULL);

    const std::string exceptionMessage =
        std::string("No port ") + fuName + "." + portName + " found.";

    // first try to fetch the port from GCU
    PortState& foundState = machineState_->portState(
        portName, currentMachine_->controlUnit()->name());
    
    if (&foundState != &NullPortState::instance()) {
        return foundState;
    }

    /// @todo Get the list of control operations from GCU instead of
    /// hard coding like this
    if (StringTools::ciEqual(fuName, "call") ||
        StringTools::ciEqual(fuName, "jump")) {

        // we'll try converting the port name to an operand number
        int operandNumber = -1;
        try {
            operandNumber = Conversion::toInt(portName);
        } catch (const NumberFormatException&) {
            // portName was not a number, we cannot find it in the
            // GCU for sure
            throw InstanceNotFound(
                __FILE__, __LINE__, __func__, exceptionMessage);
        }

        // it's a control flow operand, we should get the port from GCU
        return machineState_->portState(
            currentMachine_->controlUnit()->operation(fuName)->port(
                operandNumber)->name(), 
            currentMachine_->controlUnit()->name());
    }

    return machineState_->portState(
        StringTools::stringToLower(portName), 
        StringTools::stringToLower(fuName));
    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, exceptionMessage);
}


/**
 * Tries to locate a readable machine state part using a search string.
 *
 * For a syntax of valid search strings, consult the TCE TTA assembler
 * syntax description. All valid sources and destinations except
 * immediate sources are valid search strings. 
 *
 * @param searchString The string to use in search, e.g., (alu.p3).
 * @return Current value of the state (read-only).
 * @exception InstanceNotFound In case state couldn't be found using the
 *            search string.
 */
const SimValue& 
SimulatorFrontend::stateValue(std::string searchString) 
    throw (InstanceNotFound) {
    return state(searchString).value();
}

/**
 * Tries to locate a readable machine state part using a search string.
 *
 * See stateValue() for further comment.
 *
 * @param searchString The string to use in search, e.g., (alu.p3 or add.3).
 * @return The StateData object.
 * @exception InstanceNotFound In case state couldn't be found using the
 *            search string.
 *
 */
StateData&
SimulatorFrontend::state(std::string searchString) 
    throw (InstanceNotFound) {

    if (machineState_ == NULL || currentMachine_ == NULL) 
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, "State not found.");

    boost::smatch parsed;
	
    if (StringTools::ciEqual(searchString, "bool") ||
        StringTools::ciEqual(searchString, "boolean")) {
        return findBooleanRegister();
    } else 
        if (StringTools::ciEqual(searchString, "ra") ||
            StringTools::ciEqual(searchString, "return-address")) {
            return findPort(
                currentMachine_->controlUnit()->name(),
                currentMachine_->controlUnit()->returnAddressPort()->name());
        } else 
            if (regex_match(
                    searchString, parsed, 
                    SimulatorToolbox::fuPortRegex())) {
                /// @todo Parallel fu port access (syntax?)
                return findPort(parsed[1], parsed[2]);
            }
    
    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, 
        std::string("No state found with the search string '") +
        std::string(searchString) + "'.");
}

/**
 * Starts the wall-clock timer.
 */
void
SimulatorFrontend::startTimer() {
    startCycleCount_ = cycleCount();
    startTime_ = time(NULL);
}

/**
 * Saves the value of the wall-clock timer initialized with startTimer() to lastRunTime_ and
 * the count of simulated cycles after startTimer() to lastRunCycleCount_.
 */
void
SimulatorFrontend::stopTimer() {
    std::time_t now = time(NULL);
    lastRunTime_ = now - startTime_;
    CycleCount cycles = cycleCount();
    lastRunCycleCount_ = cycles - startCycleCount_;
}

/**
 * A thread function for handling simulation timeout
 * 
 * @param timeout timeout in seconds
 */
void 
timeoutThread(unsigned int timeout, SimulatorFrontend* simFE) {
    if (timeout == 0) {
        return;
    }
      
    TTASimulationController* simCon = simFE->simCon_;
    boost::xtime xt; 
    boost::xtime xtPoll; 
#if BOOST_VERSION < 105000
    boost::xtime_get(&xt, boost::TIME_UTC);
#else
    /* TIME_UTC was replaced by TIME_UTC_ in boost 1.50, to avoid
     * clashing with a similarly named C11 macro. */
    boost::xtime_get(&xt, boost::TIME_UTC_);
#endif
    unsigned int pollTime = 5; // poll time in seconds
    xtPoll = xt;
    xt.sec += timeout; 

    xtPoll.sec += pollTime;
    while (xt.sec > xtPoll.sec) {
        boost::thread::sleep(xtPoll);
        xtPoll.sec += pollTime;
        if (simCon != simFE->simCon_) {
            return;
        }
    }
    boost::thread::sleep(xt);

    if (simCon != simFE->simCon_) {
        return;
    }

    if (!simFE->hasSimulationEnded()) {
        simFE->prepareToStop(SRE_AFTER_TIMEOUT);
    }
}

/**
 * Run simulation until it's stopped for some reason.
 *
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 * @todo Throw exception if simulation is not initialized.
 */
void
SimulatorFrontend::run() 
    throw (SimulationExecutionError) {
 
    startTimer();
    boost::thread timeout(
        boost::bind(timeoutThread, simulationTimeout_, this));
    simCon_->run();
    stopTimer();
    // invalidate utilization statistics (they are not fresh anymore)
    delete utilizationStats_;
    utilizationStats_ = NULL;
}

/**
 * Run simulation until given address.
 *
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 * @todo Throw exception if simulation is not initialized.
 */
void 
SimulatorFrontend::runUntil(UIntWord address) 
    throw (SimulationExecutionError) {

    startTimer();
    boost::thread timeout(boost::bind(timeoutThread, 
        simulationTimeout_, this));
    simCon_->runUntil(address);
    stopTimer();
    // invalidate utilization statistics (they are not fresh anymore)
    delete utilizationStats_;
    utilizationStats_ = NULL;
}


/**
 * Advance simulation by a given amout of cycles.
 *
 * @note Does not create a timeout thread. Does not make sense here as
 * the step() always finishes.
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 * @todo Throw exception if simulation is not initialized.
 */
void
SimulatorFrontend::step(double count) 
    throw (SimulationExecutionError) {

    assert(simCon_ != NULL);

    simCon_->step(count);
    // invalidate utilization statistics (they are not fresh anymore)
    delete utilizationStats_;
    utilizationStats_ = NULL;
}

/**
 * Advance simulation by a given amout of steps and skip procedure
 * calls.
 *
 * @param count The number of steps the simulation is advanced.
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 * @todo Throw exception if simulation is not initialized.
 */
void
SimulatorFrontend::next(int count) 
    throw (SimulationExecutionError) {

    assert(simCon_ != NULL);

    startTimer();
    boost::thread timeout(boost::bind(timeoutThread, 
        simulationTimeout_, this));
    simCon_->next(count);
    stopTimer();
    
    // invalidate utilization statistics (they are not fresh anymore)
    delete utilizationStats_;
    utilizationStats_ = NULL;
}

/**
 * Returns the disassembly of instruction at given address.
 *
 * @param instructionAddress Address (index) of the instruction wanted to
 *                           disassemble.
 * @return Disassembly of an instruction.
 */
std::string 
SimulatorFrontend::disassembleInstruction(UIntWord instructionAddress) const {

    const Instruction& theInstruction =
        currentProgram_->instructionAt(instructionAddress);
    const Procedure& currentProc = dynamic_cast<const Procedure&>(
        theInstruction.parent());

    bool firstInstructionInProcedure = 
        (currentProc.startAddress().location() == instructionAddress);

    std::string disassembly = "";

    /// @todo print all labels associated at address

    if (firstInstructionInProcedure) {
        disassembly += "\n" + currentProc.name() + ":\n";
    }

    disassembly +=
        Conversion::toString(instructionAddress) + ":\t\t" + 
        POMDisassembler::disassemble(theInstruction, false);
    return disassembly;
}

/**
 * Returns a string describing the current location of simulation in the
 * simulated program.
 *
 * If simulation has ended, returns an empty string.
 *
 * @return A description as defined in Simulator specs.
 */
std::string 
SimulatorFrontend::programLocationDescription() const {

    if (hasSimulationEnded()) {
        return "";
    }

    InstructionAddress instructionAddress = programCounter();

    const InstructionAddress programLastAddress = 
        currentProgram_->lastProcedure().endAddress().location() - 1;

    if (instructionAddress > programLastAddress) {
        std::stringstream tempStream;
        return tempStream.str();
    }

    const Instruction& theInstruction =
        currentProgram_->instructionAt(instructionAddress);
    const Procedure& currentProc = 
        dynamic_cast<const Procedure&>(theInstruction.parent());

    InstructionAddress distanceFromStart = 
        instructionAddress - currentProc.startAddress().location();

    initializeDisassembler();

    DisassemblyInstruction* disassembledInstruction = 
        disassembler_->createInstruction(instructionAddress);

    std::stringstream tempStream;

    tempStream << std::setw(7) << std::right << instructionAddress << " ";
    tempStream 
        << std::setw(30) << std::right << 
        std::string("<") + currentProc.name() + "+" + 
        Conversion::toString(distanceFromStart) + ">" << ": "
        << disassembledInstruction->toString();

    delete disassembledInstruction;

    return tempStream.str();
}

/**
 * Initializes the disassembler.
 *
 * Creates a new POMDisassembler for the loaded program if it was not
 * loaded already.
 */
void
SimulatorFrontend::initializeDisassembler() const {

    if (disassembler_ != NULL || currentProgram_ == NULL) {
        // already initialized or no program to disassemble
        return;
    }
    disassembler_ = new POMDisassembler(*currentProgram_);
}

/**
 * Returns the program counter value.
 *
 * @return Program counter value.
 * @todo Throw exception if simulation is not initialized
 */
InstructionAddress
SimulatorFrontend::programCounter() const {
    assert(simCon_ != NULL);
    return simCon_->programCounter();
}

/**
 * Returns the address of the last executed instruction.
 *
 * @return Address of the last executed instruction.
 */
InstructionAddress
SimulatorFrontend::lastExecutedInstruction() const {
    assert(simCon_ != NULL);
    return simCon_->lastExecutedInstruction();
}

/**
 * Returns the current count of simulated cycles.
 *
 * @return The count of cycles.
 * @todo Throw exception if simulation is not initialized
 */
ClockCycleCount
SimulatorFrontend::cycleCount() const {
    assert(simCon_ != NULL);
    return simCon_->clockCount();
}


/**
 * Returns the current procedure.
 *
 * The current procedure is expected to be the nearest procedure behind
 * the program counter.
 *
 * @return The current procedure.
 * @todo Throw exception if simulation is not initialized
 */
const Procedure&
SimulatorFrontend::currentProcedure() const {
    assert(simCon_ != NULL);
    assert(currentProgram_ != NULL);  
    InstructionAddress address = programCounter();
    if (address > currentProgram_->lastInstruction().address().location()) {
        return currentProgram_->lastProcedure();
    } else {
        return dynamic_cast<const Procedure&>(
            currentProgram_->instructionAt(programCounter()).parent());
    }
}

/**
 * Returns true if the simulation initialized and ready to run.
 *
 * @return True if simulation is initialized.
 */
bool 
SimulatorFrontend::isSimulationInitialized() const {
    return (simCon_ != NULL && 
            simCon_->state() == SimulationController::STA_INITIALIZED);
}

/**
 * Returns true if the simulation has been initialized with a
 * program successfully.
 *
 * @return True if simulation has been initialized.
 */
bool 
SimulatorFrontend::isProgramLoaded() const {
    return (simCon_ != NULL && currentProgram_ != NULL);
}

/**
 * Returns true if a machine has been loaded for the simulation
 *
 * @return True if machine has been loaded.
 */
bool 
SimulatorFrontend::isMachineLoaded() const {
    return (currentMachine_ != NULL);
}

/**
 * Returns true if the simulation is running.
 *
 * @return True if simulation running.
 */
bool 
SimulatorFrontend::isSimulationRunning() const {
    return (simCon_ != NULL &&
            simCon_->state() == SimulationController::STA_RUNNING);
}

/**
 * Returns true if the simulation is stopped.
 *
 * @return True if simulation stopped.
 */
bool 
SimulatorFrontend::isSimulationStopped() const {
    return (simCon_ != NULL &&
            simCon_->state() == SimulationController::STA_STOPPED);
}

/**
 * Returns true if the simulation has ended, that is, the last instruction
 * have been executed.
 *
 * @return True if simulation has ended.
 */
bool 
SimulatorFrontend::hasSimulationEnded() const {
    return (simCon_ != NULL &&
            simCon_->state() == SimulationController::STA_FINISHED);
}


/**
 * Returns true if the current simulation engine uses compiled simulation
 * 
 * @return true if the current simulation engine uses compiled simulation
 */
bool 
SimulatorFrontend::isCompiledSimulation() const {
    return currentBackend_ == SIM_COMPILED;
}

/**
 * Check if we are currently using a TCE built-in debugger. This returns true if 
 * we are attached to a FPGA or an ASIC with on-circuit debug hardware.
 * 
 * @return true if we are using a TCE debug target.
 */
bool 
SimulatorFrontend::isTCEDebugger() const {
    return currentBackend_ == SIM_REMOTE;
}

/**
 * Check if we are currently using a custom debugger. This returns true if 
 * we are attached to a FPGA or an ASIC with on-circuit debug hardware.
 * 
 * @return true if we are using a custom debugger target.
 */
bool 
SimulatorFrontend::isCustomDebugger() const {
    return currentBackend_ == SIM_CUSTOM;
}

/**
 * Signals the simulator engine to stop simulation after the current
 * simulated clock cycle.
 *
 * Does nothing if simulation is not initialized or simulation is not
 * running.
 *
 * @param reason The reason for stopping.
 */
void
SimulatorFrontend::prepareToStop(StopReason reason) {
    if (simCon_ == NULL || !isSimulationRunning()) {
        return;
    }
    simCon_->prepareToStop(reason);
}

/**
 * Returns the count of stop reasons.
 *
 * @return The count of stop reasons.
 */
unsigned int 
SimulatorFrontend::stopReasonCount() const {
    return (simCon_ != NULL && simCon_->stopReasonCount());
}

/**
 * Returns the stop reason with the given index.
 *
 * @param index The wanted index.
 * @return The stop reason at the given index.
 * @exception OutOfRange If the given index is out of range.
 */
StopReason 
SimulatorFrontend::stopReason(unsigned int index) const 
    throw (OutOfRange) {
    assert(simCon_ != NULL);
    return simCon_->stopReason(index);
}

/**
 * Helper function which tells whether simulation has stopped because of
 * the given reason.
 *
 * @return True if the given reason was one of the reasons the simulation
 *         was stopped.
 */
bool 
SimulatorFrontend::hasStopReason(StopReason reason) const {

    if (simCon_ == NULL) {
        return false;
    }

    for (size_t i = 0; i < stopReasonCount(); ++i) {
        if (stopReason(i) == reason) {
            return true;
        }
    }
    return false;
}

/**
 * Returns true if simulation is stopped because of user set reason, i.e.,
 * breakpoint, watchpoint, memory write watchpoint, explicit (ctrl-c) stop,
 * etc.
 *
 * Also returns true if a runtime error was the reason for stopping, to 
 * avoid infinite loops.
 *
 * @param true If simulation stopped because of user set stop point.
 */
bool 
SimulatorFrontend::stoppedByUser() const {

    if (simCon_ == NULL) {
        return false;
    }

    return 
        hasStopReason(SRE_BREAKPOINT) || hasStopReason(SRE_USER_REQUESTED) ||
        hasStopReason(SRE_RUNTIME_ERROR);
}

/**
 * Initializes the trace database(s).
 *
 * In case no traces are enabled, this method does nothing. Making
 * multiple calls to this method is not harmful.
 *
 * @exception IOException In case trace database file could not be accessed.
 */
void
SimulatorFrontend::initializeTracing() 
    throw (IOException) {

    if (executionTracing_ || rfAccessTracing_ || 
        procedureTransferTracing_ || saveProfileData_ || 
        saveUtilizationData_) {

        // initialize the data base
        if (traceDB_ == NULL) {
            if (traceFileNameSetByUser_ == false) {
                // generate the file name
                setTraceDBFileName(programFileName_ + ".trace");

                int runningNumber = 0;
                while (FileSystem::fileExists(traceFileName_)) {
                    // append ".n" where n is a running number, in case the
                    // file exists
                    ++runningNumber;
                    setTraceDBFileName(
                        programFileName_ + ".trace" +
                        Conversion::toString(runningNumber));
                }
                // set the flag to false, since the name is generated
                traceFileNameSetByUser_ = false;
            }

            /// @note May throw IOException.
            traceDB_ = ExecutionTrace::open(traceFileName_);
        }
        if (executionTracing_ && executionTracker_ == NULL) {
            assert(simCon_ != NULL);
            assert(traceDB_ != NULL);
            executionTracker_ = new ExecutionTracker(*simCon_, *traceDB_);
        }

        if (rfAccessTracing_) {
            rfAccessTracker_ = new RFAccessTracker(
                *this, dynamic_cast<SimulationController*>(simCon_)->instructionMemory());
        }
        if (procedureTransferTracing_) {
            assert(traceDB_ != NULL);
            procedureTransferTracker_ = new ProcedureTransferTracker(
                *this, *traceDB_);
        }
    }

    // For perfomance reasons bus trace is written directly to an ascii
    // output file instead of the trace database.
    if (busTracing_ && busTracker_ == NULL) {
        // generate the file name
        std::string busTraceFileName = programFileName_ + ".bustrace";
        int runningNumber = 0;
        while (FileSystem::fileExists(busTraceFileName)) {
            // append ".n" where n is a running number, in case the
            // file exists
            ++runningNumber;
                busTraceFileName = 
                    programFileName_ + ".bustrace." + 
                    Conversion::toString(runningNumber);
        } 
        
        busTraceStream_.open(busTraceFileName.c_str(), std::ios::out);
        if (!busTraceStream_) {
            std::string errorMessage =
                "Unable to open bus trace file " + busTraceFileName +
                " for writing.";
            throw IOException(__FILE__, __LINE__, __func__, errorMessage);
        }
        busTracker_ = new BusTracker(*this, busTraceStream_);
    }
}

/**
 * Finishes the currently running simulation.
 *
 * Flushes any simulation traces etc., but does not unload nor dealloacate
 * the loaded machine and the program. This is to allow restarting the
 * simulation without needing to reinitialize the machine and program.
 */ 
void 
SimulatorFrontend::finishSimulation() {

    if (simCon_ == NULL)
        return;

    delete executionTracker_;
    executionTracker_ = NULL;

    if (traceDB_ != NULL) {
        // flush the concurrent RF access trace data
        if (rfAccessTracing_) {
            assert(rfAccessTracker_ != NULL);
            const RFAccessTracker::ConcurrentRFAccessIndex& accesses =
                rfAccessTracker_->accessDataBase();
            for (RFAccessTracker::ConcurrentRFAccessIndex::const_iterator i = 
                     accesses.begin(); i != accesses.end(); ++i) {
                const RFAccessTracker::ConcurrentRFAccess& access = (*i).first;
                const std::string& rfName = access.get<0>();
                const std::size_t& reads = access.get<2>();
                const std::size_t& writes = access.get<1>();
                const ClockCycleCount& count = (*i).second;
                traceDB_->addConcurrentRegisterFileAccessCount(
                    rfName, reads, writes, count);
            }
        }

        if (saveUtilizationData_) {
            const UtilizationStats& stats = utilizationStatistics();
            // save the function unit operation execution counts
            const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
                machine().functionUnitNavigator();
            for (int i = 0; i <= fuNav.count(); ++i) {
                TTAMachine::FunctionUnit* fu = NULL;
                if (i < fuNav.count())
                    fu = fuNav.item(i);
                else
                    fu = machine().controlUnit();
                assert(fu != NULL);
                const ClockCycleCount totalTriggersOfFU = 
                    stats.triggerCount(fu->name());

                if (totalTriggersOfFU == 0)
                    continue;

                for (int j = 0; j < fu->operationCount(); ++j) {
                    const TTAMachine::HWOperation* op = fu->operation(j);
                    assert(op != NULL);
                    const std::string operationUpper = 
                        StringTools::stringToUpper(op->name());

                    const ClockCycleCount executions = 
                        stats.operationExecutions(fu->name(), operationUpper);

                    if (executions == 0) 
                        continue;

                    traceDB_->addFunctionUnitOperationTriggerCount(
                        fu->name(), operationUpper, executions);
                }
            }


            // save the socket write counts
            const TTAMachine::Machine::SocketNavigator& socketNav = 
                machine().socketNavigator();
            for (int i = 0; i < socketNav.count(); ++i) {

                TTAMachine::Socket* socket = socketNav.item(i);
                assert(socket != NULL);
                const ClockCycleCount writes = 
                    stats.socketWrites(socket->name());
                
                traceDB_->addSocketWriteCount(socket->name(), writes);
            }            
        
            // save the bus write counts
            const TTAMachine::Machine::BusNavigator& busNav = 
                machine().busNavigator();
            for (int i = 0; i < busNav.count(); ++i) {
                TTAMachine::Bus* bus = busNav.item(i);
                assert(bus != NULL);
                const ClockCycleCount writes = stats.busWrites(bus->name());
                
                traceDB_->addBusWriteCount(bus->name(), writes);
            }

            // save the register access stats
            const TTAMachine::Machine::RegisterFileNavigator& rfNav = 
                machine().registerFileNavigator();
            for (int i = 0; i < rfNav.count(); ++i) {
                TTAMachine::RegisterFile* rf = rfNav.item(i);
                assert(rf != NULL);

                int maxRegs = 0;
                maxRegs = rf->numberOfRegisters();
                for (int reg = 0; reg < maxRegs; ++reg) {
                    ClockCycleCount reads = 
                        stats.registerReads(rf->name(), reg);
                    ClockCycleCount writes = 
                        stats.registerWrites(rf->name(), reg);
                    traceDB_->addRegisterAccessCount(
                        rf->name(), reg, reads, writes);
                }
            }            

        }

        if (saveProfileData_) {
            
            assert(simCon_ != NULL);
            assert(currentProgram_ != NULL);

            // save the instruction execution counts (profile data)
            const InstructionMemory& instructions = 
                dynamic_cast<SimulationController*>(simCon_)->instructionMemory();

            InstructionAddress firstAddress = 
                InstructionAddress(currentProgram_->startAddress().location());
            InstructionAddress lastAddress = 
                InstructionAddress(
                    currentProgram_->lastInstruction().address().location());

            /// @note this expects that each instruction is 1 address long
            for (InstructionAddress a = firstAddress; a <= lastAddress; ++a) {
                traceDB_->addInstructionExecutionCount(
                    a, instructions.instructionAtConst(a).executionCount());
            }
            
        }

	if (procedureTransferTracing_ || saveProfileData_) {
            // save the start addresses of procedures in order to provide
            // possibility for more readable query outputs with traces
	    // that include procedure data in them
            for (int i = 0; i < currentProgram_->procedureCount(); ++i) {
                const Procedure& procedure = currentProgram_->procedure(i);
                traceDB_->addProcedureAddressRange(
                    procedure.startAddress().location(), 
                    procedure.endAddress().location() - 1, procedure.name());
            }
        }
	
        traceDB_->setSimulatedCycleCount(cycleCount());
        delete lastTraceDB_;
        lastTraceDB_ = traceDB_;
        traceDB_ = NULL;
    }

    busTraceStream_.close();
    delete busTracker_;
    busTracker_ = NULL;
    delete rfAccessTracker_;
    rfAccessTracker_ = NULL;
    delete procedureTransferTracker_;
    procedureTransferTracker_ = NULL;
    delete utilizationStats_;
    utilizationStats_ = NULL;

}

/**
 * Initializes the memory system according to the address spaces in the
 * loaded machine.
 */
void 
SimulatorFrontend::initializeMemorySystem() {

    assert (currentMachine_ != NULL);
    const Machine& machine = *currentMachine_;
    memorySystem_ = new MemorySystem(machine);
    // create a memory system for the loaded machine by going
    // through all address spaces in the machine and create a memory model 
    // for each of them, except for the one of GCU's
    Machine::AddressSpaceNavigator nav = machine.addressSpaceNavigator();

    
    std::string controlUnitASName = "";
    if (machine.controlUnit() != NULL &&
        machine.controlUnit()->hasAddressSpace()) {
        controlUnitASName = machine.controlUnit()->addressSpace()->name();
    }

    for (int i = 0; i < nav.count(); ++i) {
        const AddressSpace& space = *nav.item(i);

        if (space.name() == controlUnitASName)
            continue;

        /// if shared, assume the external initializer initializes the shared 
        /// memory model to the rest of the MemorySystems before starting 
        /// the simulation
        const bool shared = space.isShared();

        MemorySystem::MemoryPtr mem;
        switch (currentBackend_) {
        case SIM_COMPILED:
             mem = MemorySystem::MemoryPtr(
                 new DirectAccessMemory(
                    space.start(), space.end(), space.width()));
             break;
        case SIM_NORMAL:
             mem = MemorySystem::MemoryPtr(
                 new IdealSRAM(
                    space.start(), space.end(), space.width()));
             break;
        case SIM_REMOTE:
        case SIM_CUSTOM:
             mem = MemorySystem::MemoryPtr(
                 new RemoteMemory( space ));

            break;
        default:            
        throw Exception(
            __FILE__, __LINE__, __func__,
            "Internal error: memory model not specified");
        }

        // If memory tracking is enabled, memories are wrapped by a proxy
        // that tracks memory access.
        if (memoryAccessTracking_) {
            mem = MemorySystem::MemoryPtr(
                new MemoryProxy(*this, mem.get()));
        }
        memorySystem_->addAddressSpace(space, mem, shared);
    }
}


/** 
 * Kills the currently running simulation.
 *
 * Allows restarting the simulation with the loaded machine and program. 
 * Flushes data collected during simulation to the trace file, if tracing is
 * enabled, and reinitializes everyting that needs to be reinitialized,
 * such as the data memory initial values.
 */
void
SimulatorFrontend::killSimulation() {
    finishSimulation();
    simCon_->reset();
    initializeTracing();
    initializeDataMemories();
    lastRunCycleCount_ = 0;
}

/**
 * Returns true in case execution tracing is enabled.
 *
 * @return True in case execution tracing is enabled.
 */
bool
SimulatorFrontend::executionTracing() const {
    return executionTracing_;
}

/**
 * Returns true in case bus tracing is enabled.
 *
 * @return True in case bus tracing is enabled.
 */
bool
SimulatorFrontend::busTracing() const {
    return busTracing_;
}

/**
 * Returns true in case register file access tracing is enabled.
 *
 * @return True in case RF access tracing is enabled.
 */
bool
SimulatorFrontend::rfAccessTracing() const {
    return rfAccessTracing_;
}
/**
 * Returns true in case procedure transfer tracing is enabled.
 *
 * @return True in case procedure transfer tracing is enabled.
 */
bool
SimulatorFrontend::procedureTransferTracing() const {
    return procedureTransferTracing_;
}

/**
 * Returns true in case profile data saving is enabled.
 *
 * @return True in case profile data saving is enabled.
 */
bool
SimulatorFrontend::profileDataSaving() const {
    return saveProfileData_;
}

/**
 * Returns true in case utilization data saving is enabled.
 *
 * @return True in case utilization data saving is enabled.
 */
bool
SimulatorFrontend::utilizationDataSaving() const {
    return saveUtilizationData_;
}

/**
 * Returns true if the compiled simulation uses static compilation
 * 
 * @return true if the compiled simulation uses static compilation
 */
bool
SimulatorFrontend::staticCompilation() const {
    return staticCompilation_;
}


/**
 * Returns the register file access tracker.
 *
 * @return The register file access tracker.
 * @exception InstanceNotFound If RF access tracking is not enabled.
 */
const RFAccessTracker& 
SimulatorFrontend::rfAccessTracker() const
    throw (InstanceNotFound) {
    if (rfAccessTracker_ == NULL) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, "RF access tracing is disabled.");
    }
    return *rfAccessTracker_;
}

/**
 * Sets compiled simulation on or off
 *
 * @param value Is compiled simulation enabled or not.
 */
void
SimulatorFrontend::setCompiledSimulation(bool value) {
    // This legacy function assumes we use a simulator engine, not a
    // remote target.
    if (currentBackend_ == SIM_REMOTE) return;
    if (currentBackend_ == SIM_CUSTOM) return;
    if (value)
        currentBackend_ = SIM_COMPILED;
    else 
        currentBackend_ = SIM_NORMAL;
}

/**
 * Sets the execution tracing on or off.
 *
 * @param value Is execution tracing enabled or not.
 */
void
SimulatorFrontend::setExecutionTracing(bool value) {
    executionTracing_ = value;
}

/**
 * Sets the bus tracing on or off.
 *
 * @param value Is bus tracing enabled or not.
 */
void
SimulatorFrontend::setBusTracing(bool value) {
    busTracing_ = value;
}

/**
 * Sets the register file access tracing on or off.
 *
 * @param value Is register file access tracing enabled or not.
 */
void
SimulatorFrontend::setRFAccessTracing(bool value) {
    rfAccessTracing_ = value;
}

/**
 * Sets the procedure transfer tracing on or off.
 *
 * @param value Is procedure transfer tracing on or off.
 */
void
SimulatorFrontend::setProcedureTransferTracing(bool value) {
    procedureTransferTracing_ = value;
}

/**
 * Sets the profile saving on or off.
 *
 * @param value Is profile saving on or off.
 */
void
SimulatorFrontend::setProfileDataSaving(bool value) {
    saveProfileData_ = value;
}

/**
 * Sets the file name of the TraceDB.
 *
 * @param fileName The file name to set.
 */
void
SimulatorFrontend::setTraceDBFileName(const std::string& fileName) {
    traceFileName_ = fileName;
    traceFileNameSetByUser_ = true;
}

/**
 * Sets the simulation timeout in seconds. Use zero for no timeout.
 * 
 * @param value Simulation timeout in seconds.
 */
void 
SimulatorFrontend::setTimeout(unsigned int value) {
    simulationTimeout_ = value;
}

/**
 * Sets the compiled simulator to use static or dynamic compilation.
 * 
 * Has no effect when running interpretive simulation.
 * 
 * @param value new value to be set
 */
void
SimulatorFrontend::setStaticCompilation(bool value) {
    staticCompilation_ = value;
}

/**
 * Returns the output stream
 * 
 * @return the output stream
 */
std::ostream&
SimulatorFrontend::outputStream() {
    assert(outputStream_ != 0);
    return *outputStream_;
}

/**
 * Sets the default output stream
 * 
 * @param stream the output stream
 */
void 
SimulatorFrontend::setOutputStream(std::ostream& stream) {
    outputStream_ = &stream;
}

/**
 * Sets the utilization data saving on or off.
 *
 * @param value Is utilization data saving on or off.
 */
void
SimulatorFrontend::setUtilizationDataSaving(bool value) {
    saveUtilizationData_ = value;
}

/**
 * Sets the FU resource conflict detection on or off.
 *
 * NOTE: this affects future simulations only. That is, if a simulation is
 * already initialized (program loaded), the old setting is used for that
 * simulation.
 *
 * @param value Should the resource conflict detection be used.
 */
void
SimulatorFrontend::setFUResourceConflictDetection(bool value) {
    fuResourceConflictDetection_ = value;
}

/**
 * Returns true if FU resource conflict detection is on, false if not.
 *
 * @return Returns current fu conflict detection setting.
 */
bool
SimulatorFrontend::fuResourceConflictDetection() const  {
    return fuResourceConflictDetection_;
}


/**
 * Sets the memory access tracking on or off.
 *
 * NOTE: this affects future simulations only. That is, if a simulation is
 * already initialized (program loaded), the old setting is used for that
 * simulation.
 *
 * @param value Should the memory access tracking be used.
 */
void
SimulatorFrontend::setMemoryAccessTracking(bool value) {
    memoryAccessTracking_ = value;
}

/**
 * Returns true if memory access tracking is enabled.
 *
 * @return Returns current memory access tracking setting.
 */
bool
SimulatorFrontend::memoryAccessTracking() const  {
    return memoryAccessTracking_;
}

/**
 * Sets the printing of the next simulated instruction to the console.
 *
 * The instruction is printed when stopping simulation.
 *
 * @param value True if the instruction should be printed.
 */
void
SimulatorFrontend::setNextInstructionPrinting(bool value) {
    printNextInstruction_ = value;
}

/**
 * Returns true if the next simulated instruction will be printed to the
 * simulator console at simulation stop.
 * 
 * @return True if the instruction should be printed.
 */
bool
SimulatorFrontend::nextInstructionPrinting() const  {
    return printNextInstruction_;
}

/** Sets the printing of simulation time statistics
 * 
 * @param value True if the statistics should be printed out
 */
void SimulatorFrontend::setSimulationTimeStatistics(bool value) {
    printSimulationTimeStatistics_ = value;
}

/**
 * Returns true if the time statistics are to be printed after a command
 * 
 * @return True, if the time statistics are to be printed after a command
 */
bool SimulatorFrontend::simulationTimeStatistics() const {
    return printSimulationTimeStatistics_;
}

/**
 * Returns the StopPointManager.
 *
 * Asserts if no StopPointManager is initiated.
 * @return The used StopPointManager.
 */
StopPointManager&
SimulatorFrontend::stopPointManager() {
    assert(stopPointManager_ != NULL);
    return *stopPointManager_;
}

/**
 * Returns the memory system of the currently loaded machine.
 *
 * Asserts if no simulation is initialized.
 * @todo Throw instead.
 * @return The used MemorySystem.
 */
MemorySystem&
SimulatorFrontend::memorySystem() {
    assert(memorySystem_ != NULL);
    return *memorySystem_;
}

/**
 * Returns a reference to the state model of the currently loaded machine.
 *
 * Asserts if no simulation is initialized.
 * @return State model of the simulated machine.
 */
MachineState&
SimulatorFrontend::machineState() {
    assert (machineState_ != NULL);
    return *machineState_;
}

/**
 * Returns the current utilization statistics of the processor.
 *
 * Recalculates the statistics in case they are old, that is, simulation
 * has been continued or restarted since it was calculated. Should not be
 * called in case simulation is not initialized!
 *
 * @todo: unimplemented for remote debuggers
 */
const UtilizationStats& 
SimulatorFrontend::utilizationStatistics() {
    if (utilizationStats_ == NULL) {
        // stats calculation differs slightly for compiled & interpretive sims.
        if (!isCompiledSimulation()) {
            utilizationStats_ = new UtilizationStats();
            SimulationStatistics stats(
                *currentProgram_, dynamic_cast<SimulationController*>(
                    simCon_)->instructionMemory());
            stats.addStatistics(*utilizationStats_);
            stats.calculate();
        } else {
            CompiledSimUtilizationStats* compiledSimUtilizationStats =
                new CompiledSimUtilizationStats();
            CompiledSimController& compiledSimCon = 
                dynamic_cast<CompiledSimController&>(*simCon_);
            compiledSimUtilizationStats->calculate(program(), 
                *compiledSimCon.compiledSimulation());
            utilizationStats_ = compiledSimUtilizationStats;
        }
    }
    return *utilizationStats_;
}


/**
 * Returns a reference to the last executed instruction.
 *
 * @return Last instruction executed.
 */
const ExecutableInstruction&
SimulatorFrontend::lastExecInstruction() const {
    assert(simCon_ != NULL);
    const InstructionMemory& memory = dynamic_cast<SimulationController*>(
        simCon_)->instructionMemory();
    return memory.instructionAtConst(lastExecutedInstruction());
}


/**
 * Returns reference to the instruction at given address.
 *
 * @return Instruction at the given address.
 */
const ExecutableInstruction&
SimulatorFrontend::executableInstructionAt(
    InstructionAddress address) const {

    assert(simCon_ != NULL);
    const InstructionMemory& memory = dynamic_cast<SimulationController*>(
        simCon_)->instructionMemory();
    return memory.instructionAtConst(address);
}

/**
 * Returns true in case simulation with the currently loaded program
 * cannot be finished automatically.
 *
 * In order for this method to return false, it means that while loading
 * the simulated program, a *probable* ending point in the program was 
 * detected and it is possible that when running the simulation it is possible 
 * to finish it automatically at that position. If this method returns true
 * it is *impossible* to finish simulation automatically.
 *
 * @return True if it's not possible to end simulation automatically.
 */
bool
SimulatorFrontend::automaticFinishImpossible() const {
    return simCon_ != NULL && simCon_->automaticFinishImpossible();
}

/**
 * Returns the last produced execution trace database.
 *
 * The ownership of the TraceDB is transferred to the caller. That is,
 * it should delete it after use. Note that this method returns the 
 * TraceDB only once, next calls return NULL unless new TraceDB has
 * been produced.
 *
 * @return The traceDB instance.
 */
ExecutionTrace*
SimulatorFrontend::lastTraceDB() {

    ExecutionTrace* last = lastTraceDB_;
    lastTraceDB_ = NULL;
    return last;
}

/**
 * Returns the instance of SimulationEventHandler.
 *
 * @return The instance of SimulationEventHandler.
 * @todo This should probably be an inline function as it used to be in
 * the SimulatorToolbox class...
 */
SimulationEventHandler&
SimulatorFrontend::eventHandler() {
    if (eventHandler_ == NULL) {
        eventHandler_ = new SimulationEventHandler();
    }
    return *eventHandler_;
}

/**
 * Returns the count of cycles simulated in the last simulation run.
 *
 * For example, if run was executed and then interrupted by the user
 * or a breakpoint, returns how long the run was in simulation cycles.
 *
 * @return The cycle count.
 */
CycleCount
SimulatorFrontend::lastRunCycleCount() const {
    return lastRunCycleCount_;
}

/**
 * Returns the runtime of the last simulation session in seconds.
 *
 * For example, if run was executed and then interrupted by the user
 * or a breakpoint, returns how long the run was in wall clock time.
 *
 * @return Simulation wall clock time in seconds.
 */
double
SimulatorFrontend::lastRunTime() const {
    return lastRunTime_;
}

/**
 * This method is used to report a runtime error detected in 
 * the simulated program.
 *
 * An SE_RUNTIME_ERROR event is announced after storing the report.
 *
 * @param eventHandler Simulation event handler for the error
 * @param severity Severity classification of the runtime error.
 * @param description Textual description of the error.
 */
void
SimulatorFrontend::reportSimulatedProgramError(
    RuntimeErrorSeverity severity, const std::string& description) {
    ProgramErrorDescription report;
    report.first = severity;
    report.second = description;
    programErrorReports_.push_back(report);
    eventHandler().handleEvent(SimulationEventHandler::SE_RUNTIME_ERROR);
}

/**
 * Returns a program error report with given severity and index.
 *
 * @param severity Severity.
 * @param index Index.
 * @return The error report text.
 */
std::string
SimulatorFrontend::programErrorReport(
    RuntimeErrorSeverity severity, std::size_t index) {

    size_t count = 0;
    for (ProgramErrorDescriptionList::iterator i = 
             programErrorReports_.begin(); i != programErrorReports_.end();
         ++i) {
        if ((*i).first == severity) {
            if (count == index)
                return (*i).second;
            ++count;
        }
    }
    return "";
}

/**
 * Returns the count of program error reports with given severity.
 *
 * @param severity The error report severity interested in.
 * @return The count of error reports.
 */
std::size_t 
SimulatorFrontend::programErrorReportCount(
    RuntimeErrorSeverity severity) {
    size_t count = 0;
    for (ProgramErrorDescriptionList::iterator i = 
             programErrorReports_.begin(); i != programErrorReports_.end();
         ++i) {
        if ((*i).first == severity)
            ++count;
    }
    return count;
}

/**
 * Clears the runtime error report list.
 */
void 
SimulatorFrontend::clearProgramErrorReports() {
    programErrorReports_.clear();
}

/**
 * Compares the states of two simulator engines.
 *
 * This is useful in debugging bugs in an simulator engine implementation.
 * It assumes the engines have been stepped equal amount of instruction cycles 
 * and, thus, the TTA programmer visible context should be equal.
 *
 * @param other A frontend to the other engine to compare this one to.
 * @param differences An optional output stream where to output information
 * of the possible differences.
 * @return true in case the states are equal, false otherwise. 
 */
bool
SimulatorFrontend::compareState(
    SimulatorFrontend& other, std::ostream* differences) {

    static InstructionAddress previousPC = 0;

    if (programCounter() != other.programCounter()) {
        if (differences != NULL) 
            *differences
                << "SIMULATION ERROR DETECTED (PCs DIFFER)" << std::endl
                << "--------------------------------------" << std::endl
                << "      cycle: " << cycleCount() << std::endl
                << " other's PC: " << other.programCounter() << std::endl
                << "   this' PC: " << programCounter() << std::endl;
        return false;
    }

    bool errorLocationPrinted = false;
    bool equal = true;

    const TTAMachine::Machine::RegisterFileNavigator& rfNav = 
        machine().registerFileNavigator();
    for (int i = 0; i < rfNav.count(); ++i) {
        TTAMachine::RegisterFile& rf = *rfNav.item(i);
        for (int reg = 0; reg < rf.size(); ++reg) {
            std::string thisReg = 
                registerFileValue(rf.name(), reg);
            std::string otherReg =
                other.registerFileValue(rf.name(), reg);
            if (thisReg != otherReg) {
                equal = false;
                if (!errorLocationPrinted && differences != NULL) {
                    std::string procedureName =
                        (dynamic_cast<TTAProgram::Procedure&>(
                            program().instructionAt(programCounter()).parent())).name();

                    *differences
                        << "DIFFERING REGISTER FILE VALUES" << std::endl
                        << "------------------------------" << std::endl
                        << "      cycle: " << cycleCount() << std::endl
                        << "         PC: " << programCounter() << std::endl
                        << "previous PC: " << previousPC << std::endl
                        << "   function: " << procedureName << std::endl
                        << "disassembly around previous PC:" << std::endl;
                    int start = 
                        std::max(0, (int)previousPC - 5);
                    int end = previousPC + 5;
                    for (int instr = start; instr <= end; ++instr) {
                        if (instr == (int)previousPC)
                            *differences << "==> ";
                        *differences
                            << POMDisassembler::disassemble(
                                program().instructionAt(instr), 
                                true)
                            << std::endl;
                    }
                    errorLocationPrinted = true;
                }
                if (differences != NULL) 
                    *differences
                        << rf.name() << "." << reg << ": "
                        << thisReg << " (this) vs. " 
                        << otherReg << " (other)" << std::endl;
            }
        } 
    }

    errorLocationPrinted = false;

    const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
        machine().functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); ++i) {
        TTAMachine::FunctionUnit& fu = *fuNav.item(i);

        for (int port = 0; port < fu.portCount(); ++port) {
            // skip output ports as compiled sim is not exact with them at BB boundaries
            if (fu.port(port)->isOutput())
                continue; 
            std::string portName = fu.port(port)->name();
            DisassemblyFUPort portString(fu.name(), portName);
            SimValue thisReg = 
                FUPortValue(fu.name(), portName);
            SimValue otherReg = 
                other.FUPortValue(fu.name(), portName);
            if (thisReg.intValue() != otherReg.intValue()) {
                equal = false;
                if (!errorLocationPrinted && differences != NULL) {
                    std::string procedureName =
                        (dynamic_cast<TTAProgram::Procedure&>(
                            program().instructionAt(programCounter()).parent())).name();

                    *differences 
                        << "DIFFERING FUNCTION UNIT PORT VALUES" << std::endl
                        << "-----------------------------------" << std::endl
                        << "      cycle: " << cycleCount() << std::endl
                        << "         PC: " << programCounter() << std::endl
                        << "previous PC: " << previousPC << std::endl
                        << "   function: " << procedureName << std::endl
                        << "disassembly around previous PC:" << std::endl;
                    int start = 
                        std::max(0, (int)previousPC - 5);
                    int end = previousPC + 5;
                    for (int instr = start; instr <= end; ++instr) {
                        if (instr == (int)previousPC)
                            *differences << "==> ";
                        *differences
                                << POMDisassembler::disassemble(
                                    program().instructionAt(instr), true)
                                << std::endl;
                    }
                    errorLocationPrinted = true;
                }
                if (differences != NULL)
                    *differences
                        << portString.toString() <<  ": "
                        << thisReg.intValue() << " (this) vs. " 
                        << otherReg.intValue() << " (other)" << std::endl;
            }
            
        }


    }

    previousPC = programCounter();

    return equal;
}
/* vim: set ts=4 expandtab: */
