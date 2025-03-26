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
 * @file TestGeneratorBase.cc
 *
 * Implementation of TestGeneratorBase class.
 *
 * Created on: 25.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestGeneratorBase.hh"

#include <iostream>

/**
 * Main constructor.
 */
TestGeneratorBase::TestGeneratorBase(
    const std::string& generatortitle,
    const std::string& description,
    bool enabledByDefault)

    : generatortitle_(generatortitle),
      description_(description),
      machine_(NULL),
      implementation_(NULL),
      enabled_(enabledByDefault) {
}

/**
 * Destructor.
 */
TestGeneratorBase::~TestGeneratorBase() {
}


const std::string&
TestGeneratorBase::title() const {
    return generatortitle_;
}


const std::string&
TestGeneratorBase::description() const {
    return description_;
}


const TTAMachine::Machine&
TestGeneratorBase::machine() const {
    return *machine_;
}


void
TestGeneratorBase::setMachine(const TTAMachine::Machine& machine) {
    machine_ = &machine;
}

const IDF::MachineImplementation&
TestGeneratorBase::implementation() const {
    return *implementation_;
}


void
TestGeneratorBase::setImplementation(
    const IDF::MachineImplementation& implementation) {
    implementation_ = &implementation;
}


bool
TestGeneratorBase::isEnabled() const {
    return enabled_;
}


void
TestGeneratorBase::setEnabled(bool setting) {
    enabled_ = setting;
}


std::vector<TestCase>
TestGeneratorBase::generateTestCases(
    RandomNumberGenerator::SeedType seed) {

    if (initializedWithAdf()) {
        return generateTestCasesImpl(seed);
    } else {
        return std::vector<TestCase>(); // Return empty.
    }
}


bool
TestGeneratorBase::initializedWithAdf() const {
    return machine_ != NULL;
}


bool
TestGeneratorBase::initializedWithIdf() const {
    return implementation_ != NULL;
}

