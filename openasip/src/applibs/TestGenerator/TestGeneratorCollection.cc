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
 * @file TestGeneratorCollection.cc
 *
 * Implementation of TestGeneratorCollection class.
 *
 * Created on: 11.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestGeneratorCollection.hh"

#include <algorithm>
#include <sstream>

#include "MapTools.hh"
#include "TCEString.hh"

#include "ImmediateTestGenerator.hh"
#include "FUOperationTestGenerator.hh"
#include "ICacheTestGenerator.hh"
#include "GuardTestGenerator.hh"


TestGeneratorCollection::~TestGeneratorCollection() {
    deleteTestGenerators(testGenerators_);
}


TestGeneratorCollection::TestGeneratorCollection()
    : machine_(NULL),
      implementation_(NULL) {

    loadDefaultTestGenerators(testGenerators_);
}

void
TestGeneratorCollection::setMachine(const TTAMachine::Machine& targetMachine) {
    deleteTestGenerators(testGenerators_);
    machine_ = &targetMachine;
    loadDefaultTestGenerators(testGenerators_);
    setMachine(targetMachine, testGenerators_);
}

void
TestGeneratorCollection::setImplementation(
    const IDF::MachineImplementation& implementation) {
    implementation_ = &implementation;
    setImplementation(implementation, testGenerators_);
}

std::vector<TestCase>
TestGeneratorCollection::generateTestCases(
    RandomNumberGenerator::SeedType seed) {
    std::vector<TestCase> testcases;
    GeneratorMap::iterator genIt;
    for (auto generator : testGenerators_) {
        if (generator.second->isEnabled()) {
            std::vector<TestCase> newSetOfTestCases =
                generator.second->generateTestCases(seed);
            // Todo: test case name conflict check here?
            testcases.insert(testcases.end(),
                newSetOfTestCases.begin(), newSetOfTestCases.end());
        }
    }
    return testcases;
}


const TestGeneratorCollection::GeneratorMap&
TestGeneratorCollection::getGenerators() const {
    return testGenerators_;
}


void
TestGeneratorCollection::setEnabled(
    const GeneratorKey& generatorKey, bool setting) {

    if (testGenerators_.count(generatorKey) == 0) {
        return;
    } else {
        testGenerators_[generatorKey]->setEnabled(setting);
    }
}


void
TestGeneratorCollection::loadDefaultTestGenerators(
    GeneratorMap& generatorsOut) {

    std::insert_iterator<GeneratorMap> containerInserter =
           std::inserter(generatorsOut, generatorsOut.end());

    containerInserter = create_pair(new ImmediateTestGenerator());
    containerInserter = create_pair(new FUOperationTestGenerator());
    containerInserter = create_pair(new ICacheTestGenerator());
    containerInserter = create_pair(new GuardTestGenerator());
}


void
TestGeneratorCollection::setMachine(
    const TTAMachine::Machine& targetMachine,
    GeneratorMap& generators) {

    GeneratorMap::iterator it;
    for (it = generators.begin(); it != generators.end(); it++) {
        it->second->setMachine(targetMachine);
    }
}

void
TestGeneratorCollection::setImplementation(
    const IDF::MachineImplementation& implementation,
    GeneratorMap& generators) {

    GeneratorMap::iterator it;
    for (it = generators.begin(); it != generators.end(); it++) {
        it->second->setImplementation(implementation);
    }
}

std::pair<TestGeneratorCollection::GeneratorKey, TestGeneratorBase*>
TestGeneratorCollection::create_pair(TestGeneratorBase* newGenerator) {
    return std::make_pair(newGenerator->title(), newGenerator);
}


void
TestGeneratorCollection::deleteTestGenerators(GeneratorMap& generators) {
    while (!generators.empty()) {
        GeneratorMap::iterator it = generators.begin();
        TestGeneratorBase* to_remove = it->second;
        it->second = NULL;
        generators.erase(it);
        delete to_remove;
    }
}
