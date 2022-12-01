/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <vector>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <ctime>
#include <stdint.h>
#include "HDBManager.hh"
#include "FUEntry.hh"
#include "TestbenchGenerator.hh"
#include "FUTestbenchGenerator.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "OutputPortState.hh"
#include "InputPortState.hh"
#include "FUPortImplementation.hh"
#include "FUImplementation.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "Conversion.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;

#define STIMULUS_PER_OP 10
#define INDENT "   "

FUTestbenchGenerator::FUTestbenchGenerator(HDB::FUEntry* fu): 
    fuEntry_(fu), fuImpl_(NULL), fuArch_(NULL), msm_(NULL), inputPorts_(),
    outputPorts_(), opcodePort_(), machine_(NULL), memSystem_(NULL) {
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
}


void
FUTestbenchGenerator::generateTestbench(std::ofstream& file) {

    fuImpl_ = &fuEntry_->implementation();
    fuArch_ = &fuEntry_->architecture();
    
    createMachineState();
    parseFuPorts();
    createTbInstantiation();
    createStimulus();
    createTbCode();
    writeTestbench(file, fuImpl_);
}

/**
 * Create a machine architecture object model with the tested FU in it
 */
void
FUTestbenchGenerator::createMachineState() {

    machine_ = new TTAMachine::Machine();
    machine_->addFunctionUnit(fuArch_->architecture());

    MachineStateBuilder msmBuilder;
    
    memSystem_ = new MemorySystem(*machine_);

    msm_ = msmBuilder.build(*machine_, *memSystem_);
}


