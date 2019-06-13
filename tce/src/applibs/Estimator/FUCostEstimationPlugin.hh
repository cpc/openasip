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
 * @file FUCostEstimationPlugin.hh
 *
 * Declaration of FUCostEstimationPlugin class and definition of CPP macro
 * used to export FU cost estimation plugins.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_COST_ESTIMATION_PLUGIN_HH
#define TTA_FU_COST_ESTIMATION_PLUGIN_HH

#include <string>

#include "Exception.hh"
#include "CostEstimatorTypes.hh"
#include "CostEstimationPlugin.hh"
#include "FUImplementationLocation.hh"

namespace HDB {
    class HDBManager;
    class FUEntry;
}

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class FunctionUnit;
    class FUPort;
}

class ExecutionTrace;
class UtilizationStats;

namespace CostEstimator {
/**
 * An interface for classes that implement pluggable function unit cost 
 * estimation algorithms.
 */
class FUCostEstimationPlugin : public CostEstimationPlugin {
public:
    FUCostEstimationPlugin(const std::string& name);
    virtual ~FUCostEstimationPlugin();

    virtual bool estimateArea(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementation,
        AreaInGates& area,
        HDB::HDBManager& hdb);

    virtual bool estimateEnergy(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb);

    virtual bool estimatePortWriteDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& inputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimatePortReadDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& outputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimateMaximumComputationDelay(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& computationDelay,
        HDB::HDBManager& hdb);
};

/**
 * Exports the given class as an FU cost estimation plugin.
 */
#define EXPORT_FU_COST_ESTIMATOR_PLUGIN(PLUGIN_NAME__) \
extern "C" { \
    FUCostEstimationPlugin* create_estimator_plugin_##PLUGIN_NAME__(\
        ) { \
        PLUGIN_NAME__* instance = new PLUGIN_NAME__(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_estimator_plugin_##PLUGIN_NAME__(\
        FUCostEstimationPlugin* target) {\
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
