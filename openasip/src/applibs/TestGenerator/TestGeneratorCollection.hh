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
 * @file TestGeneratorCollection.hh
 *
 * Declaration of TestGeneratorCollection class.
 *
 * Created on: 11.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTGENERATORCOLLECTION_HH
#define TESTGENERATORCOLLECTION_HH

#include <iostream>
#include <map>
#include <vector>

#include "TestGeneratorBase.hh"
#include "TestCase.hh"
#include "RandomNumberGenerator.hh"

namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

/**
 * The container class for test generators.
 */
class TestGeneratorCollection {
public:
    typedef std::string GeneratorKey; // Id string from test generators' as key
    typedef std::map<GeneratorKey, TestGeneratorBase*> GeneratorMap;

    TestGeneratorCollection();
    virtual ~TestGeneratorCollection();

    void setMachine(const TTAMachine::Machine& targetMachine);
    void setImplementation(const IDF::MachineImplementation& implementation);
    std::vector<TestCase> generateTestCases(
        RandomNumberGenerator::SeedType seed =
            RandomNumberGenerator::DEFAULTSEED);
    const GeneratorMap& getGenerators() const;
    void setEnabled(const GeneratorKey& generatorKey, bool setting = true);

private:
    void loadDefaultTestGenerators(GeneratorMap& generatorsOut);
    static void setMachine(
        const TTAMachine::Machine& targetMachine,
        GeneratorMap& generators);
    static void setImplementation(
        const IDF::MachineImplementation& implementation,
        GeneratorMap& generators);
    static std::pair<GeneratorKey, TestGeneratorBase*> create_pair(
        TestGeneratorBase* newGenerator);
    static void deleteTestGenerators(GeneratorMap& generators);

    /// The target architecture for the tests.
    const TTAMachine::Machine* machine_;
    /// The implementation of the architecture.
    const IDF::MachineImplementation* implementation_;
    /// The testGenerator container.
    GeneratorMap testGenerators_;
};

#endif /* TESTGENERATORCOLLECTION_HH */
