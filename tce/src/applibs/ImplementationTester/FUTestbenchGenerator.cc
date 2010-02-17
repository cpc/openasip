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
 * @file FUTestbenchGenerator.cc
 *
 * Implementation of FUTestbenchGenerator class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <vector>
#include <boost/format.hpp>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <ctime>

#include "HDBManager.hh"
#include "FUEntry.hh"
#include "FUTestbenchGenerator.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "GlobalLock.hh"
#include "OutputPortState.hh"
#include "InputPortState.hh"
#include "FUPortImplementation.hh"
#include "FUImplementation.hh"
#include "HWOperation.hh"
#include "StringTools.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

#define STIMULUS_PER_OP 10
#define INDENT "   "

const std::string FUTestbenchGenerator::FU_TB_TEMPLATE_ = 
    "fu_testbench.vhdl.template";

FUTestbenchGenerator::FUTestbenchGenerator(HDB::FUEntry* fu): 
    fuEntry_(fu), fuImpl_(NULL), fuArch_(NULL), msm_(NULL), inputPorts_(),
    outputPorts_(), opcodePort_(), machine_(NULL), memSystem_(NULL),
    glock_(NULL) {
}

FUTestbenchGenerator::~FUTestbenchGenerator() {
    if (msm_) {
        delete(msm_);
    }
    if (machine_) {
        delete(machine_);
    }
    if (memSystem_) {
        delete(memSystem_);
    }
    if (glock_) {
        delete(glock_);
    }
}

void FUTestbenchGenerator::generateTestbench(ofstream& file) {
    fuImpl_ = &fuEntry_->implementation();
    fuArch_ = &fuEntry_->architecture();
    
    createMachineState();
    parseFuPorts();
    createTbInstantiation();
    createStimulus();
    createTbCode();
    writeTestbench(file);
}

void
FUTestbenchGenerator::createMachineState() {
    // create a machine architecture object model with the tested FU in it
    machine_ = new TTAMachine::Machine();
    machine_->addFunctionUnit(fuArch_->architecture());


    // create a simulation model (machine state model) for the machine 
    MachineStateBuilder msmBuilder;
    
    // dummy, empty memory system. No support for memory accessing FUs
    memSystem_ = new MemorySystem(*machine_);

    glock_ = new GlobalLock();
    
    msm_ = msmBuilder.build(*machine_, *memSystem_, *glock_);
}

void FUTestbenchGenerator::parseFuPorts() {
    // divide ports into input and output ports
    for (int i = 0; i < fuArch_->architecture().portCount(); i++) {
        const std::string portName = fuArch_->architecture().port(i)->name();
        PortState& simulatedPort = 
            msm_->portState(portName, fuArch_->architecture().name());

        if (fuArch_->architecture().port(i)->isOpcodeSetting())
            opcodePort_ = portName;

        if (dynamic_cast<OutputPortState*>(&simulatedPort)) {
            outputPorts_.push_back(portName);
        } else if (dynamic_cast<InputPortState*>(&simulatedPort)) {
            inputPorts_.push_back(portName);
        } else if (&simulatedPort == &NullPortState::instance()) {
            InvalidData e(__FILE__, __LINE__, "ImplementationTester",
                          "Port not found in state");
            throw e;
        } else {
            InvalidData e(__FILE__, __LINE__, "ImplementationTester",
                          "Port " + portName + " has unknown direction.");
            throw e;
        }
    }
}

