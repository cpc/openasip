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
 * @file RFTestbenchGenerator.hh
 *
 * Declaration of RFTestbenchGenerator class
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_TESTBENCH_GENERATOR_HH
#define TTA_RF_TESTBENCH_GENERATOR_HH

#include <string>
#include <fstream>
#include <vector>
#include "TestbenchGenerator.hh"
#include "HDBManager.hh"
#include "RFEntry.hh"
#include "MemorySystem.hh"

class MachineState;

namespace TTAMachine {
    class RegisterFile;
    class Machine;
}

class RFTestbenchGenerator : public TestbenchGenerator {
public:
    RFTestbenchGenerator(HDB::RFEntry* rf);

    virtual ~RFTestbenchGenerator();

    virtual void generateTestbench(std::ofstream& file);

private:
    void createMachineState();

    void parseRfPorts();

    void createTbInstantiation();    

    void createStimulus();

    void createTbCode();

    void 
    createStimulusArrays(
        PortDataArray& inputData,
        PortDataArray& inputOpcode,
        PortDataArray& inputLoad,
        PortDataArray& outputData,
        PortDataArray& outputOpcode,
        PortDataArray& outputLoad);

    void
    writeDataArrays(
     std::ostringstream& stream,
     PortDataArray& array,
     int portWidth);

    int opcodePortWidth() const;

    HDB::RFEntry* rfEntry_;
    HDB::RFImplementation* rfImpl_;
    HDB::RFArchitecture* rfArch_;
    TTAMachine::RegisterFile* machRf_;

    MachineState* msm_;
    TTAMachine::Machine* machine_;
    MemorySystem* memSystem_;

    std::vector<std::string> inputPorts_;
    std::vector<std::string> inputLoadPorts_;
    std::vector<std::string> inputOpcodePorts_;
    std::vector<std::string> outputPorts_;
    std::vector<std::string> outputLoadPorts_;
    std::vector<std::string> outputOpcodePorts_;

    static const std::string RF_NAME_;
};

#endif
