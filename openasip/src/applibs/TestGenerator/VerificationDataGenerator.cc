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
 * @file VerificationDataGenerator.cc
 *
 * Implementation of VerificationDataGenerator class.
 *
 * Created on: 26.2.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "VerificationDataGenerator.hh"
#include "BusTracker.hh"

#include "Machine.hh"
#include "Program.hh"
#include "TestCase.hh"

VerificationDataGenerator::VerificationDataGenerator(
    unsigned int timeOutMs)
    : simulator_(),
      outputDir_(""),
      busTraceStream_(),
      busTracker_(simulator_, busTraceStream_) {

    simulator_.setTimeout(timeOutMs);
}

/**
 * Constructor.
 */
VerificationDataGenerator::VerificationDataGenerator(
    const TTAMachine::Machine& machine,
    const Path& outputDir,
    unsigned int timeOutMs)
    : simulator_(),
      outputDir_(outputDir),
      busTraceStream_(),
      busTracker_(simulator_, busTraceStream_) {

    simulator_.loadMachine(machine);
    simulator_.setTimeout(timeOutMs);
}

VerificationDataGenerator::~VerificationDataGenerator() {
}

/**
 * Writes verification data for given program.
 *
 * Verification data includes only bus trace currently.
 *
 * @param TestCase Uses the program in simulation and testname as prefix in
 *                 verification data files (e.g. {testname}.bustrace)
 * @exception IOException If opening files to write fails.
 * @exception SimulationException If error occurs in the simulator.
 */
void
VerificationDataGenerator::generate(const TestCase& testCase) {
    Path busTraceFile(outputDir_);
    busTraceFile /= testCase.testName() + ".bustrace";
    Path cycleCountFile(outputDir_);
    cycleCountFile /= testCase.testName() + ".cyclecount";
    std::ofstream cycleCountStream;
    reopen(busTraceStream_, busTraceFile);
    reopen(cycleCountStream, cycleCountFile);
    if (simulator_.isSimulationInitialized()
        && !simulator_.hasSimulationEnded()) {
        simulator_.killSimulation();
    }
    try {
        simulator_.loadProgram(*testCase.program());
    } catch (Exception& e) {
        if (busTraceStream_.is_open()) {
            busTraceStream_.close();
        }
        if (FileSystem::fileExists(busTraceFile.string())) {
            FileSystem::removeFileOrDirectory(busTraceFile.string());
        }
        THROW_EXCEPTION(IOException, e.errorMessage());
    }
    try {
        simulator_.run();
        cycleCountStream << simulator_.cycleCount() << std::endl;
    } catch (Exception& e) {
        if (busTraceStream_.is_open()) {
            busTraceStream_.close();
        }
        if (FileSystem::fileExists(busTraceFile.string())) {
            FileSystem::removeFileOrDirectory(busTraceFile.string());
        }
        THROW_EXCEPTION(SimulationException, e.errorMessage());
    }

    if (!simulator_.hasSimulationEnded()) {
        // Todo message simulation failure or throw exception.
    }

    cycleCountStream.close();
}

void
VerificationDataGenerator::setMachine(const TTAMachine::Machine& machine) {
    simulator_.loadMachine(machine);
}

void
VerificationDataGenerator::setOutputDirectory(const Path& outputDir) {
    outputDir_ = outputDir;
}

/**
 * Reopens file stream to given file of which data will be overwritten.
 *
 * @exception IOException If opening file to write fails.
 */
void
VerificationDataGenerator::reopen(std::ofstream& stream, const Path& file) {
    if (stream.is_open()) {
        stream.close();
    }
    stream.clear();
    stream.open(file.string().c_str());
    if (!stream.is_open()) {
        throw IOException(__FILE__, __LINE__, __func__,
            std::string("Unable to write to file ") + file.string().c_str());
    }
}
