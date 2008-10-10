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
 * @file CostDBEntryStatsFU.cc
 *
 * Implementation of CostDBEntryStatsFU class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <map>

#include "CostDBEntryStatsFU.hh"

using std::string;
using std::map;


/**
 * Constructor.
 *
 * @param areaData area.
 * @param delayData delay.
 */
CostDBEntryStatsFU::CostDBEntryStatsFU(double areaData, double delayData) : 
    CostDBEntryStats(areaData, delayData) {
}

/**
 * Constructor.
 *
 * Combines two statistics into one using coefficient as a weighting
 * factor on interpolating the statistics. For example, if weighting
 * factor is 0.4, the first area 100 and the second 200, area of new
 * statistics will be 140. delay and energy will be handled similarly.
 *
 * @param stats1 First statistics.
 * @param stats2 Second statistics.
 * @param coefficient Weighting factor.
 */
CostDBEntryStatsFU::CostDBEntryStatsFU(
    const CostDBEntryStatsFU& stats1,
    const CostDBEntryStatsFU& stats2,
    double coefficient):
    CostDBEntryStats(stats1, stats2, coefficient) {
}


/**
 * Destructor.
 */
CostDBEntryStatsFU::~CostDBEntryStatsFU() {
}

/**
 * Create correct type of statistics.
 *
 * @return Correct type of statistics.
 */
CostDBEntryStats*
CostDBEntryStatsFU::createStats() const {
    return new CostDBEntryStatsFU(area(), delay());
}

/**
 * Returns the energy of an entry in an active cycle.
 *
 * The function will fail since function units do not have unambiguous
 * energy for the whole unit but separately for each operation.
 *
 * @return The energy of an entry in an active cycle.
 * @exception WrongSubclass An illegal function was called for this 
 * instance.
 * @exception KeyNotFound Never thrown by this function.
 */
double
CostDBEntryStatsFU::energyActive() const 
    throw (WrongSubclass, KeyNotFound) {
   
    throw WrongSubclass(__FILE__, __LINE__, 
			"CostDBEntryStatsFU::energyActive");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Returns the energy of an entry when given operation is executed.
 *
 * @param name name of the operation.
 * @return The energy of an entry when given operation is executed.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound No energy matching the string found.
 */
double
CostDBEntryStatsFU::energyOperation(const std::string& name) const 
    throw (WrongSubclass, KeyNotFound) {

    return findEnergy(name);
}

/**
 * Checks whether the energy exists for the given operation.
 *
 * @param name name of the operation.
 * @return True if the energy exists for the given operation.
 * @exception WrongSubclass Never thrown by this function.
 */
bool
CostDBEntryStatsFU::hasEnergyOperation(const std::string& name) const 
    throw (WrongSubclass) {

    return hasEnergy(name);
}

/**
 * Set the energy of an entry in an active cycle.
 *
 * The function will fail since function units do not have unambiguous
 * energy for the whole unit but separately for each operation.
 *
 * @param energy The energy of an entry in an active cycle.
 * @exception WrongSubclass An illegal function was called for this 
 * instance.
 */
void
CostDBEntryStatsFU::setEnergyActive(double)
    throw (WrongSubclass) {
    
    throw WrongSubclass(__FILE__, __LINE__,
			"CostDBEntryStatsFU::setEnergyActive");
}

/**
 * Set the energy of an entry when given operation is executed.
 *
 * @param name name of the operation.
 * @param energy The energy of an entry when given operation is executed.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStatsFU::setEnergyOperation(const std::string& name, double energy) 
    throw (WrongSubclass) {
    
    addEnergy(name, energy);
}