void FUTestbenchGenerator::createTbInstantiation() {
    componentBinding_ 
        << INDENT << "for tested_fu_0 : fu_under_test use entity work.";
    componentBinding_ << fuImpl_->moduleName() << ";" << std::endl;

    componentDeclaration_
        << INDENT << "component fu_under_test" << std::endl
        << INDENT INDENT << "port(" << std::endl;
    
    componentInstantiation_
        << INDENT << "tested_fu_0\t:\tfu_under_test " << std::endl
        << INDENT INDENT << "port map (" << std::endl;

    for (int i = 0; i < fuImpl_->architecturePortCount(); ++i) {
        HDB::FUPortImplementation& port = fuImpl_->architecturePort(i);
        // this might not always work..
        const int portWidth = fuArch_->architecture().port(i)->width();
        const bool isInput = 
            ContainerTools::containsValue(
                inputPorts_, port.architecturePort());
        componentDeclaration_ 
            << INDENT INDENT << port.name() << "\t: ";
        registerSpecifications_
            << INDENT << "signal " << port.name() << "\t: ";
        componentInstantiation_
            << INDENT INDENT INDENT << port.name() << " => " << port.name();

        if (isInput)
            componentDeclaration_ << "in";
        else
            componentDeclaration_ << "out";

        componentDeclaration_
            << " std_logic_vector("
            << portWidth - 1 << " downto 0);" << std::endl;

        registerSpecifications_
            << "std_logic_vector("
            << portWidth - 1 << " downto 0);" << std::endl;

        if (isInput) {
            componentDeclaration_
                << INDENT INDENT << port.loadPort() 
                << "\t: in  std_logic;" << std::endl;

            registerSpecifications_
                << INDENT << "signal " << port.loadPort() 
                << "\t: std_logic;" << std::endl;

            componentInstantiation_
                << "," << std::endl
                << INDENT INDENT INDENT << port.loadPort() << " => " 
                << port.loadPort();
        }
        if (i < fuImpl_->architecturePortCount() - 1) {
            componentInstantiation_ << "," << std::endl;
        } else {
            if (fuImpl_->opcodePort() != "") {
                componentDeclaration_
                    << INDENT INDENT << fuImpl_->opcodePort() << "\t: "
                    << "in" << " std_logic_vector("
                    << fuImpl_->maxOpcodeWidth() - 1 << " downto 0);"
                    << std::endl;
                componentInstantiation_
                    << "," << std::endl
                    << INDENT INDENT INDENT 
                    << fuImpl_->opcodePort() 
                    << " => " << fuImpl_->opcodePort();
                registerSpecifications_
                    << INDENT << "signal " << fuImpl_->opcodePort() << "\t: ";
                registerSpecifications_
                    << "std_logic_vector("
                    << fuImpl_->maxOpcodeWidth() - 1 
                    << " downto 0);" << std::endl;
            }
        }
    }
    componentInstantiation_ 
        << "," << std::endl
        << INDENT INDENT INDENT
        << fuImpl_->clkPort() << " => " << fuImpl_->clkPort() 
        << "," << std::endl
        << INDENT INDENT INDENT
        << fuImpl_->rstPort() << " => " << fuImpl_->rstPort() 
        << "," << std::endl
        << INDENT INDENT INDENT
        << fuImpl_->glockPort() << " => " << fuImpl_->glockPort() 
        << ");";

    componentDeclaration_
        << INDENT INDENT << fuImpl_->glockPort() << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << fuImpl_->rstPort()   << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << fuImpl_->clkPort()   << "\t: in  std_logic);" 
        << std::endl
        << INDENT << "end component;" 
        << std::endl;

    registerSpecifications_
        << INDENT << "signal " << fuImpl_->glockPort() << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << fuImpl_->rstPort()   << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << fuImpl_->clkPort() << "  " 
        << "\t: std_logic;" << std::endl;
}

