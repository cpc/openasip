/**
 * @file CostDBEntryStats.cc
 *
 * Implementation of CostDBEntryStats class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <map>
#include <string>

#include "Application.hh"
#include "CostDBEntryStats.hh"

using std::string;
using std::map;

// warning: is it possible that operation can have one of the
//          following names?
const std::string CostDBEntryStats::ENERGY_ACTIVE = "(active)";
const std::string CostDBEntryStats::ENERGY_IDLE = "(idle)";
const std::string CostDBEntryStats::ENERGY_READ = "(read)";
const std::string CostDBEntryStats::ENERGY_WRITE = "(write)";
const std::string CostDBEntryStats::ENERGY_READ_WRITE = "(rd,wr)";


/**
 * Constructor.
 *
 * @param areaData area.
 * @param internalDelayData Components internal delay.
 */
CostDBEntryStats::CostDBEntryStats(double areaData, double internalDelayData) : 
    area_(areaData), delay_(internalDelayData) {
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
 * @exception KeyNotFound Thrown in case that the two statistics won't contain
 * equal keys.
 */
CostDBEntryStats::CostDBEntryStats(
    const CostDBEntryStats& stats1, const CostDBEntryStats& stats2,
    double coefficient) 
    throw (KeyNotFound) {

    area_  = stats1.area() + coefficient * (stats2.area()  - stats1.area());
    delay_ = stats1.delay() + coefficient * (stats2.delay() - stats1.delay());

    for (EnergyMap::const_iterator i = stats1.energies_.begin();
         i != stats1.energies_.end(); i++) {

        string key = (*i).first;
        double energy1 = (*i).second;
        double energy2 = stats2.findEnergy(key);

        double energy = energy1 + coefficient * (energy2 - energy1);

        addEnergy(key, energy);
    }

    // checks that the energy keys are equal in both statistics
    for (EnergyMap::const_iterator i = stats2.energies_.begin();
         i != stats2.energies_.end(); i++) {

        string key = (*i).first;
        // checks if the energy exist for the key
        stats1.findEnergy(key);
    }
    // check also keys of the other stats
    for (EnergyMap::const_iterator i = stats1.energies_.begin();
         i != stats1.energies_.end(); i++) {

        string key = (*i).first;
        // checks if the energy exist for the key
        stats2.findEnergy(key);
    }

    for (DelayMap::const_iterator i = stats1.delays_.begin();
         i != stats1.delays_.end(); i++) {

        string key = (*i).first;
        double delay1 = (*i).second;
        double delay2 = stats2.findDelay(key);

        double delay = delay1 + coefficient * (delay2 - delay1);

        addDelay(key, delay);
    }
    
    // checks that the delay keys are equal in both statistics
    for (DelayMap::const_iterator i = stats2.delays_.begin();
         i != stats2.delays_.end(); i++) {

        string key = (*i).first;
        // checks if the delay exist for the key
        stats1.findDelay(key);
    }
    // check also keys of the other stats
    for (DelayMap::const_iterator i = stats1.delays_.begin();
         i != stats1.delays_.end(); i++) {

        string key = (*i).first;
        // checks if the delay exist for the key
        stats2.findDelay(key);
    }
}


/**
 * Destructor.
 */
CostDBEntryStats::~CostDBEntryStats() {
}

/**
 * Returns the copy of the statistics.
 *
 * @return Copy of statistics.
 */
CostDBEntryStats*
CostDBEntryStats::copy() const {

    CostDBEntryStats* newStats = createStats();

    for (EnergyMap::const_iterator i = energies_.begin();
         i != energies_.end(); i++) {

        newStats->addEnergy((*i).first, (*i).second);
    }
    for (DelayMap::const_iterator i = delays_.begin();
         i != delays_.end(); i++) {

        newStats->setDelay((*i).first, (*i).second);
    }

    return newStats;
}

/**
 * Create correct type of statistics.
 *
 * @return Correct type of statistics.
 */
CostDBEntryStats*
CostDBEntryStats::createStats() const {

    return new CostDBEntryStats(area(), delay());
}

/**
 * Returns the energy of an entry in an active cycle.
 *
 * @return The energy of an entry in an active cycle.
 * @exception WrongSubclass Never thrown.
 * @exception KeyNotFound Thrown if active energy is not set.
 */
double
CostDBEntryStats::energyActive() const 
    throw (WrongSubclass, KeyNotFound) {

    return findEnergy(ENERGY_ACTIVE);
}

/**
 * Returns the energy of an entry in an idle cycle.
 *
 * @return The energy of an entry in an idle cycle.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound Thrown if idle enrgy is not set.
 */
double
CostDBEntryStats::energyIdle() const
    throw (WrongSubclass, KeyNotFound) {

    return findEnergy(ENERGY_IDLE);
}


/**
 * Returns the energy of an entry when given operation is executed.
 *
 * The function will fail since this class should be used for entries
 * which do not have operation set as a search key.
 *
 * @param name The name of the operation.
 * @return The energy of an entry when given operation is executed.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 * @exception KeyNotFound No energy matching the string found.
 */
double
CostDBEntryStats::energyOperation(const std::string&) const
    throw (WrongSubclass, KeyNotFound) {

    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::energyOperation");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Checks whether the energy exists for the given operation.
 *
 * @param name name of the operation.
 * @return True if the energy exists for the given operation.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 */
bool
CostDBEntryStats::hasEnergyOperation(const std::string&) const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::hasEnergyOperation");
    return false; // stupid return statement to make compiler quiet
}

/**
 * Returns the read energy of an entry.
 *
 * @return The read energy of an entry.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 * @exception KeyNotFound Never thrown by this function.
 */
double
CostDBEntryStats::energyRead() const 
    throw (WrongSubclass, KeyNotFound) {
    
    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::energyRead");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Returns the write energy of an entry.
 *
 * @return The write energy of an entry.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 * @exception KeyNotFound Never thrown by this function.
 */
double
CostDBEntryStats::energyWrite() const 
    throw (WrongSubclass, KeyNotFound) {
    
    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::energyRead");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Returns the reads and writes energy of an entry.
 *
 * @param reads The number of simultaneus reads done for the unit.
 * @param writes The number of simultaneus writes done for the unit.
 * @return The reads and writes energy of an entry.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 * @exception KeyNotFound Never thrown by this function.
 */
double
CostDBEntryStats::energyReadWrite(int, int) const 
    throw (WrongSubclass, KeyNotFound) {

    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::energyReadsWrites");
    return 0.0; // stupid return statement to make compiler quiet
}

/**
 * Checks whether the energy exists for a given key.
 *
 * @param key The key.
 * @return True if energy exists for a given key, otherwise false.
 */
bool
CostDBEntryStats::hasEnergy(const std::string& key) const {

    EnergyMap::const_iterator i = energies_.find(key);
    return i != energies_.end();
}

/**
 * Returns the energy of an entry for a given key.
 *
 * @param key The key.
 * @return The energy of an entry in an active cycle.
 * @exception KeyNotFound Requested key was not found.
 */
double
CostDBEntryStats::findEnergy(const std::string& key) const 
    throw (KeyNotFound) {

    EnergyMap::const_iterator i = energies_.find(key);
    if (i == energies_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, "CostDBEntryStats::findEnergy");
    }
    return i->second;
}

/**
 * Set the energy of an entry in an active cycle.
 *
 * @param energy The energy of an entry in an active cycle.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStats::setEnergyActive(double energy) 
    throw (WrongSubclass) {

    EnergyMap::iterator iter = energies_.find(ENERGY_ACTIVE);
    if (iter != energies_.end()) {
        energies_.erase(iter);
    }
    addEnergy(ENERGY_ACTIVE, energy);
}

/**
 * Set the energy of an entry in an idle cycle.
 *
 * @param energy The energy of an entry in an idle cycle.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStats::setEnergyIdle(double energy) 
    throw (WrongSubclass) {

    EnergyMap::iterator iter = energies_.find(ENERGY_IDLE);
    if (iter != energies_.end()) {
        energies_.erase(iter);
    }
    addEnergy(ENERGY_IDLE, energy);
}

/**
 * Set the energy of an entry when given operation is executed.
 *
 * The function will fail since this class should be used for entries
 * which do not have operation set as a search key.
 *
 * @param name name of the operation.
 * @param energy The energy of an entry when given operation is executed.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 */
void
CostDBEntryStats::setEnergyOperation(const std::string&, double)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, 
                        "CostDBEntryStats::setEnergyOperation");
}

