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
 * @file TestbenchGenerator.hh
 *
 * Declaration of TestbenchGenerator class
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TESTBENCH_GENERATOR_HH
#define TTA_TESTBENCH_GENERATOR_HH

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "HWBlockImplementation.hh"

class TestbenchGenerator {
public:
    TestbenchGenerator();

    virtual ~TestbenchGenerator();

    virtual void generateTestbench(std::ofstream& file) = 0;

protected:
    typedef std::map<std::string, std::vector<uint32_t> > PortDataArray;

    virtual void writeStimulusArray(
        std::ostringstream& stream, 
        std::vector<uint32_t>& dataArray, 
        std::string portName,
        int portWidth);

    void writeTbConstants(int totalCycles, int outputIgnoreCycles);

    void writeTestbench(
        std::ofstream& file, HDB::HWBlockImplementation* impl);

    std::ostringstream& declarationStream();
    std::ostringstream& bindingStream();
    std::ostringstream& signalStream();
    std::ostringstream& instantiationStream();
    std::ostringstream& inputArrayStream();
    std::ostringstream& opcodeArrayStream();
    std::ostringstream& loadArrayStream();
    std::ostringstream& outputArrayStream();
    std::ostringstream& tbCodeStream();

private:
    std::string findVhdlTemplate() const;

    void loadVhdlTemplate(
        const std::string& fileName, std::string& vhdlTemplate) const;

    std::ostringstream componentDeclaration_;
    std::ostringstream componentBinding_;
    std::ostringstream signalDeclaration_;
    std::ostringstream componentInstantiation_;
    std::ostringstream inputArrays_;
    std::ostringstream opcodeArrays_;
    std::ostringstream loadSignalArrays_;
    std::ostringstream outputArrays_;
    std::ostringstream testbenchCode_;

    static const std::string TB_TEMPLATE_;
};

#endif
