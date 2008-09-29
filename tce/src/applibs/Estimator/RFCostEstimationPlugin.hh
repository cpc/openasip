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
 * @file RFCostEstimationPlugin.hh
 *
 * Declaration of RFCostEstimationPlugin class and definitions of CPP macros
 * used to define and export RF cost estimation plugins.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_COST_ESTIMATION_PLUGIN_HH
#define TTA_RF_COST_ESTIMATION_PLUGIN_HH

#include <string>

#include "Exception.hh"
#include "CostEstimatorTypes.hh"
#include "CostEstimationPlugin.hh"
#include "RFImplementationLocation.hh"

namespace HDB {
    class HDBManager;
    class FUEntry;
}

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class BaseRegisterFile;
    class RFPort;
}

class ExecutionTrace;
class UtilizationStats;

namespace CostEstimator {

/**
 * A base interface for all classes that implement pluggable register file
 * cost estimation algorithms.
 */
class RFCostEstimationPlugin : public CostEstimationPlugin {
public:
    RFCostEstimationPlugin(const std::string& name);
    virtual ~RFCostEstimationPlugin();

    virtual bool estimateArea(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        AreaInGates& area,
        HDB::HDBManager& hdb);

    virtual bool estimateEnergy(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb);

    virtual bool estimatePortWriteDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& inputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimatePortReadDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& outputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimateMaximumComputationDelay(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& computationDelay,
        HDB::HDBManager& hdb);
};

/**
 * Exports the given class as a RF cost estimation plugin.
 */
#define EXPORT_RF_COST_ESTIMATOR_PLUGIN(PLUGIN_NAME__) \
extern "C" { \
    RFCostEstimationPlugin* create_estimator_plugin_##PLUGIN_NAME__(\
        ) { \
        PLUGIN_NAME__* instance = new PLUGIN_NAME__(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_estimator_plugin_##PLUGIN_NAME__(\
        RFCostEstimationPlugin* target) {\
        delete target;\
    }\
}

}

/**
 * Defines the description string for the plugin.
 *
 * Can be used only inside a plugin class definition.
 *
 * @param TEXT__ The description string. 
 */
#define DESCRIPTION(TEXT__) \
public:\
    virtual std::string description() const { return TEXT__; }\
    int* XXXXXd_escrip__tion__

#endif
