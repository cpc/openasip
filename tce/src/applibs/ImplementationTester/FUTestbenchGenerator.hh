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
 * @file FUTestbenchGenerator.hh
 *
 * Declaration of FUTestbenchGenerator class
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_TESTBENCH_GENERATOR_HH
#define TTA_FU_TESTBENCH_GENERATOR_HH

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "HDBManager.hh"
#include "FUEntry.hh"
#include "MachineState.hh"
#include "Machine.hh"
#include "MemorySystem.hh"
#include "GlobalLock.hh"

class FUTestbenchGenerator {
public:
    FUTestbenchGenerator(HDB::FUEntry* fu);

    ~FUTestbenchGenerator();

    void generateTestbench(std::ofstream& file);

private:

    void parseFuPorts();

    void createMachineState();

    void createTbInstantiation();

    void createStimulus();

    void createTbCode();

    void writeTestbench(std::ofstream& file);

    void
    loadVHDLTemplate(const std::string& fileName, std::string& vhdlTemplate);

    typedef std::map<std::string, std::vector<uint32_t> > PortDataArray;

    void
    writeInputPortStimulus(
        PortDataArray& inputs,
        const std::string& operation,
        const std::string& portName, uint32_t stimulus);

    void readValuesFromOutPorts(PortDataArray& outputs);

    void 
    createStimulusArrays(
        PortDataArray& inputStimulus, 
        std::vector<uint32_t>& loadStimulus,
        std::vector<std::string>& operations, PortDataArray& outputStimulus);
        

    std::string findFuTemplate();

    HDB::FUEntry* fuEntry_;
    HDB::FUImplementation* fuImpl_;
    HDB::FUArchitecture* fuArch_;

    MachineState* msm_;
    std::vector<std::string> inputPorts_;
    std::vector<std::string> outputPorts_;
    std::string opcodePort_;

    TTAMachine::Machine* machine_;
    MemorySystem* memSystem_;
    GlobalLock* glock_;

    std::ostringstream componentDeclaration_;
    std::ostringstream componentBinding_;
    std::ostringstream registerSpecifications_;
    std::ostringstream componentInstantiation_;
    std::ostringstream stimulusArrays_;
    std::ostringstream opcodeArray_;
    std::ostringstream loadSignalArrays_;
    std::ostringstream expectedOutputs_;
    std::ostringstream testBenchCode_;

    static const std::string FU_TB_TEMPLATE_;
};

#endif
