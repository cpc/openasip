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
 * @file DSDBManager.hh
 *
 * Declaration of DSDBManager class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DSDB_MANAGER_HH
#define TTA_DSDB_MANAGER_HH

#include <string>
#include <set>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
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

#define ILLEGAL_ROW_ID (-1)

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
        MachineConfiguration(
            RowID arch, bool hasImpl, RowID impl) : 
            architectureID(arch), 
            hasImplementation(hasImpl),
            implementationID(impl) {}

        MachineConfiguration() : 
            architectureID(ILLEGAL_ROW_ID), 
            hasImplementation(false),
            implementationID(ILLEGAL_ROW_ID) {}
    };

    struct ApplicationData {
        std::string id;
        std::string name;
    };
   
    /// Struct for configuration costs with a specified application
    struct ConfigurationCosts {
        RowID configurationID;
        std::string application;
        double energyEstimate;
        ClockCycleCount cycleCount;
        double longestPathDelay;
        double area;
    };
   
    struct idComparator {
        bool operator()(ConfigurationCosts c1, ConfigurationCosts c2) const {
            if (c1.configurationID != c2.configurationID)
            return (c1.configurationID < c2.configurationID);
            // if the id's match then applications must differ
            return (c1.application < c2.application);
        }
    };
   
    struct cycleComparator {
        bool operator()(ConfigurationCosts c1, ConfigurationCosts c2) const {
            if (c1.cycleCount == c2.cycleCount) {
                if (c1.configurationID != c2.configurationID) {
                    return (c1.configurationID < c2.configurationID);
                } else {
                    return (c1.application < c2.application);
                }
            }
            return (c1.cycleCount < c2.cycleCount);
        } 
    };

    struct energyComparator {
        bool operator()(ConfigurationCosts c1, ConfigurationCosts c2) const {
            if (c1.energyEstimate == c2.energyEstimate) {
                if (c1.configurationID != c2.configurationID) {
                    return (c1.configurationID < c2.configurationID);
                } else {
                    return (c1.application < c2.application);
                }
            }
            return (c1.energyEstimate < c2.energyEstimate);
        }
    };
   
    struct appComparator {
        bool operator()(ConfigurationCosts c1, ConfigurationCosts c2) const {
            if (c1.application == c2.application)
                return (c1.configurationID < c2.configurationID);
            return (c1.application < c2.application);
        }
    };

    typedef boost::tuple<RowID, int, ClockCycleCount>
    ParetoPointConnectivityAndCycles;

    typedef std::set<ParetoPointConnectivityAndCycles> 
    ParetoSetConnectivityAndCycles;

    /// Identifiers for ordering results.
    enum Order {
        ORDER_BY_CONFIGURATION,
        ORDER_BY_CYCLE_COUNT,
        ORDER_BY_ENERGY_ESTIMATE,
        ORDER_BY_APPLICATION
    };

    DSDBManager(const std::string& file);

    virtual ~DSDBManager();

    static DSDBManager* createNew(const std::string& file);

    std::string dsdbFile() const;

    RowID addArchitecture(const TTAMachine::Machine& mom);
    RowID addImplementation(
        const IDF::MachineImplementation& impl, double longestPathDelay,
        CostEstimator::AreaInGates area);

    RowID addConfiguration(const MachineConfiguration& conf);

    RowID addApplication(const std::string& path);

    void addEnergyEstimate(
        RowID application, RowID implementation, double energyEstimate);

    void addCycleCount(
        RowID application, RowID architecture, ClockCycleCount count);

    void setLongestPathDelayEstimate(RowID implementation, double delay);

    void setAreaEstimate(RowID implementation, CostEstimator::AreaInGates area);

    bool hasApplication(RowID id) const;
    bool hasApplication(const std::string& applicationPath) const;
    void removeApplication(RowID id);

    std::string applicationPath(RowID id) const;

    bool hasArchitecture(RowID id) const;
    TTAMachine::Machine* architecture(RowID id) const;

    RowID architectureId(const TTAMachine::Machine& mach) const;

    bool hasImplementation(RowID id) const;
    IDF::MachineImplementation* implementation(RowID id) const;

    bool hasConfiguration(RowID id) const;
    MachineConfiguration configuration(RowID id) const;
    RowID configurationId(const MachineConfiguration& conf) const;

    void removeConfiguration(RowID id);

    bool hasEnergyEstimate(RowID application, RowID implementation) const;
    double energyEstimate(RowID application, RowID implementation) const;

    bool hasCycleCount(RowID application, RowID architecture) const;
    ClockCycleCount cycleCount(RowID application, RowID architecture) const;
    std::vector<ClockCycleCount> 
    cycleCounts(const MachineConfiguration& conf) const;
    bool
    isUnschedulable(
        RowID application, RowID architecture) const;
    void setUnschedulable(RowID application, RowID architecture);

    double longestPathDelayEstimate(RowID implementation) const;
    CostEstimator::AreaInGates areaEstimate(RowID implementation) const;

    std::set<RowID> applicationIDs() const;
    std::set<RowID> architectureIDs() const;
    std::set<RowID> configurationIDs() const;
    std::set<RowID> archConfigurationIDs(RowID architectureID) const;

    void writeArchitectureToFile(RowID id, const std::string& path) const;
    void writeImplementationToFile(RowID id, const std::string& path) const;
    void writeConfigurationToFile(
        const MachineConfiguration& conf, const std::string& path);

    std::vector<ConfigurationCosts> applicationCostEstimatesByConf(
        Order ordering = ORDER_BY_CONFIGURATION) const;

    ParetoSetConnectivityAndCycles 
    paretoSetConnectivityAndCycles(RowID application=ILLEGAL_ROW_ID) const;

    int applicationCount() const;
private:
    std::string architectureString(RowID id) const;
    std::string implementationString(RowID id) const;

    /// Handle to the database.
    SQLite* db_;
    /// Handle to the database connection.
    RelationalDBConnection* dbConnection_;
    /// The DSDB file containing the current database.
    std::string file_;
};

#endif