void 
FUTestbenchGenerator::parseFuPorts() {

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

/**
 * Creates component declaration, connection signals and connects FU component
 * to testbench
 */
void
FUTestbenchGenerator::createTbInstantiation() {

    bindingStream() 
        << INDENT << "for tested_fu_0 : fu_under_test use entity work.";
    bindingStream() << fuImpl_->moduleName() << ";" << std::endl;
    
    declarationStream()
        << INDENT << "component fu_under_test" << std::endl
        << INDENT INDENT << "port(" << std::endl;
    
    instantiationStream()
        << INDENT << "tested_fu_0\t:\tfu_under_test " << std::endl
        << INDENT INDENT << "port map (" << std::endl;

    for (int i = 0; i < fuImpl_->architecturePortCount(); ++i) {
        HDB::FUPortImplementation& port = fuImpl_->architecturePort(i);
        // this might not always work..
        const int portWidth = fuArch_->architecture().port(i)->width();
        const bool isInput = 
            ContainerTools::containsValue(
                inputPorts_, port.architecturePort());
        declarationStream() 
            << INDENT INDENT << port.name() << "\t: ";
        signalStream()
            << INDENT << "signal " << port.name() << "\t: ";
        instantiationStream()
            << INDENT INDENT INDENT << port.name() << " => " << port.name();

        if (isInput)
            declarationStream() << "in";
        else
            declarationStream() << "out";

        declarationStream()
            << " std_logic_vector("
            << portWidth - 1 << " downto 0);" << std::endl;

        signalStream()
            << "std_logic_vector("
            << portWidth - 1 << " downto 0);" << std::endl;

        if (isInput) {
            declarationStream()
                << INDENT INDENT << port.loadPort() 
                << "\t: in  std_logic;" << std::endl;

            signalStream()
                << INDENT << "signal " << port.loadPort() 
                << "\t: std_logic_vector(1-1 downto 0);" << std::endl;

            instantiationStream()
                << "," << std::endl
                << INDENT INDENT INDENT << port.loadPort() << " => " 
                << port.loadPort() << "(0)";
        }
        if (i < fuImpl_->architecturePortCount() - 1) {
            instantiationStream() << "," << std::endl;
        } else {
            if (fuImpl_->opcodePort() != "") {
                declarationStream()
                    << INDENT INDENT << fuImpl_->opcodePort() << "\t: "
                    << "in" << " std_logic_vector("
                    << fuImpl_->maxOpcodeWidth() - 1 << " downto 0);"
                    << std::endl;
                instantiationStream()
                    << "," << std::endl
                    << INDENT INDENT INDENT 
                    << fuImpl_->opcodePort() 
                    << " => " << fuImpl_->opcodePort();
                signalStream()
                    << INDENT << "signal " << fuImpl_->opcodePort() << "\t: ";
                signalStream()
                    << "std_logic_vector("
                    << fuImpl_->maxOpcodeWidth() - 1 
                    << " downto 0);" << std::endl;
            }
        }
    }
    instantiationStream() 
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

    declarationStream()
        << INDENT INDENT << fuImpl_->glockPort() << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << fuImpl_->rstPort()   << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << fuImpl_->clkPort()   << "\t: in  std_logic);" 
        << std::endl
        << INDENT << "end component;" 
        << std::endl;

    signalStream()
        << INDENT << "signal " << fuImpl_->glockPort() << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << fuImpl_->rstPort() << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << fuImpl_->clkPort() << "\t: std_logic;"
        << std::endl;
}


/**
 * Creates input and output data tables
 *
 * Creates input and output data tables and control signals for the testbench.
 * Every operation is tested STIMULUS_PER_OP times and command execution is
 * pipelined. Only fully pipelined FUs are supported.
 */
void
FUTestbenchGenerator::createStimulus() {

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
        if (fuArch_->architecture().operationCount() > 1) {
            // insert dummy values for opcode port
            const string operation = 
                fuArch_->architecture().operation(lastOpIndex)->name();
            startedOperations.push_back(operation);
        }
        for (std::size_t j = 0; j < inputPorts_.size(); ++j) {
            const string portName = inputPorts_.at(j);
            // operation does not matter
            const string operation = 
                fuArch_->architecture().operation(lastOpIndex)->name();
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
    
    int waitCycles = 
        fuArch_->architecture().operation(startedOperations.at(0))->latency();
    // TODO: change this when supporting different pipelines
    int opCount = fuArch_->architecture().operationCount();
    int latencyOfLastOp =
        fuArch_->architecture().operation(opCount-1)->latency();
    int totalCycles = 
        STIMULUS_PER_OP * opCount + latencyOfLastOp;
    writeTbConstants(totalCycles, waitCycles);
}


/**
 * Writes the testbench main process code
 */
void
FUTestbenchGenerator::createTbCode() {

    for (std::size_t i = 0; i < inputPorts_.size(); ++i) {
        const std::string portName = inputPorts_.at(i);
        const std::string hwDataPortName = 
            fuImpl_->portImplementationByArchitectureName(portName).name();
        const std::string hwLoadPortName = 
            fuImpl_->
            portImplementationByArchitectureName(portName).loadPort();
        tbCodeStream()
            << INDENT INDENT 
            << hwDataPortName << " <= " << hwDataPortName << "_data("
            << "current_cycle);" << std::endl
            << INDENT INDENT << hwLoadPortName << " <= " 
            << hwLoadPortName << "_data(current_cycle);" << std::endl;        
    }
    
    if (fuArch_->architecture().operationCount() > 1) {
        tbCodeStream() 
            << INDENT INDENT 
            << fuImpl_->opcodePort() 
            << " <= " << fuImpl_->opcodePort() << "_data(current_cycle); ";
    }

    tbCodeStream()
        << std::endl << std::endl
        << INDENT INDENT 
        << "if current_cycle >= IGNORE_OUTPUT_COUNT then" << std::endl;

    for (std::size_t i = 0; i < outputPorts_.size(); ++i) {
        const std::string portName = outputPorts_.at(i);
        const std::string hwDataPortName = 
            fuImpl_->portImplementationByArchitectureName(portName).name();
        tbCodeStream() 
            << INDENT INDENT INDENT 
            << "assert " << hwDataPortName << " = " << hwDataPortName 
            << "_data(current_cycle)" << std::endl
            << INDENT INDENT INDENT INDENT 
            << "report lf & \"TCE Assert: Verification failed at cycle \" "
            << "& integer'image(current_cycle) & \" for output " << i << "\"" << std::endl
            << INDENT INDENT INDENT INDENT <<"& \" actual: \" "
            << "& to_hstring(" << hwDataPortName << ")" 
            << std::endl
            << INDENT INDENT INDENT INDENT << "& " 
            << "\" expected: \" & to_hstring(" << hwDataPortName
            << "_data(current_cycle))  severity error;" 
            << std::endl << std::endl;
    }

    tbCodeStream()
        << INDENT INDENT 
        << "end if;" << std::endl;
}

/**
 * Writes input data to the given port and saves the input value to an array
 *
 * @param inputs PortDataArray containing input ports
 * @param operation Name of the triggered operation
 * @param portName Name of the port where data is written to
 * @param stimulus Input data
 */
void
FUTestbenchGenerator::writeInputPortStimulus(
    PortDataArray& inputs,
    const std::string& operation, 
    const std::string& portName, uint32_t stimulus) {

    string operationString = "";
    PortState* simulatedPort = NULL;
    bool isOpcodePort = false;
    // fetch the virtual opcode setting port for the triggered operation
    if (portName == opcodePort_) {
        operationString = 
            StringTools::stringToLower(
                std::string(".") + operation);
        isOpcodePort = true;
    }

    simulatedPort = &msm_->portState(
        portName + operationString,
        fuArch_->architecture().name());
    
    const int inputWidth = simulatedPort->value().width();
    SimValue value(inputWidth);

    int wantedBits = inputWidth;
    if (isShiftOrRotOp(operation) && isOpcodePort
        && inputWidth > 5) {
        // log2(32) = 5 bits needed to express max shift
        wantedBits = 5;
    }
    stimulus = truncateStimulus(stimulus, wantedBits);

    inputs[portName].push_back(stimulus);
    value = stimulus;
    simulatedPort->setValue(value);
}

/**
 * Reads data from output ports and saves the values to an array
 *
 * @param outputs PortDataArray containing the output ports
 */
void 
FUTestbenchGenerator::readValuesFromOutPorts(PortDataArray& outputs) {

    for (std::size_t i = 0; i < outputPorts_.size(); ++i) {
        const string portName = outputPorts_.at(i);

        PortState* simulatedPort = NULL;
        simulatedPort = &msm_->portState(
            portName, fuArch_->architecture().name());

        outputs[portName].push_back(
            simulatedPort->value().intValue());
    }
}

/**
 * Writes input, output and control signal data to output streams
 *
 * @param inputStimulus Input port data
 * @param loadStimulus Load port data
 * @param operations Triggered operations
 * @param outputStimulus Output port data
 */
void 
FUTestbenchGenerator::createStimulusArrays(
    PortDataArray& inputStimulus, 
    std::vector<uint32_t>& loadStimulus,
    std::vector<std::string>& operations, PortDataArray& outputStimulus) {

    // input array(s)
    for (PortDataArray::iterator i = inputStimulus.begin();
         i != inputStimulus.end(); i++) {
        HDB::FUPortImplementation* port = 
            &fuImpl_->portImplementationByArchitectureName((*i).first);
        const string hwPortName = port->name();
        const int hwPortWidth = 
            fuArch_->architecture().port((*i).first)->width();
        vector<uint32_t> data = i->second;
        writeStimulusArray(inputArrayStream(), data, hwPortName, hwPortWidth);

        const string loadPortName = port->loadPort();
        if (!loadPortName.empty()) {
            int loadPortWidth = 1;
            writeStimulusArray(loadArrayStream(), loadStimulus, loadPortName,
                               loadPortWidth);
        }
    }
    
    // opcode arrays
    // Special case so we can print the operation names to the testbench
    const int operationsInFU = fuArch_->architecture().operationCount(); 
    if (operationsInFU > 1) {
        const string hwPortName = fuImpl_->opcodePort();
        const int hwPortWidth = fuImpl_->maxOpcodeWidth();
        opcodeArrayStream()
            << INDENT INDENT
            << "type " << hwPortName << "_data_array is array "
            << "(natural range <>) of"  << std::endl << INDENT INDENT INDENT
            << "std_logic_vector(" << hwPortWidth - 1 << " downto 0);" 
            << std::endl << std::endl
            << INDENT INDENT
            << "constant "<< hwPortName << "_data : " << hwPortName 
            << "_data_array :=" << std::endl;
        
        for (std::size_t i = 0; i < operations.size(); ++i) {
            uint32_t input = fuImpl_->opcode(operations.at(i));
            std::string inputAsBinaryLiteral = 
                Conversion::toBinary(input, hwPortWidth);
            opcodeArrayStream() << INDENT INDENT;
            if (i == 0) {
                opcodeArrayStream() << "(";
            } else {
                opcodeArrayStream() << " ";
            }
            opcodeArrayStream() << "\"" << inputAsBinaryLiteral << "\"";
            if (i == operations.size() - 1) {
                opcodeArrayStream() << ");";
            } else {
                opcodeArrayStream() << ",";
            }
            opcodeArrayStream()
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
        vector<uint32_t> data = i->second;
        writeStimulusArray(outputArrayStream(), data, hwPortName, 
                           hwPortWidth);
    }
}


/**
 * Test if operation is shift or rotation operation
 *
 * @param operation Name of the operation
 * @return Is operation shift or rotate
 */
bool
FUTestbenchGenerator::isShiftOrRotOp(const std::string& operation) const {

    string opName = StringTools::stringToLower(operation);
    
    if (opName == "shl" || opName == "shr" || opName == "shru") {
        return true;
    } else if (opName == "rotl" || opName == "rotr") {
        return true;
    }
    return false;
}

/**
 * Truncates shift operand to log2(32) bits
 *
 * @param operand Operand to be truncated
 * @return Truncated operand
 */
uint32_t
FUTestbenchGenerator::truncateStimulus(uint32_t operand, int nBits) const {
    
    if (nBits < 0) {
        InvalidData exc(__FILE__, __LINE__, "ImplementationTester",
                        "Negative amount f wanted bits");
        throw exc;
    }

    unsigned int dataWidth = 32;
    uint32_t truncated =
        (operand << (dataWidth - nBits) >> (dataWidth - nBits));
    return truncated;
}
