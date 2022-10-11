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
CostDBEntryStatsFU::energyActive() const {
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
CostDBEntryStatsFU::energyOperation(const std::string& name) const {
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
CostDBEntryStatsFU::hasEnergyOperation(const std::string& name) const {
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
CostDBEntryStatsFU::setEnergyActive(double) {
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
CostDBEntryStatsFU::setEnergyOperation(const std::string& name, double energy) {
    addEnergy(name, energy);
}
