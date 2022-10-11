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
 * @file ProGeContext.cc
 *
 * Implementation of ProGeContext class.
 *
 * Created on: 9.6.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ProGeContext.hh"

#include "GlobalPackage.hh"

namespace ProGe {

ProGeContext::ProGeContext(
    const TTAMachine::Machine& adf, const IDF::MachineImplementation& idf,
    const std::string& progeOutputDirectory,
    const std::string& sharedOutputDirectory,
    const std::string& coreEntityName, HDL targetHDL,
    int /*imemWidthInMAUs */)
    : adf_(adf),
      idf_(idf),
      progeOutputDirectory_(progeOutputDirectory),
      sharedOutputDirectory_(sharedOutputDirectory),
      entityName_(coreEntityName),
      hdl_(targetHDL),
      globalPackage_(new GlobalPackage(coreEntityName)) {}

ProGeContext::~ProGeContext() {}

const TTAMachine::Machine&
ProGeContext::adf() const {
    return adf_;
}

const IDF::MachineImplementation&
ProGeContext::idf() const {
    return idf_;
}

const std::string&
ProGeContext::outputDirectory() const {
    return progeOutputDirectory_;
}

const std::string&
ProGeContext::sharedDeirectory() const {
    return sharedOutputDirectory_;
}

const std::string&
ProGeContext::coreEntityName() const {
    return entityName_;
}

HDL
ProGeContext::targetHDL() const {
    return hdl_;
}

const GlobalPackage&
ProGeContext::globalPackage() const {
    return *globalPackage_;
}

} /* namespace ProGe */
