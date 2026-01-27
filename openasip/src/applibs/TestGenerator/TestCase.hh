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
 * @file TestCase.hh
 *
 * Declaration of TestCase class.
 *
 * Created on: 26.2.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTCASE_HH
#define TESTCASE_HH

#include <string>

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class AddressSpace;
}

/*
 * The class representing a test case.
 */
class TestCase {
public:
    TestCase();
    TestCase(
        TTAProgram::Program* program,
        const std::string& testname = std::string(),
        bool generateVerificationData = true);
    TestCase(
        const TTAMachine::AddressSpace& space,
        const std::string& testname = std::string());
    virtual ~TestCase();

    void setProgram(TTAProgram::Program* program);
    TTAProgram::Program* program();
    const TTAProgram::Program* program() const;
    void setTestName(const std::string& newName);
    const std::string& testName() const;
    bool generateVerificationData() const { return generateVerificationData_; }

private:
    TTAProgram::Program* program_;
    std::string testname_;
    /// The flag to tell if verification data should be generated. This mean
    /// the data has not been created or it is not wanted.
    bool generateVerificationData_ = true;
};

#endif /* TESTCASE_HH */
