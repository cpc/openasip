/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProGeTestBenchGenerator.hh
 *
 * Declaration of ProGeTestBenchGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_TEST_BENCH_GENERATOR_HH
#define TTA_PROGE_TEST_BANCH_GENERATOR_HH

#include "Machine.hh"
#include "Exception.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"

#include <string>

/**
 * Class for test bench generating objects.
 *
 * Base class for script generating.
 */
class ProGeTestBenchGenerator {
public:
    ProGeTestBenchGenerator();
    virtual ~ProGeTestBenchGenerator();

    void generate(
        const TTAMachine::Machine& mach,
        const IDF::MachineImplementation& implementation,
        const std::string& dstDirectory,
        const std::string& progeOutDir)
        throw (IOException, OutOfRange, InvalidName, InvalidData);

private:
    void copyTestBenchFiles(const std::string& dstDirectory);
    void createFile(const std::string& fileName)
        throw (IOException);
    std::string getSignalMapping(
        const std::string& fuName,
        const std::string& epName,
        bool widthIsOne,
        const std::string& memoryName,
        const std::string& memoryLine)
        throw (InvalidName);
    void createTBConstFile(
        std::string dstDirectory,
        const std::string dataWidth = "",
        const std::string addrWidth = "");
    void createProcArchVhdl(
        const std::string& dstDirectory,
        const std::string& topLevelVhdl,
        const std::string& signalMappings)
        throw (IOException);

};

#endif

