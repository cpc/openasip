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
 * @file TestGeneratorBase.hh
 *
 * Declaration of TestGeneratorBase class.
 *
 * Created on: 25.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTGENERATORBASE_HH
#define TESTGENERATORBASE_HH

#include <vector>
#include <string>

#include "Application.hh"
#include "RandomNumberGenerator.hh"
#include "TestCase.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
    class CodeSnippet;
}

namespace IDF {
    class MachineImplementation;
}

class TestGeneratorBase {
public:

    TestGeneratorBase(
        const std::string& generatortitle,
        const std::string& description,
        bool enabledByDefault = true);
    virtual ~TestGeneratorBase();

    const std::string& title() const;
    const std::string& description() const;
    const TTAMachine::Machine& machine() const;
    void setMachine(const TTAMachine::Machine& machine);
    const IDF::MachineImplementation& implementation() const;
    void setImplementation(const IDF::MachineImplementation& implementation);

    bool isEnabled() const;
    void setEnabled(bool setting = true);

    std::vector<TestCase> generateTestCases(
        RandomNumberGenerator::SeedType seed =
            RandomNumberGenerator::DEFAULTSEED);

protected:
    bool initializedWithAdf() const;
    bool initializedWithIdf() const;

    virtual std::vector<TestCase> generateTestCasesImpl(
        RandomNumberGenerator::SeedType seed =
            RandomNumberGenerator::DEFAULTSEED) =0;

private:
    /// The title of test generator serving as identification.
    const std::string generatortitle_;
    /// The description of test generator.
    const std::string description_;
    /// The architecture for which the test cases are to be generated.
    const TTAMachine::Machine* machine_;
    /// The optional implementation of the architecture. NULL if not defined.
    const IDF::MachineImplementation* implementation_;

    bool enabled_;
};

#endif /* TESTGENERATORBASE_HH */
