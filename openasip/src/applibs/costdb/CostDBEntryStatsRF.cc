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
 * @file CostDBEntryStatsRF.cc
 *
 * Implementation of CostDBEntryStatsRF class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <map>

#include "CostDBEntryStatsRF.hh"
#include "Application.hh"
#include "Conversion.hh"

using std::string;
using std::map;


/**
 * Constructor.
 *
 * @param areaData area.
 * @param delayData delay.
 */
CostDBEntryStatsRF::CostDBEntryStatsRF(double areaData, double delayData):
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
CostDBEntryStatsRF::CostDBEntryStatsRF(
    const CostDBEntryStatsRF& stats1,
    const CostDBEntryStatsRF& stats2,
    double coefficient):
    CostDBEntryStats(stats1, stats2, coefficient) {
}


/**
 * Destructor.
 */
CostDBEntryStatsRF::~CostDBEntryStatsRF() {
}



/**
 * Create correct type of statistics.
 *
 * @return Correct type of statistics.
 */
CostDBEntryStats*
CostDBEntryStatsRF::createStats() const {

    return new CostDBEntryStatsRF(area(), delay());
}

/**
 * Returns the energy of an entry in an active cycle.
 *
 * The function will fail since register files do not have unambiguous
 * energy for the whole unit but separately for each access pairs.
 *
 * @return The energy of an entry in an active cycle.
 * @exception WrongSubclass An illegal function was called for this 
 * instance.
 * @exception KeyNotFound Never thown by this function.
 */
double
CostDBEntryStatsRF::energyActive() const {
    throw WrongSubclass(__FILE__, __LINE__, 
			"CostDBEntryStatsRF::energyActive");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Returns the read energy of an entry.
 *
 * @return The read energy of an entry.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound Throws if read energy is not set.
 */
double
CostDBEntryStatsRF::energyRead() const {
    return findEnergy(ENERGY_READ);
}

/**
 * Returns the write energy of an entry.
 *
 * @return The write energy of an entry.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound Throws if write energy is not set.
 */
double
CostDBEntryStatsRF::energyWrite() const {
    return findEnergy(ENERGY_WRITE);
}

/**
 * Returns the reads and writes energy of an entry.
 *
 * @param reads The number of simultaneus reads done for the unit.
 * @param writes The number of simultaneus writes done for the unit.
 * @return The reads and writes energy of an entry.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound Throws if the requested energy is not found.
 */
double
CostDBEntryStatsRF::energyReadWrite(int reads, int writes) const {
    return findEnergy(generateReadWriteString(reads, writes));
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
CostDBEntryStatsRF::setEnergyActive(double) {
    throw WrongSubclass(__FILE__, __LINE__,
			"CostDBEntryStatsRF::setEnergyActive");
}

/**
 * Set the read energy of an entry.
 *
 * @param energy The read energy of an entry.
 * @exception WrongSubclass An illegal function was called for this instance.
 */
void
CostDBEntryStatsRF::setEnergyRead(double energy) {
    addEnergy(ENERGY_READ, energy);
}

/**
 * Set the write energy of an entry.
 *
 * @param energy The write energy of an entry.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStatsRF::setEnergyWrite(double energy) {
    addEnergy(ENERGY_WRITE, energy);
}

/**
 * Set the reads and writes energy of an entry.
 *
 * @param energy The reads and writes energy of an entry.
 * @param reads The number of reads of the unit.
 * @param writes The number of writes of the unit.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStatsRF::setEnergyReadWrite(int reads, int writes, double energy) {
    addEnergy(generateReadWriteString(reads, writes), energy);
}

/**
 * Returns a string corresponding to given (read,write) combination in
 * internal format.
 *
 * @param reads Number of simultaneous reads.
 * @param writes Number of simultaneous writes.
 * @return String corresponding to given (read,write) combination in
 *         internal format.
 */
std::string
CostDBEntryStatsRF::generateReadWriteString(int reads, int writes) {

    return ENERGY_READ_WRITE + "_" +
        Conversion::toString(reads) + "_" +
        Conversion::toString(writes);
}
