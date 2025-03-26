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
 * @file TestFileWriter.hh
 *
 * Declaration of TestFileWriter class.
 *
 * Created on: 12.3.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTFILEWRITER_HH
#define TESTFILEWRITER_HH

#include "TCEString.hh"
#include "VerificationDataGenerator.hh"
#include "ProgramImageGenerator.hh"
#include "FileSystem.hh"

namespace TTAMachine {
    class Machine;
}

class TestCase;

/*
 * Services to write various automated test generator related files.
 */
class TestFileWriter {
public:
    TestFileWriter();
    virtual ~TestFileWriter();

    void setOutput(const Path& directory);
    void setTestCase(const TestCase& testCase);
    void setMachine(const TTAMachine::Machine& machine);
    void setImageFormat(ProgramImageGenerator::OutputFormat format);
    void setDataMemoryWidthInMAUs(int widthInMAUs);

    void makeVerificationData();
    void makeProgramImages();
    void makeDisassembly();
    void makeTPEF();
    void makeTestRunnerScript(const Path altDirectory = Path());

private:

    std::string formatExtension(ProgramImageGenerator::OutputFormat format);

    /// Output directory for test files.
    Path outputDirectory_;
    /// The current test case to process.
    const TestCase* testCase_;
    /// The current target architecture.
    const TTAMachine::Machine* machine_;
    /// The verification data generator.
    VerificationDataGenerator verifGen_;
    /// The program image generator.
    ProgramImageGenerator pig_;
    /// The output image format of the program and data.
    ProgramImageGenerator::OutputFormat imageFormat_;
    /// Binary encoding for The program image generator.
    BinaryEncoding* bem_;
    /// Data memory width in MAUs.
    int dataMemoryWidthInMAUs_;
};

#endif /* TESTFILEWRITER_HH */