void FUTestbenchGenerator::createStimulus() {
    FUState& simFU = msm_->fuState(fuArch_->architecture().name());
    assert(&simFU != &NullFUState::instance());

    // stimulus for each port in each clock cycle
    PortDataArray inputStimulus;

    // stimulus for load ports. Notice that all load ports get the same
    // signal!
    // TODO: Exploit this when supporting non-fully pipelined operations
    //       (if latency(op N) > latency(op N+1) there needs to be no-load
    //       cycles when switching operations)
    vector<uint32_t> loadStimulus;

    // operations started in each clock cycle (names)
    vector<string> startedOperations;

    // expected output for each port in each clock cycle
    PortDataArray outputs;

    // initialize a random number generator for the stimuli
    boost::uniform_int<> distribution(INT_MIN, INT_MAX);
    boost::mt19937 rng;
    rng.seed(time(NULL));
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        randomNumber(rng, distribution);

    const int cyclesToSimulate = STIMULUS_PER_OP;
    for (int opIndex = 0; opIndex < fuArch_->architecture().operationCount(); 
         ++opIndex) {
        const string operation = 
            fuArch_->architecture().operation(opIndex)->name();

        for (int i = 0; i < cyclesToSimulate; i++) {
            startedOperations.push_back(operation);

            // generate stimulus for each port
            for (std::size_t i = 0; i < inputPorts_.size(); ++i) {
                const string portName = inputPorts_.at(i);
                writeInputPortStimulus(
                    inputStimulus, 
                    operation, portName, (uint32_t)randomNumber());
            }
            
            // load signal stimulus
            uint32_t loadOnThisCycle = 1;
            loadStimulus.push_back(loadOnThisCycle);
 
            readValuesFromOutPorts(outputs);
            // advance the simulation clock
            simFU.endClock();
            simFU.advanceClock();
        }
    }
    // flush the pipeline
    int lastOpIndex = fuArch_->architecture().operationCount()-1;
    int pipelineClearCycles =
        fuArch_->architecture().operation(lastOpIndex)->latency();
    for (int i = 0; i < pipelineClearCycles; i++) {
        
        for (std::size_t j = 0; j < inputPorts_.size(); ++j) {
            const string portName = inputPorts_.at(j);
            // operation does not matter
            const string operation = 
                fuArch_->architecture().operation(lastOpIndex)->name();
            startedOperations.push_back(operation);
            uint32_t inputStim = 0;
            writeInputPortStimulus(
                inputStimulus, operation, portName, inputStim);
        }
        // no load on these cycles
        uint32_t loadStim = 0;
        loadStimulus.push_back(loadStim);
        
        readValuesFromOutPorts(outputs);
        // advance the simulation clock
        simFU.endClock();
        simFU.advanceClock();
    }
    createStimulusArrays(
        inputStimulus, loadStimulus, startedOperations, outputs);
}

void FUTestbenchGenerator::createTbCode() {
    for (std::size_t i = 0; i < inputPorts_.size(); ++i) {
        const std::string portName = inputPorts_.at(i);
        const std::string hwDataPortName = 
            fuImpl_->portImplementationByArchitectureName(portName).name();
        const std::string hwLoadPortName = 
            fuImpl_->
            portImplementationByArchitectureName(portName).loadPort();
        testBenchCode_ 
            << INDENT INDENT 
            << hwDataPortName << " <= " << hwDataPortName << "_inputs("
            << "current_cycle);" << std::endl
            << INDENT INDENT << hwLoadPortName 
            << " <= load_inputs(current_cycle);" << std::endl;        
    }
    
    if (fuArch_->architecture().operationCount() > 1) {
        testBenchCode_ 
            << INDENT INDENT 
            << fuImpl_->opcodePort() 
            << " <= opcode_inputs(current_cycle); ";
    }

    testBenchCode_
        << std::endl << std::endl
        << INDENT INDENT 
        << "if current_cycle >= IGNORE_OUTPUT_COUNT then" << std::endl;

    for (std::size_t i = 0; i < outputPorts_.size(); ++i) {
        const std::string portName = outputPorts_.at(i);
        const std::string hwDataPortName = 
            fuImpl_->portImplementationByArchitectureName(portName).name();
        testBenchCode_ 
            << INDENT INDENT INDENT 
            << "assert " << hwDataPortName << " = " 
            << "expected_" << hwDataPortName << "(current_cycle)"
            << std::endl
            << INDENT INDENT INDENT INDENT 
            << "report lf & "
            << "\"TCE Assert: Verification failed at cycle \" & str(current_cycle, 10) "
            << "severity error;" << std::endl << std::endl;
    }

    testBenchCode_
        << INDENT INDENT 
        << "end if;" << std::endl;
}

void FUTestbenchGenerator::writeTestbench(std::ofstream& file) {
    string templateFile = findFuTemplate();
    string vhdlTemplate = "";
    loadVHDLTemplate(templateFile, vhdlTemplate);
    
    string testBench = 
        (boost::format(vhdlTemplate)         
         % componentDeclaration_.str()
         % componentBinding_.str()
         % registerSpecifications_.str()
         % componentInstantiation_.str()
         % stimulusArrays_.str() 
         % opcodeArray_.str()
         % loadSignalArrays_.str()
         % expectedOutputs_.str()
         % fuImpl_->clkPort()
         % fuImpl_->rstPort()
         % fuImpl_->glockPort()
         % testBenchCode_.str()).str();

    file << testBench;
}