/**
 * Set the read energy of an entry.
 *
 * @param energy The read energy of an entry.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 */
void
CostDBEntryStats::setEnergyRead(double) 
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::setEnergyRead");
}

/**
 * Set the write energy of an entry.
 *
 * @param energy The write energy of an entry.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 */
void
CostDBEntryStats::setEnergyWrite(double) 
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::setEnergyWrite");
}

/**
 * Set the reads and writes energy of an entry.
 *
 * @param energy The reads and writes energy of an entry.
 * @param reads The number of reads of the unit.
 * @param writes The number of writes of the unit.
 * @exception WrongSubclass An illegal function was called for this
 * instance.
 */
void
CostDBEntryStats::setEnergyReadWrite(int, int, double) 
    throw (WrongSubclass) {
    
    throw WrongSubclass(__FILE__, __LINE__,
                        "CostDBEntryStats::setEnergyReadsWrites");
}

/**
 * Set the energy of an entry for a given key.
 *
 * Removes the old value.
 *
 * @param key The key.
 * @param energy The energy of an entry for a given key.
 */
void
CostDBEntryStats::addEnergy(const std::string& key, double energy) {

    // if old value for energy is found it is removed first
    EnergyMap::iterator iter = energies_.find(key);
    if (iter != energies_.end()) {
        energies_.erase(iter);
    }
    std::pair<const std::string, double> energyKey = 
        std::pair<const std::string, double>(key, energy);
    energies_.insert(energyKey);
}

