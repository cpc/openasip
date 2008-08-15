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
 * @file DSDBManager.hh
 *
 * Declaration of DSDBManager class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DSDB_MANAGER_HH
#define TTA_DSDB_MANAGER_HH

#include <string>
#include <set>
#include <vector>
#include "DBTypes.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
#include "CostEstimatorTypes.hh"

class SQLite;
class RelationalDBConnection;

namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

/**
 * Design space database manager.
 *
 * Design space database stores information about explored machine
 * configurations for test applications.
 *
 * The following information can be stored:
 * - Machine architectures.
 * - Implementations for architectures which define a working machine
 *   configuration.
 * - Longest path delay estimates for implementations.
 * - Area estimates for implementations.
 * - Test applications for exploration.
 * - Runtimes as clock cycle counts for application-architecture pairs.
 * - Energy estimates for application-implementation pairs.
 */
class DSDBManager {
public:
    struct MachineConfiguration {
        RowID architectureID;
        bool hasImplementation;
        RowID implementationID;
    };

    /// Struct for configuration costs with a specified application
    struct ConfigurationCosts {
        RowID configurationID;
        std::string application;
        double energyEstimate;
        ClockCycleCount cycleCount;
    };

    /// Identifiers for ordering results.
    enum Order {
        ORDER_BY_CONFIGURATION,
        ORDER_BY_CYCLE_COUNT,
        ORDER_BY_ENERGY_ESTIMATE,
        ORDER_BY_APPLICATION
    };

    DSDBManager(const std::string& file)
        throw (IOException);

    virtual ~DSDBManager();

    static DSDBManager* createNew(const std::string& file)
        throw (IOException);

    std::string dsdbFile() const;

    RowID addArchitecture(const TTAMachine::Machine& mom) 
        throw (RelationalDBException);
    RowID addImplementation(
        const IDF::MachineImplementation& impl,
        double longestPathDelay,
        CostEstimator::AreaInGates area)
        throw (KeyNotFound);

    RowID addConfiguration(const MachineConfiguration& conf)
        throw (KeyNotFound);

    RowID addApplication(const std::string& path);

    void addEnergyEstimate(
        RowID application, RowID implementation, double energyEstimate)
        throw (KeyNotFound);

    void addCycleCount(
        RowID application, RowID architecture, ClockCycleCount count)
        throw (KeyNotFound);

    void setLongestPathDelayEstimate(
        RowID implementation, double delay)
        throw (KeyNotFound);

    void setAreaEstimate(
        RowID implementation, CostEstimator::AreaInGates area)
        throw (KeyNotFound);

    bool hasApplication(RowID id) const;
    bool hasApplication(const std::string& applicationPath) const;
    void removeApplication(RowID id)
        throw (KeyNotFound);

    std::string applicationPath(RowID id) const
        throw (KeyNotFound);

    bool hasArchitecture(RowID id) const;
    TTAMachine::Machine* architecture(RowID id) const
        throw (KeyNotFound);

    bool hasImplementation(RowID id) const;
    IDF::MachineImplementation* implementation(RowID id) const
        throw (KeyNotFound);

    bool hasConfiguration(RowID id) const;
    MachineConfiguration configuration(RowID id) const
        throw (KeyNotFound);

    void removeConfiguration(RowID id)
        throw (KeyNotFound);

    bool hasEnergyEstimate(RowID application, RowID implementation) const;
    double energyEstimate(RowID application, RowID implementation) const
        throw (KeyNotFound);

    bool hasCycleCount(RowID application, RowID architecture) const;
    ClockCycleCount cycleCount(RowID application, RowID architecture) const
        throw (KeyNotFound);

    double longestPathDelayEstimate(RowID implementation) const 
        throw (KeyNotFound);

    CostEstimator::AreaInGates areaEstimate(RowID implementation) const
        throw (KeyNotFound);

    std::set<RowID> applicationIDs() const;
    std::set<RowID> architectureIDs() const;
    std::set<RowID> configurationIDs() const;
    std::set<RowID> archConfigurationIDs(RowID architectureID) const
        throw (KeyNotFound);

    void writeArchitectureToFile(RowID id, const std::string& path) const
        throw (KeyNotFound, IOException);
    void writeImplementationToFile(RowID id, const std::string& path) const
        throw (KeyNotFound, IOException);

    std::vector<ConfigurationCosts> applicationCostEstimatesByConf(
        Order ordering = ORDER_BY_CONFIGURATION) const;

    int applicationCount() const;
private:
    std::string architectureString(RowID id) const
        throw (KeyNotFound);
    std::string implementationString(RowID id) const
        throw (KeyNotFound);
    
    /// Handle to the database.
    SQLite* db_;
    /// Handle to the database connection.
    RelationalDBConnection* dbConnection_;
    /// The DSDB file containing the current database.
    std::string file_;    
};

#endif