void 
FUTestbenchGenerator::loadVHDLTemplate(
    const string& fileName, string& vhdlTemplate) {

    ifstream input(fileName.c_str());
    if (!input.is_open()) {
        InvalidData exception(__FILE__, __LINE__, "", 
                              "The VHDL template file " + fileName + 
                              "unreadable.");
        throw exception;
    }

    string line = "";
    while (getline(input, line)) {
        vhdlTemplate += line;
        vhdlTemplate += "\n";
    }
    input.close();
}

void
FUTestbenchGenerator::writeInputPortStimulus(
    PortDataArray& inputs,
    const string& operation, const string& portName, uint32_t stimulus) {
    string operationString = "";
    PortState* simulatedPort = NULL;
    // fetch the virtual opcode setting port for the triggered 
    // operation
    if (portName == opcodePort_) {
        operationString = 
            StringTools::stringToLower(
                std::string(".") + operation);
    }
    simulatedPort = &msm_->portState(
        portName + operationString,
        fuArch_->architecture().name());
    
    const int inputWidth = simulatedPort->value().width();
    SimValue value(inputWidth);

    stimulus = (stimulus << (32 - inputWidth) >> (32 - inputWidth));
    inputs[portName].push_back(stimulus);

    value = stimulus;
    simulatedPort->setValue(value);
}

void FUTestbenchGenerator::readValuesFromOutPorts(PortDataArray& outputs) {
    for (std::size_t i = 0; i < outputPorts_.size(); ++i) {
        const string portName = outputPorts_.at(i);

        PortState* simulatedPort = NULL;
        simulatedPort = &msm_->portState(
            portName, fuArch_->architecture().name());

        outputs[portName].push_back(
            simulatedPort->value().intValue());
    }
}