/**
 * Returns the delay of given port is usage an an input/output.
 *
 * @param port The name of the port.
 * @return The delay of an entry when given port is used.
 * @exception WrongSubclass Never thrown by this function.
 * @exception KeyNotFound Thrown if delay for given port is not set.
 */
double
CostDBEntryStats::delayPort(const std::string& port) const 
    throw (WrongSubclass, KeyNotFound) {
 
    return findDelay(port);
}

/**
 * Set the delay of an input/output port usage.
 *
 * @param port Name of the input/output port.
 * @param delay The delay of an input/ouput when using the given port.
 * @exception WrongSubclass Never thrown by this function.
 */
void
CostDBEntryStats::setDelay(const std::string& port, double delay) 
    throw (WrongSubclass) {

    addDelay(port, delay);
}

/**
 * Returns the input/output delay of an entry usign given key as an
 * input/output port.
 *
 * @param key The key.
 * @return The input/output delay using the given port.
 * @exception KeyNotFound Requested key was not found.
 */
double
CostDBEntryStats::findDelay(const std::string& key) const 
    throw (KeyNotFound) {

    DelayMap::const_iterator i = delays_.find(key);

    if (i == delays_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, "CostDBEntryStats::findDelay");
    }    
    return i->second;
}

/**
 * Set the delay of an entry for a given key.
 *
 * Removes the old value.
 *
 * @param key The key.
 * @param delay The delay of an entry for a given key.
 */
void
CostDBEntryStats::addDelay(const std::string& key, double delay) {

    // if old value for delay is found it is removed first
    DelayMap::iterator iter = delays_.find(key);
    if (iter != delays_.end()) {
        delays_.erase(iter);
    }
    std::pair<const std::string, double> delayKey = 
        std::pair<const std::string, double>(key, delay);
    delays_.insert(delayKey);
}

/**
 * Checks whether the delay exists for a given key.
 *
 * @param key The key.
 * @return True if delay exists for a given key, otherwise false.
 */
bool
CostDBEntryStats::hasDelay(const std::string& key) const {

    DelayMap::const_iterator i = delays_.find(key);
    return i != delays_.end();
}
