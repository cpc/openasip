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
 * @file ICDecoderEstimatorPlugin.hh
 *
 * Declaration of ICDecoderEstimatorPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IC_DECODER_ESTIMATOR_PLUGIN_HH
#define TTA_IC_DECODER_ESTIMATOR_PLUGIN_HH

#include "CostEstimationPlugin.hh"
#include "TransportPath.hh"
#include "SocketImplementationLocation.hh"
#include "BusImplementationLocation.hh"

namespace TTAMachine {
    class Machine;
    class ControlUnit;
}
namespace TTAProgram {
    class Program;
}

namespace HDB {
    class HDBRegistry;
}

namespace IDF {
    class MachineImplementation;
}

class ExecutionTrace;

namespace CostEstimator {
/**
 * Base class for plugins that are able to estimate costs of a IC and
 * decoder combination.
 */
class ICDecoderEstimatorPlugin : public CostEstimationPlugin {
public:

    ICDecoderEstimatorPlugin(
        const std::string& name);
    virtual ~ICDecoderEstimatorPlugin();

    virtual bool estimateICDelayOfPath(
        HDB::HDBRegistry& hdbRegistry,
        const TransportPath& path,
        const IDF::MachineImplementation& machineImplementation,
        const IDF::SocketImplementationLocation& sourceSocketImplementation,
        const IDF::BusImplementationLocation& busImplementation,
        const IDF::SocketImplementationLocation& 
        destinationSocketImplementation,
        DelayInNanoSeconds& delay);    

    virtual bool estimateICArea(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        AreaInGates& area);

    virtual bool estimateICEnergy(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy);

    virtual TTAMachine::ControlUnit* generateControlUnit();
};

}

/**
 * Exports the given class as an IC&decoder estimator.
 *
 * @note The class name of the estimator MUST be [PLUGIN_NAME__]Estimator,
 *       thus an estimator exported with EXPORT_ICDEC_ESTIMATOR(Example)
 *       exports a class called ExampleEstimator.
 */
#define EXPORT_ICDEC_ESTIMATOR(PLUGIN_NAME__) \
extern "C" { \
    ICDecoderEstimatorPlugin*\
    create_estimator_plugin_##PLUGIN_NAME__(\
        ) { \
        PLUGIN_NAME__##Estimator* instance = \
            new PLUGIN_NAME__##Estimator(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_estimator_plugin_##PLUGIN_NAME__(\
        ICDecoderEstimatorPlugin* target) {\
        delete target;\
    }\
}

#endif