void 
FUTestbenchGenerator::createStimulusArrays(
    PortDataArray& inputStimulus, 
    std::vector<uint32_t>& loadStimulus,
    std::vector<std::string>& operations, PortDataArray& outputStimulus) {
    
    // input array(s)
    for (PortDataArray::iterator i = inputStimulus.begin();
         i != inputStimulus.end(); i++) {
        const string hwPortName = 
            fuImpl_->portImplementationByArchitectureName((*i).first).name();
        const int hwPortWidth = 
            fuArch_->architecture().port((*i).first)->width();

        stimulusArrays_
            << INDENT INDENT 
            << "type " << hwPortName 
            << "_input_array is array (natural range <>) of" << std::endl
            << INDENT INDENT INDENT
            << "std_logic_vector(" << hwPortWidth - 1 << " downto 0);" 
            << std::endl << std::endl
            << INDENT INDENT
            << "constant " << hwPortName << "_inputs : "
            << hwPortName << "_input_array :=" << std::endl;

        const vector<uint32_t>& inputs = (*i).second;
        for (std::size_t j = 0; j < inputs.size(); ++j) {
            uint32_t input = inputs.at(j);
            std::string inputAsBinaryLiteral = 
                Conversion::toBinary(input, hwPortWidth);
            stimulusArrays_ << INDENT INDENT;
            
            if (j == 0) {
                stimulusArrays_ << "(";
            } else {
                stimulusArrays_ << " ";
            }
            
            stimulusArrays_ << "\"" << inputAsBinaryLiteral << "\"";
            
            if (j == inputs.size() - 1) {
                stimulusArrays_ << ");";
            } else {
                stimulusArrays_ << ",";
            }
            stimulusArrays_ 
                << "\t -- @" << j << " = " << input << std::endl;   
        }
        stimulusArrays_ << std::endl;
    }
    
    // load signals array
    loadSignalArrays_ 
        << INDENT INDENT << "type load_input_array is array "
        << "(natural range <>) of std_logic;" << std::endl << std::endl
        << INDENT INDENT
        << "constant load_inputs : load_input_array :=" << std::endl;
    for (unsigned int i = 0; i < loadStimulus.size(); i++) {
        loadSignalArrays_ << INDENT INDENT;
        if (i == 0) {
            loadSignalArrays_ << "(";
        } else {
            loadSignalArrays_ << " ";
        }

        loadSignalArrays_ << "'" << loadStimulus.at(i) << "'";

        if (i == loadStimulus.size() - 1) {
            loadSignalArrays_ << ");";
        } else {
            loadSignalArrays_ << ",";
        }
        loadSignalArrays_ << std::endl;
    }
    
    // opcode arrays
    const int operationsInFU = fuArch_->architecture().operationCount(); 
    if (operationsInFU > 1) {
        const string hwPortName = fuImpl_->opcodePort();
        const int hwPortWidth = fuImpl_->maxOpcodeWidth();
        opcodeArray_ 
            << INDENT INDENT
            << "type opcode_input_array is array (natural range <>) of" 
            << std::endl << INDENT INDENT INDENT
            << "std_logic_vector(" << hwPortWidth - 1 << " downto 0);" 
            << std::endl << std::endl
            << INDENT INDENT
            << "constant opcode_inputs : opcode_input_array :=" << std::endl;
        
        for (std::size_t i = 0; i < operations.size(); ++i) {
            uint32_t input = fuImpl_->opcode(operations.at(i));
            std::string inputAsBinaryLiteral = 
                Conversion::toBinary(input, hwPortWidth);
            opcodeArray_ << INDENT INDENT;
            if (i == 0) {
                opcodeArray_ << "(";
            } else {
                opcodeArray_ << " ";
            }
            opcodeArray_ << "\"" << inputAsBinaryLiteral << "\"";
            if (i == operations.size() - 1) {
                opcodeArray_ << ");";
            } else {
                opcodeArray_ << ",";
            }
            opcodeArray_
                << "\t -- @" << i << " = " << input << " (" 
                << operations.at(i) << ")" << std::endl;
        }
    }
    
    // output arrays
    for (PortDataArray::iterator i = outputStimulus.begin();
         i != outputStimulus.end(); ++i) {
        const std::string hwPortName = 
            fuImpl_->portImplementationByArchitectureName((*i).first).name();
        const int hwPortWidth = 
            fuArch_->architecture().port((*i).first)->width();
        
        expectedOutputs_
            << INDENT INDENT 
            << "type expected_" << hwPortName 
            << "_array is array (natural range <>) of" << std::endl
            << INDENT INDENT INDENT
            << "std_logic_vector(" << hwPortWidth - 1 << " downto 0);" 
            << std::endl << std::endl
            << INDENT INDENT
            << "constant expected_" << hwPortName << " : "
            << "expected_" << hwPortName << "_array :=" << std::endl;
        
        const std::vector<uint32_t>& outs = (*i).second;
        for (std::size_t j = 0; j < outs.size(); ++j) {
            uint32_t output = outs.at(j);
            string outputAsBinaryLiteral = 
                Conversion::toBinary(output, hwPortWidth);

            expectedOutputs_ << INDENT INDENT;
            if (j == 0) {
                expectedOutputs_ << "(";
            } else {
                expectedOutputs_ << " ";
            }

            expectedOutputs_ << "\"" << outputAsBinaryLiteral << "\"";

            if (j == outs.size() - 1) {
                expectedOutputs_ << ");";
            } else {
                expectedOutputs_ << ",";
            }
            expectedOutputs_ 
                << "\t -- @" << j << " = " << output << std::endl;
            }
        expectedOutputs_ << std::endl;
    }

    // these should actually go to testbench code
    const int waitCycles = 
        fuArch_->architecture().operation(operations.at(0))->latency();
    // TODO: change this when supporting different pipelines
    int opCount = fuArch_->architecture().operationCount();
    int latencyOfLastOp =
        fuArch_->architecture().operation(opCount-1)->latency();
    int totalCycles = 
        STIMULUS_PER_OP * opCount + latencyOfLastOp;
    expectedOutputs_ 
        << INDENT INDENT << "constant IGNORE_OUTPUT_COUNT : integer := " 
        << waitCycles << ";" << std::endl;
    expectedOutputs_
        << INDENT INDENT << "constant TOTAL_CYCLE_COUNT : integer := " 
        << totalCycles << ";" << std::endl;
}

std::string FUTestbenchGenerator::findFuTemplate() {
    vector<string> paths = Environment::implementationTesterTemplatePaths();
    for (unsigned int i = 0; i < paths.size(); i++) {
        string file = paths.at(i) + FileSystem::DIRECTORY_SEPARATOR 
            + FU_TB_TEMPLATE_;
        if (FileSystem::fileExists(file)) {
            return file;
        }
    }

    InvalidData exception(__FILE__, __LINE__, "", 
                          "The VHDL template file " + FU_TB_TEMPLATE_ + 
                          "not found");
    throw exception;

    string notFound = "";
    return notFound;
}


