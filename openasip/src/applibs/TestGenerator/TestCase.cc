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
 * @file TestCase.cc
 *
 * Implementation of TestCase class.
 *
 * Created on: 26.2.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestCase.hh"

#include "AddressSpace.hh"
#include "Program.hh"
#include "Procedure.hh"

/**
 * Constructs an empty test case.
 */
TestCase::TestCase()
    : program_(NULL),
      testname_(std::string()) {
}

/**
 * Constructs a test case using given program as test case.
 *
 * @param program The test case program
 * @param testname The name of the test case
 * @param generateVerificationData By default verification data is generated
 *                                 automatically. If set to false this process
 *                                 should be skipped.
 * Takes ownership of the program.
 */
TestCase::TestCase(
    TTAProgram::Program* program,
    const std::string& testname,
    bool generateVerificationData)
    : program_(program),
      testname_(testname),
      generateVerificationData_(generateVerificationData) {
}

/**
 * Constructs a test case with preassigned program.
 */
TestCase::TestCase(
    const TTAMachine::AddressSpace& space,
    const std::string& testname)
    : program_(new TTAProgram::Program(space)),
      testname_(testname) {

    program_->addProcedure(new TTAProgram::Procedure("main", space));
}

/**
 * Destructor.
 */
TestCase::~TestCase() {
}

void TestCase::setProgram(TTAProgram::Program* program) {
    program_ = program;
}

TTAProgram::Program* TestCase::program() {
    return program_;
}

const TTAProgram::Program* TestCase::program() const {
    return program_;
}

void TestCase::setTestName(const std::string& newName) {
    testname_ = newName;
}

const std::string& TestCase::testName() const {
    return testname_;
}
