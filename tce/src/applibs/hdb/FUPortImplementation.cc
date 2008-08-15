/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file FUPortImplementation.cc
 *
 * Implementation of FUPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
