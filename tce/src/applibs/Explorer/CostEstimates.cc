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
 * @file CostEstimates.cc
 *
 * Implementation of CostEstimates class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
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
 * Sets new cycle count for the given program.
 *
 * If there was an old cycle count value for the given key it is removed first.
 *
 * @param program Program that is used as a key.
 * @param cycles Cycle count of the program (in cycles).
 */
void
CostEstimates::setCycleCount(
    const TTAProgram::Program& program, ClockCycleCount cycles) {

    map<const TTAProgram::Program*, ClockCycleCount>::iterator iter = 
        cycleMap_.find(&program);
    if (iter != cycleMap_.end()) {
        cycleMap_.erase(iter);
    }
    cycleMap_.insert(
        std::pair<const TTAProgram::Program*, ClockCycleCount>(
            &program, cycles));
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

/**
 * Returns the number of different cycle counts (one for each program)
 * in the cost estimation data.
 * 
 * @return Returns the number of stored cycle counts.
 */
int
CostEstimates::cycleCounts() const {

    return cycleMap_.size();
}

/**
 * Returns the cycle count value from the given index.
 *
 * @param index The index.
 * @return The cycle count value in the given index.
 * @exception OutOfRange If the given index is less than 0 or greater or 
 * equal to the number of cycle counts.
 */
ClockCycleCount
CostEstimates::cycleCount(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= cycleCounts()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    map<const TTAProgram::Program*, ClockCycleCount>::const_iterator iter = 
        cycleMap_.begin();
    while (index > 0) {
        iter++;
        index--;
    }
    return (*iter).second;
}

/**
 * Returns the cycle count value corresponding to the Program.
 *
 * @param program Program of which cycle count is returned.
 * @return The cycle count of the Program.
 * @exception KeyNotFound Is thrown if program has no cycle count set.
 */
ClockCycleCount
CostEstimates::cycleCount(const TTAProgram::Program& program) const
    throw (KeyNotFound) {

    map<const TTAProgram::Program*, ClockCycleCount>::const_iterator iter = 
        cycleMap_.find(&program);
    if (iter == cycleMap_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
    return (*iter).second;
}
