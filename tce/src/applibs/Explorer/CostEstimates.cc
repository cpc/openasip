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
 * @file CostEstimates.cc
 *
 * Implementation of CostEstimates class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <map>
#include "CostEstimates.hh"
#include "Program.hh"

using std::map;

/**
 * The contstructor.
 */
CostEstimates::CostEstimates() :
    area_(0), longestPathDelay_(0) {
}

/**
 * The destructor.
 */
CostEstimates::~CostEstimates() {
}

/**
 * Sets the area estimate (in gates).
 *
 * @param area Area estimate value (in gates).
 */
void
CostEstimates::setArea(double area) {

    area_ = area;
}

/**
 * Sets the longest path delay estimate (in nano seconds).
 *
 * @param delay Longest path delay estimte value (in nano seconds).
 */
void
CostEstimates::setLongestPathDelay(double delay) {

    longestPathDelay_ = delay;
}

/**
 * Sets new energy estimate for the given program.
 *
 * If there was an old energy value for the given key it is removed first.
 *
 * @param program Program that is used as a key.
 * @param energy Energy consumed while running the program (in milli joules).
 */
void
CostEstimates::setEnergy(
    const TTAProgram::Program& program, double energy) {

    map<const TTAProgram::Program*, double>::iterator iter = 
        energyMap_.find(&program);
    if (iter != energyMap_.end()) {
        energyMap_.erase(iter);
    }
    energyMap_.insert(
        std::pair<const TTAProgram::Program*, double>(&program, energy));
}

/**
 * Returns the area estimate (in gates).
 *
 * @return Returns the area estimate (in gates).
 */
double
CostEstimates::area() const {

    return area_;
}

/**
 * Returns the longest path delay estimate (in nano seconds).
 *
 * @return Returns the longest path delay estimate (in nano seconds).
 */
double
CostEstimates::longestPathDelay() const {

    return longestPathDelay_;
}

/**
 * Returns the number of different energy estimates (one for each program)
 * in the cost estimation data.
 * 
 * @return Returns the number of stored energy estimates.
 */
int
CostEstimates::energies() const {

    return energyMap_.size();
}

/**
 * Returns the energy value from the given index.
 *
 * @param index The index.
 * @return The energy value in the given index.
 * @exception OutOfRange If the given index is less than 0 or greater or 
 * equal to the number of energies.
 */
double
CostEstimates::energy(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= energies()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    map<const TTAProgram::Program*, double>::const_iterator iter = 
        energyMap_.begin();
    while (index > 0) {
        iter++;
        index--;
    }
    return (*iter).second;
}

/**
 * Returns the energy value corresponding to the Program.
 *
 * @param program Program of which energy is returned.
 * @return The energy of the Program.
 * @exception KeyNotFound Is thrown if program has no energy set.
 */
double 
CostEstimates::energy(const TTAProgram::Program& program) const
    throw (KeyNotFound) {

    map<const TTAProgram::Program*, double>::const_iterator iter = 
        energyMap_.find(&program);
    if (iter == energyMap_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
    return (*iter).second;
}

