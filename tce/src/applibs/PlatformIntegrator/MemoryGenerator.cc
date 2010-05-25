/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file MemoryGenerator.cc
 *
 * Implementation of MemoryGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include "MemoryGenerator.hh"
#include "PlatformIntegrator.hh"
using std::string;
using std::vector;


MemoryGenerator::MemoryGenerator(
    int mauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream):
    mauWidth_(mauWidth), widthInMaus_(widthInMaus), addrWidth_(addrWidth),
    initFile_(initFile), integrator_(integrator),
    warningStream_(warningStream), errorStream_(errorStream) {

}

MemoryGenerator::~MemoryGenerator() {
}

int
MemoryGenerator::memoryTotalWidth() const {

    return mauWidth_*widthInMaus_;
}
    
int 
MemoryGenerator::memoryMauSize() const {

    return mauWidth_;
}

int 
MemoryGenerator::memoryWidthInMaus() const {

    return widthInMaus_;
}


int
MemoryGenerator::memoryAddrWidth() const {

    return addrWidth_;
}


std::string 
MemoryGenerator::initializationFile() const {
    
    return initFile_;
}

const PlatformIntegrator*
MemoryGenerator::platformIntegrator() const {

    return integrator_;
}

std::ostream&
MemoryGenerator::warningStream() {

    return warningStream_;
}

std::ostream&
MemoryGenerator::errorStream() {

    return errorStream_;
}


int
MemoryGenerator::findSignal(
    const std::string& name,
    const std::vector<std::string>& signals) {

    for (unsigned int i = 0; i < signals.size(); i++) {
        if (signals.at(i).find(name) != string::npos) {
            return i;
        }
    }
    return -1;
}
