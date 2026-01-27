/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file VerificationDataGenerator.hh
 *
 * Implementation of VerificationDataGenerator class.
 *
 * Created on: 26.2.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef VERIFICATIONDATAGENERATOR_HH
#define VERIFICATIONDATAGENERATOR_HH

#include <fstream>

#include "Exception.hh"
#include "SimulatorFrontend.hh"
#include "BusTracker.hh"
#include "FileSystem.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

class TestCase;
class Path;
class BusTracker;
class SimulatorFrontend;


/*
 * Class that writes verification data from Programs.
 */
class VerificationDataGenerator {
public:
    VerificationDataGenerator(unsigned int timeOutMs = 1000);
    VerificationDataGenerator(
        const TTAMachine::Machine& machine,
        const Path& outputDir,
        unsigned int timeOutMs = 1000);
    virtual ~VerificationDataGenerator();
    void setMachine(const TTAMachine::Machine& machine);
    void setOutputDirectory(const Path& outputDir);
    virtual void generate(const TestCase& testCase);

protected:
    /// The simulator used to produce verification data.
    SimulatorFrontend simulator_;
    /// The path to directory where the verification data are placed.
    Path outputDir_;
    /// File stream for bus trace.
    std::ofstream busTraceStream_;
    /// Generates bus trace verification data.
    BusTracker busTracker_;

private:
    void reopen(std::ofstream& stream, const Path& file);
};

#endif /* VERIFICATIONDATAGENERATOR_HH */
