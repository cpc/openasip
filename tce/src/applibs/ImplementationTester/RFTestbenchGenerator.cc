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
 * @file RFTestbenchGenerator.cc
 *
 * Implementation of RFTestbenchGenerator class.
 *
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
#include "RFEntry.hh"
#include "TestbenchGenerator.hh"
#include "RFTestbenchGenerator.hh"
#include "RFArchitecture.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "RegisterFileState.hh"
#include "RFPortImplementation.hh"
#include "RFImplementation.hh"
#include "MathTools.hh"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::ostringstream;

#define INDENT "   "

const std::string RFTestbenchGenerator::RF_NAME_ = "testRF";

RFTestbenchGenerator::RFTestbenchGenerator(HDB::RFEntry* rf):
    rfEntry_(rf), rfImpl_(NULL), rfArch_(NULL), machRf_(NULL), msm_(NULL),
    inputPorts_(), inputLoadPorts_(), inputOpcodePorts_(), outputPorts_(),
    outputLoadPorts_(), outputOpcodePorts_() {

}

RFTestbenchGenerator::~RFTestbenchGenerator() {
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

/**
 * Creates the testbench and writes it to the given filestream
 *
 * @param file Filestream where the testbench is written
 */
void 
RFTestbenchGenerator::generateTestbench(std::ofstream& file) {

    rfImpl_ = &rfEntry_->implementation();
    rfArch_ = &rfEntry_->architecture();

    createMachineState();
    parseRfPorts();
    createStimulus();
    createTbInstantiation();
    createTbCode();
    writeTestbench(file, rfImpl_);
}

/**
 * Creates machine state model with the RF under test in it
 */
void
RFTestbenchGenerator::createMachineState() {

    machine_ = new TTAMachine::Machine();
    int size = 0;
    if (rfArch_->hasParameterizedSize()) {
        // set reasonable default value
        size = 16;
        rfArch_->setSize(size);
    } else {
        size = rfArch_->size();
    }
    int width = 0;
    if (rfArch_->hasParameterizedWidth()) {
        width = 32;
        rfArch_->setWidth(width);
    } else {
        width = rfArch_->width();
    }

    string name = RF_NAME_;
    // create simulation model of the RF
    machRf_ = 
        new TTAMachine::RegisterFile(
            name, size, width, rfArch_->maxReads(), rfArch_->maxWrites(),
            rfArch_->guardLatency(), TTAMachine::RegisterFile::NORMAL,
            rfArch_->zeroRegister());
    machine_->addRegisterFile(*machRf_);

    MachineStateBuilder msmBuilder;
    
    memSystem_ = new MemorySystem(*machine_);

    msm_ = msmBuilder.build(*machine_, *memSystem_);
}


void 
RFTestbenchGenerator::parseRfPorts() {

     for (int i = 0; i < rfImpl_->portCount(); i++) {
         string portName = rfImpl_->port(i).name();
         string opcodePort = rfImpl_->port(i).opcodePort();
         string loadPort =  rfImpl_->port(i).loadPort();
         if (rfImpl_->port(i).direction() == HDB::IN) {
             inputPorts_.push_back(portName);
             inputOpcodePorts_.push_back(opcodePort);
             inputLoadPorts_.push_back(loadPort);
         } else if (rfImpl_->port(i).direction() == HDB::OUT) {
             outputPorts_.push_back(portName);
             outputOpcodePorts_.push_back(opcodePort);
             outputLoadPorts_.push_back(loadPort);
         } else {
             assert(false && "RF port implementation does not have direction");
         }
     }
 }


/**
 * Creates component declaration, connection signals and connects RF component
 * to testbench
 */
void 
RFTestbenchGenerator::createTbInstantiation() {

    bindingStream() 
        << INDENT << "for tested_rf_0 : rf_under_test use entity work.";
    bindingStream() << rfImpl_->moduleName() << ";" << std::endl;
    
    declarationStream()
        << INDENT << "component rf_under_test" << std::endl;
    instantiationStream()
        << INDENT << "tested_rf_0\t:\trf_under_test " << std::endl;

    string sizeGeneric = rfImpl_->sizeParameter();
    string widthGeneric = rfImpl_->widthParameter();
    if (!sizeGeneric.empty() || !widthGeneric.empty()) {
        declarationStream()
            << INDENT INDENT << "generic(" << std::endl;
        instantiationStream()
            << INDENT INDENT << "generic map (" << std::endl;
        if (!sizeGeneric.empty()) {
            declarationStream()
                << INDENT INDENT INDENT << sizeGeneric
                << "\t: integer := " << rfArch_->size();
            instantiationStream()
                << INDENT INDENT INDENT  << sizeGeneric << " => " 
                << rfArch_->size();
            if (!widthGeneric.empty()) {
                declarationStream() << ";" << std::endl;
                instantiationStream() << "," << std::endl;
            }
        }

        if (!widthGeneric.empty()) {
            declarationStream()
                << INDENT INDENT INDENT << widthGeneric
                << "\t: integer := " << rfArch_->width();
            instantiationStream()
                << INDENT INDENT INDENT << widthGeneric << " => " 
                << rfArch_->width();
        }
        declarationStream()
            << ");" << std::endl;
        instantiationStream()
            << ")" << std::endl;
    }

    declarationStream()
        << INDENT INDENT << "port(" << std::endl;
    instantiationStream()
        << INDENT INDENT << "port map (" << std::endl;
    
    const int portWidth = rfArch_->width();
    int opcodeWidth = opcodePortWidth();
    for (int i = 0; i < rfArch_->writePortCount(); i++) {
        // data port
         declarationStream() 
             << INDENT INDENT << inputPorts_.at(i) << "\t: "
             << "in std_logic_vector(" << portWidth
             << "-1 downto 0);" << std::endl;    
         // load port
         declarationStream() 
             << INDENT INDENT << inputLoadPorts_.at(i) << "\t: "
             << "in std_logic;" << std::endl;
         // opcode port
         declarationStream()
             << INDENT INDENT << inputOpcodePorts_.at(i) << "\t: "
             << "in std_logic_vector(" << opcodeWidth
             << "-1 downto 0);" << std::endl;

         signalStream()
            << INDENT << "signal " << inputPorts_.at(i) << "\t: "
            << "std_logic_vector(" << portWidth << "-1 downto 0);" 
            << std::endl;
         signalStream()
             << INDENT << "signal " << inputLoadPorts_.at(i)
             << "\t: std_logic_vector(1-1 downto 0);" << std::endl;
         signalStream()
             << INDENT << "signal " << inputOpcodePorts_.at(i) << "\t: "
             << "std_logic_vector(" << opcodeWidth << "-1 downto 0);"
             << std::endl;

         instantiationStream()
            << INDENT INDENT INDENT << inputPorts_.at(i) << " => " 
            << inputPorts_.at(i) << "," << std::endl;
         instantiationStream()
             << INDENT INDENT INDENT << inputLoadPorts_.at(i) << " => " 
             << inputLoadPorts_.at(i) << "(0)," << std::endl;
         instantiationStream()
             << INDENT INDENT INDENT << inputOpcodePorts_.at(i) << " => " 
             << inputOpcodePorts_.at(i) << "," << std::endl;
    }

    for (int i = 0; i < rfArch_->readPortCount(); i++) {
        declarationStream() 
             << INDENT INDENT << outputPorts_.at(i) << "\t: "
             << "out std_logic_vector(" << portWidth
             << "-1 downto 0);" << std::endl;    
         // load port
         declarationStream() 
             << INDENT INDENT << outputLoadPorts_.at(i) << "\t: "
             << "in std_logic;" << std::endl;
         // opcode port
         declarationStream()
             << INDENT INDENT << outputOpcodePorts_.at(i) << "\t: "
             << "in std_logic_vector(" << opcodeWidth
             << "-1 downto 0);" << std::endl;
         
         signalStream()
            << INDENT << "signal " << outputPorts_.at(i) << "\t: "
            << "std_logic_vector(" << portWidth << "-1 downto 0);" 
            << std::endl;
         signalStream()
             << INDENT << "signal " << outputLoadPorts_.at(i)
             << "\t: std_logic_vector(1-1 downto 0);" << std::endl;
         signalStream()
             << INDENT << "signal " << outputOpcodePorts_.at(i) << "\t: "
             << "std_logic_vector(" << opcodeWidth << "-1 downto 0);"
             << std::endl;

         instantiationStream()
            << INDENT INDENT INDENT << outputPorts_.at(i) << " => " 
            << outputPorts_.at(i) << "," << std::endl;
         instantiationStream()
             << INDENT INDENT INDENT << outputLoadPorts_.at(i) << " => "
             << outputLoadPorts_.at(i) << "(0)," << std::endl;
         instantiationStream()
             << INDENT INDENT INDENT << outputOpcodePorts_.at(i) << " => " 
             << outputOpcodePorts_.at(i) << "," << std::endl;  
    }
    instantiationStream() 
        << INDENT INDENT INDENT
        << rfImpl_->clkPort() << " => " << rfImpl_->clkPort() 
        << "," << std::endl
        << INDENT INDENT INDENT
        << rfImpl_->rstPort() << " => " << rfImpl_->rstPort() 
        << "," << std::endl
        << INDENT INDENT INDENT
        << rfImpl_->glockPort() << " => " << rfImpl_->glockPort() 
        << ");";

    declarationStream()
        << INDENT INDENT << rfImpl_->glockPort() << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << rfImpl_->rstPort() << "\t: in  std_logic;" 
        << std::endl
        << INDENT INDENT << rfImpl_->clkPort() << "\t: in  std_logic);" 
        << std::endl
        << INDENT << "end component;" 
        << std::endl;
    
    signalStream()
        << INDENT << "signal " << rfImpl_->glockPort() << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << rfImpl_->rstPort() << "\t: std_logic;" 
        << std::endl
        << INDENT << "signal " << rfImpl_->clkPort() << "\t: std_logic;" 
        << std::endl;
    
}


/**
 * Creates input and output data tables
 *
 * Creates input and output data and control signals as well for the 
 * testbench. Testbench writes to and reads from every register. If RF has
 * multiple write or read ports maximum number of ports is used on every 
 * cycle. Ports are written and read in round robin order.
 * Test is pipelined in such way that writing starts on the first cycle and
 * reading starts when all the registers can be read without stall cycles.
 */
void
RFTestbenchGenerator::createStimulus() {

    RegisterFileState& simRF = msm_->registerFileState(RF_NAME_);
    assert(&simRF != &NullRegisterFileState::instance());

    PortDataArray inputData;
    PortDataArray inputOpcode;
    PortDataArray inputLoad;

    PortDataArray outputData;
    PortDataArray outputOpcode;
    PortDataArray outputLoad;

    // initialize a random number generator for the stimuli
    boost::uniform_int<> distribution(INT_MIN, INT_MAX);
    boost::mt19937 rng;
    rng.seed(time(NULL));
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
        randomNumber(rng, distribution);
   
    // Number of cycles needed to write to all registers
    int fillCycles = 0;
    if (rfArch_->size() >= rfArch_->writePortCount()) {
        double size = rfArch_->size();
        double ports = rfArch_->writePortCount();
        fillCycles = static_cast<int>(ceil(size/ports));
    } else {
        // RF has more write ports than registers
        fillCycles = rfArch_->size();
    }
    // Number of cycles needed to read all registers (this includes flush 
    // cycles)
    int readCycles =
        static_cast<int>(ceil(rfArch_->size() / rfArch_->readPortCount()));
    
    // Number of cycles needed to wait before we can start reading from RF
    int outputWaitCycles = 0;
    // Number of cycles needed to read rest of the register after fill cycles
    int pipelineFlushCycles = 0;
    if (fillCycles > readCycles) {
        pipelineFlushCycles = rfArch_->latency();
        // total cycles - read cycles
        outputWaitCycles = fillCycles + pipelineFlushCycles - readCycles;
    } else {
        pipelineFlushCycles = rfArch_->latency() + (readCycles - fillCycles);
        outputWaitCycles = rfArch_->latency();
    }

    // port index and register where next input should be written to
    int wrPortIndex = 0;
    int wrRegIndex = 0;
    // port index and register where next output should be read from
    int rdPortIndex = 0;
    int rdRegIndex = 0;
    // port index and register where next output load is written to
    int rdLoadPortIndex = 0;
    int rdLoadRegIndex = 0;

    // Write data to registers and also start reading them when possible
    for (int i = 0; i < fillCycles; i++) {
        // Handle output first because register state is updated immediately
        if (i < outputWaitCycles) {
            for (int j = 0; j < rfArch_->readPortCount(); j++) {
                string portName = outputPorts_.at(j);
                outputData[portName].push_back(0);
            }
        } else {
            for (int j = 0; j < rfArch_->readPortCount(); j++) {
                string portName = outputPorts_.at(rdPortIndex);
                outputData[portName].push_back(
                    simRF.registerState(rdRegIndex).value().unsignedValue());
                // round robin regs and ports
                rdRegIndex++;
                rdRegIndex = rdRegIndex % rfArch_->size();
                rdPortIndex++;
                rdPortIndex = rdPortIndex % rfArch_->readPortCount();
            }
        }

        // write output load and opcode signals
        if ( i < (outputWaitCycles - rfArch_->latency())) {
            for (int j = 0; j < rfArch_->readPortCount(); j++) {
                string opcodePort = outputOpcodePorts_.at(j);
                string loadPort =  outputLoadPorts_.at(j);
                outputOpcode[opcodePort].push_back(0);
                outputLoad[loadPort].push_back(0);
            }
        } else {
           for (int j = 0; j < rfArch_->readPortCount(); j++) {
                string opcodePort = outputOpcodePorts_.at(rdLoadPortIndex);
                string loadPort =  outputLoadPorts_.at(rdLoadPortIndex);
                outputOpcode[opcodePort].push_back(rdLoadRegIndex);
                outputLoad[loadPort].push_back(1);
                // round robin regs and ports
                rdLoadRegIndex++;
                rdLoadRegIndex = rdLoadRegIndex % rfArch_->size();
                rdLoadPortIndex++;
                rdLoadPortIndex = rdLoadPortIndex % rfArch_->readPortCount();
           } 
        }
            
        
        // write inputs
        for (int j = 0; j < rfArch_->writePortCount(); j++) {
            int nopPortIndex = wrPortIndex;
            // write only to as many ports per cycle as possible
            // and write only once to every register
            if (j < rfArch_->maxWrites() && wrRegIndex < rfArch_->size()) {
                uint32_t stimulus = (uint32_t)randomNumber();
                const int portWidth = rfArch_->width();
                SimValue simStim(portWidth);
                stimulus = (stimulus << (32 - portWidth) >> (32 - portWidth));
                simStim = stimulus;
                simRF.registerState(wrRegIndex).setValue(simStim);

                string portName = inputPorts_.at(wrPortIndex);
                inputData[portName].push_back(stimulus);

                string opcodePort = inputOpcodePorts_.at(wrPortIndex);
                uint32_t opcode = static_cast<uint32_t>(wrRegIndex);
                inputOpcode[opcodePort].push_back(opcode);

                string loadPort = inputLoadPorts_.at(wrPortIndex);
                uint32_t load = 1;
                inputLoad[loadPort].push_back(load);

                wrRegIndex++;
                // use round robin for write ports
                wrPortIndex++;
                wrPortIndex = wrPortIndex % rfArch_->writePortCount();
            } else {
                // write nop to other ports
                uint32_t stimulus = 0;
                uint32_t opcode = 0;
                uint32_t load = 0;
                
                string portName = inputPorts_.at(nopPortIndex);
                inputData[portName].push_back(stimulus);
                
                string opcodePort = inputOpcodePorts_.at(nopPortIndex);
                inputOpcode[opcodePort].push_back(opcode);
                string loadPort = inputLoadPorts_.at(nopPortIndex);
                inputLoad[loadPort].push_back(load);

                // round robin
                wrPortIndex++;
                wrPortIndex = wrPortIndex % rfArch_->writePortCount();
            }
        }
    }

    // No more data to be written, read rest of the registers
    for (int i = 0; i < pipelineFlushCycles; i++) {
        for (int j = 0; j < rfArch_->writePortCount(); j++) {
            // write nop to write ports
            uint32_t stimulus = 0;
            uint32_t opcode = 0;
            uint32_t load = 0;
                
            string portName = inputPorts_.at(j);
            inputData[portName].push_back(stimulus);
                
            string opcodePort = inputOpcodePorts_.at(j);
            inputOpcode[opcodePort].push_back(opcode);
            string loadPort = inputLoadPorts_.at(j);
            inputLoad[loadPort].push_back(load);
        }
        // write output load signals
        for (int j = 0; j < rfArch_->readPortCount(); j++) {
            string opcodePort = outputOpcodePorts_.at(rdLoadPortIndex);
            string loadPort =  outputLoadPorts_.at(rdLoadPortIndex);
            outputOpcode[opcodePort].push_back(rdLoadRegIndex);
            outputLoad[loadPort].push_back(1);
            // round robin regs and ports
            rdLoadRegIndex++;
            rdLoadRegIndex = rdLoadRegIndex % rfArch_->size();
            rdLoadPortIndex++;
            rdLoadPortIndex = rdLoadPortIndex % rfArch_->readPortCount();
        } 
        // write output data, opcodes and loads
        for (int j = 0; j < rfArch_->readPortCount(); j++) {
            string portName = outputPorts_.at(rdPortIndex);
            outputData[portName].push_back(
                simRF.registerState(rdRegIndex).value().unsignedValue());
            // round robin regs and ports
            rdRegIndex++;
            rdRegIndex = rdRegIndex % rfArch_->size();
            rdPortIndex++;
            rdPortIndex = rdPortIndex % rfArch_->readPortCount();
        }
    }

    createStimulusArrays(inputData, inputOpcode, inputLoad, outputData,
                         outputOpcode, outputLoad);

    int totalCycleCount = fillCycles + pipelineFlushCycles;
    writeTbConstants(totalCycleCount, outputWaitCycles);
}

/**
 * Writes the testbench main process code
 */
void 
RFTestbenchGenerator::createTbCode() {
    
    // input ports
    for (int i = 0; i < rfArch_->writePortCount(); i++) {
        string portName = inputPorts_.at(i);
        string loadPort = inputLoadPorts_.at(i);
        string opcodePort = inputOpcodePorts_.at(i);
        
        tbCodeStream()
            << INDENT INDENT << portName << " <= " << portName << "_data("
            << "current_cycle);" << std::endl
            << INDENT INDENT << loadPort << " <= " << loadPort 
            <<"_data(current_cycle);" << std::endl
            << INDENT INDENT << opcodePort << " <= " << opcodePort 
            << "_data(current_cycle);" << std::endl;
    }
    // output port load and opcode signals
    for (int i = 0; i < rfArch_->readPortCount(); i++) {       
        string loadPort = outputLoadPorts_.at(i);
        string opcodePort = outputOpcodePorts_.at(i);

        tbCodeStream() 
            << INDENT INDENT << loadPort << " <= " << loadPort 
            <<"_data(current_cycle);" << std::endl
            << INDENT INDENT << opcodePort << " <= " << opcodePort 
            << "_data(current_cycle);" << std::endl;
    }
    // output ports
    tbCodeStream()
        << std::endl << std::endl << INDENT INDENT 
        << "if current_cycle >= IGNORE_OUTPUT_COUNT then" << std::endl;
    for (int i = 0; i < rfArch_->readPortCount(); i++) {
        string portName = outputPorts_.at(i);
        tbCodeStream() 
            << INDENT INDENT INDENT 
            << "assert " << portName << " = " << portName << "_data"
            << "(current_cycle)" << std::endl
            << INDENT INDENT INDENT INDENT 
            << "report lf & \"TCE Assert: Verification failed at cycle \" "
            << "& str(current_cycle, 10)" << std::endl
            << INDENT INDENT INDENT INDENT <<"& \" output: \" "
            << "& str(conv_integer(signed(" << portName << ")), 10)" 
            << std::endl
            << INDENT INDENT INDENT INDENT << "& " 
            << "\" expected: \" & str(conv_integer(signed(" << portName
            << "_data(current_cycle))), 10)  severity error;" 
            << std::endl << std::endl;
    }
    tbCodeStream() << INDENT INDENT << "end if;" << std::endl;
}

/**
 * Writes input, output and control signal data to output streams
 *
 * @param inputData Array containing input ports and their values
 * @param inputOpcode Array containing input port opcode ports and their 
 *                    values
 * @param inputLoad Array containing input port load ports and their values
 * @param outputData Array containing output ports and their values
 * @param outputOpcode Array containing output port opcode ports and their
 *                     values
 * @param outputLoad Array containing output port load ports and their values
 */
void 
RFTestbenchGenerator::createStimulusArrays(
    PortDataArray& inputData,
    PortDataArray& inputOpcode,
    PortDataArray& inputLoad,
    PortDataArray& outputData,
    PortDataArray& outputOpcode,
    PortDataArray& outputLoad) {

    int portWidth = rfArch_->width();
    writeDataArrays(inputArrayStream(), inputData, portWidth);
    writeDataArrays(outputArrayStream(), outputData, portWidth);

    int loadWidth = 1;
    writeDataArrays(loadArrayStream(), inputLoad, loadWidth);
    writeDataArrays(loadArrayStream(), outputLoad, loadWidth);

    int opcodeWidth = opcodePortWidth();
    writeDataArrays(opcodeArrayStream(), inputOpcode, opcodeWidth);
    writeDataArrays(opcodeArrayStream(), outputOpcode, opcodeWidth);
}

int 
RFTestbenchGenerator::opcodePortWidth() const {

    int width = 0;
    if (rfArch_->size() > 1) {
        unsigned int biggestIndex = rfArch_->size()-1;
        width = MathTools::requiredBits(biggestIndex);
    } else {
        width = 1;
    }
    return width;
}

/**
 * Write one PortDataArray to output stream
 *
 * @param stream Output stream
 * @param array Array to be written
 * @param portWidth Width of the output port
 */
void
RFTestbenchGenerator::writeDataArrays(
     std::ostringstream& stream,
     PortDataArray& array,
     int portWidth) {

    for (PortDataArray::iterator i = array.begin(); i != array.end(); i++) {
        string portName = i->first;
        vector<uint32_t> data = i->second;
        writeStimulusArray(stream, data, portName, portWidth);
    }
}
