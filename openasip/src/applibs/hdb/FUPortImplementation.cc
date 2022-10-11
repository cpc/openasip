/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file FUPortImplementation.cc
 *
 * Implementation of FUPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUPortImplementation.hh"
#include "FUImplementation.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Adds the port automatically to the given FUImplementation.
 *
 * @param name Name of the port.
 * @param architecturePort Name of the corresponding port in architecture.
 * @param widthFormula The formula for the width of the port.
 * @param loadPort Name of the load enable port.
 * @param guardPort Name of the guard port if the port is guarded.
 * @param parent The parent FU implementation.
 */
FUPortImplementation::FUPortImplementation(
    const std::string& name,
    const std::string& architecturePort,
    const std::string& widthFormula,
    const std::string& loadPort,
    const std::string& guardPort,
    FUImplementation& parent) :
    PortImplementation(name, loadPort), 
    architecturePort_(architecturePort), widthFormula_(widthFormula), 
    guardPort_(guardPort) {

    parent.addArchitecturePort(this);
}


/**
 * The destructor.
 */
FUPortImplementation::~FUPortImplementation() {
}


/**
 * Sets the corresponding port in the FU architecture.
 *
 * @param name Name of the port.
 */
void
FUPortImplementation::setArchitecturePort(const std::string& name) {
    architecturePort_ = name;
}


/**
 * Returns the name of the corresponding port in the FU architecture.
 *
 * @return The name of the port.
 */
std::string
FUPortImplementation::architecturePort() const {
    return architecturePort_;
}


/**
 * Sets the name of the guard port if the port is guarded.
 *
 * @param name Name of the guard port.
 */
void
FUPortImplementation::setGuardPort(const std::string& name) {
    guardPort_ = name;
}


/**
 * Returns the name of the guard port if the port is guarded.
 *
 * @return The name of the guard port.
 */
std::string
FUPortImplementation::guardPort() const {
    return guardPort_;
}


/**
 * Sets the width formula of the port.
 *
 * @param formula The new formula.
 */
void
FUPortImplementation::setWidthFormula(const std::string& formula) {
    widthFormula_ = formula;
}


/**
 * Returns the formula for the width of the port.
 *
 * @return The formula.
 */
std::string
FUPortImplementation::widthFormula() const {
    return widthFormula_;
}
}
