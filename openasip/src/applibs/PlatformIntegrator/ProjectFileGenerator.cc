/*
    Copyright (c) 2002-2010 Tampere University.

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
/**
 * @file ProjectFileGenerator.cc
 *
 * Implementation of ProjectFileGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <iostream>
#include "ProjectFileGenerator.hh"
#include "PlatformIntegrator.hh"
#include "StringTools.hh"
using PlatInt::SignalMapping;

ProjectFileGenerator::ProjectFileGenerator(
    TCEString coreEntity,
    const PlatformIntegrator* integrator):
    coreEntity_(coreEntity), integrator_(integrator) {
}


ProjectFileGenerator::~ProjectFileGenerator() {

    for (unsigned int i = 0; i < signalMap_.size(); i++) {
        delete signalMap_.at(i);
    }
}


void
ProjectFileGenerator::addHdlFile(const TCEString& file) {

    hdlFiles_.push_back(file);
}


void
ProjectFileGenerator::addHdlFiles(const std::vector<TCEString>& files) {

    for (unsigned int i = 0; i < files.size(); i++) {
        hdlFiles_.push_back(files.at(i));
    }
}


void
ProjectFileGenerator::addMemInitFile(const TCEString& memInit) {
    
    memInitFiles_.push_back(memInit);
}


void
ProjectFileGenerator::addSignalMapping(const SignalMapping& mapping) {
    
    SignalMapping* signalMap = new SignalMapping(mapping);
    signalMap_.push_back(signalMap);
}


const std::vector<TCEString>&
ProjectFileGenerator::hdlFileList() const {

    return hdlFiles_;
}


const std::vector<TCEString>&
ProjectFileGenerator::memInitFileList() const {

    return memInitFiles_;
}


const PlatformIntegrator*
ProjectFileGenerator::integrator() const {
    return integrator_;
}

int
ProjectFileGenerator::signalMappingCount() const {

    return signalMap_.size();
}


const SignalMapping*
ProjectFileGenerator::signalMapping(int index) const {

    return signalMap_.at(index);
}


TCEString
ProjectFileGenerator::extractFUName(
    const TCEString& port,
    const TCEString& delimiter) const {
    
    TCEString::size_type pos = port.find(delimiter);
    if (pos == TCEString::npos || pos == 0) {
        return port;
    }

    TCEString fuName = port.substr(0, pos);
    return StringTools::trim(fuName);
}
