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
 * @file TestFileWriter.cc
 *
 * Implementation of TestFileWriter class.
 *
 * Created on: 12.3.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestFileWriter.hh"

#include <fstream>

#include "Machine.hh"
#include "VerificationDataGenerator.hh"
#include "ProgramImageGenerator.hh"
#include "BEMGenerator.hh"
#include "Program.hh"
#include "ProgramWriter.hh"
#include "TestCase.hh"
#include "BinaryEncoding.hh"
#include "ControlUnit.hh"
#include "POMDisassembler.hh"
#include "Environment.hh"

/**
 * Constructor.
 */
TestFileWriter::TestFileWriter()
    : outputDirectory_(""),
      testCase_(NULL),
      machine_(NULL),
      pig_(),
      imageFormat_(ProgramImageGenerator::ASCII),
      bem_(NULL) {
}

/**
 * Destructor.
 */
TestFileWriter::~TestFileWriter() {
}

/**
 * Sets output directory for services.
 */
void
TestFileWriter::setOutput(const Path& directory) {
    outputDirectory_ = directory;
    verifGen_.setOutputDirectory(outputDirectory_);
    return;
}

/**
 * Sets test case to be processed.
 */
void
TestFileWriter::setTestCase(const TestCase& testCase) {
    testCase_ = &testCase;
    return;
}

/**
 * Sets the architecture.
 *
 * Needed by makeVerificationData() function.
 */
void
TestFileWriter::setMachine(const TTAMachine::Machine& machine) {
    machine_ = &machine;
    verifGen_.setMachine(*machine_);
    BEMGenerator bemGenerator(*machine_);
    if (bem_ != NULL) {
        delete bem_; bem_ = NULL;
    }
    bem_ = bemGenerator.generate();
    pig_.loadBEM(*bem_);
    pig_.loadMachine(*machine_);
    return;
}

/**
 * Sets image format of the program and data by reading string description.
 */
void
TestFileWriter::setImageFormat(ProgramImageGenerator::OutputFormat format) {
    imageFormat_ = format;
    return;
}

/**
 * Sets data memory width in MAUs.
 */
void
TestFileWriter::setDataMemoryWidthInMAUs(int widthInMAUs) {
    dataMemoryWidthInMAUs_ = widthInMAUs;
    return;
}

/**
 * Writes verification data of current TestCase.
 *
 * setTestCase() and setMachine() must be called before this.
 */
void
TestFileWriter::makeVerificationData() {
    assert(testCase_ != NULL && "Unset test case.");
    assert(machine_ != NULL && "Unset machine.");

    verifGen_.generate(*testCase_);
    return;
}

/**
 * Writes program and data images from current TestCase.
 *
 * setTestCase() and setMachine() must be called before this.
 */
void
TestFileWriter::makeProgramImages() {
    assert(testCase_ != NULL && "Unset test case.");
    assert(machine_ != NULL && "Unset machine.");
    using TTAProgram::ProgramWriter;
    using TTAMachine::Machine;

    stringCRef testname = testCase_->testName();
    ProgramImageGenerator::TPEFMap tpefMap;
    tpefMap[testname] = ProgramWriter::createBinary(*testCase_->program());
    pig_.loadPrograms(tpefMap);

    // Generate program image //
    Path filePath = outputDirectory_;
    filePath /= (testname + formatExtension(imageFormat_));
    std::ofstream imageStream(filePath.string().c_str());
    pig_.generateProgramImage(testname, imageStream, imageFormat_);
    imageStream.close();

    // Generate Data Images //
    Machine::AddressSpaceNavigator asNav = machine_->addressSpaceNavigator();
    for (int i = 0; i < asNav.count(); i++) {
        std::string asName = asNav.item(i)->name();
        if (machine_->controlUnit()->addressSpace()->name() == asName) {
            continue; // Skip instruction memory address space.
        }

        filePath = outputDirectory_;
        filePath /= (testname + "_" + asName + formatExtension(imageFormat_));
        imageStream.open(filePath.string().c_str());
        pig_.generateDataImage(
            testname,
            *tpefMap[testname],
            asName, imageStream,
            imageFormat_,
            dataMemoryWidthInMAUs_,
            false);
        imageStream.close();
    }
    return;
}

/**
 * Writes disassembly of current TestCase.
 */
void
TestFileWriter::makeDisassembly() {
    assert(testCase_ != NULL && "Unset test case.");

    Path filePath = outputDirectory_;
    filePath /= (testCase_->testName() + ".tceasm");
    std::ofstream disasmFile(filePath.string().c_str());
    disasmFile << POMDisassembler::disassembleFully(
        *testCase_->program(), true);
    disasmFile.close();

    return;
}

/**
 * Writes TPEF of current TestCase.
 */
void
TestFileWriter::makeTPEF() {
    Path filePath = outputDirectory_;
    filePath /= (testCase_->testName() + ".tpef");
    TTAProgram::Program::writeToTPEF(*testCase_->program(), filePath);
}

/**
 * Writes testrunner script that runs the generated test cases in RTL-simulator.
 *
 * @param altDirectory Overrides the target output directory set by setOutput().
 */
void 
TestFileWriter::makeTestRunnerScript(const Path altDirectory) {

    Path src = Path(Environment::dataDirPath("TeGe"));
    src /= std::string("testrunner.py");
    Path dst;
    if (altDirectory.empty()) {
        dst = outputDirectory_;
    } else {
        dst = altDirectory;
    }
    dst /= src.filename();
    FileSystem::copy(src.string(), dst.string());
}

/**
 * Returns file extension string for given output image format.
 */
std::string
TestFileWriter::formatExtension(ProgramImageGenerator::OutputFormat format) {
    std::string extension;
    switch(format) {
        case ProgramImageGenerator::BINARY:
        case ProgramImageGenerator::ASCII:
        case ProgramImageGenerator::ARRAY:
            extension = ".img";
            break;
        case ProgramImageGenerator::MIF:
            extension = ".mif";
            break;
        case ProgramImageGenerator::VHDL:
            extension = "_pkg.vhdl";
            break;
        case ProgramImageGenerator::COE:
            extension = ".coe";
            break;
        default:
            assert(false && "Unknown image format.");
            break;
    }
    return extension;
}

