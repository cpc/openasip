/**
 * @file CompiledSimUtilizationStats.hh
 *
 * Declaration of CompiledSimUtilizationStats class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_UTILIZATION_STATS_HH
#define COMPILED_SIM_UTILIZATION_STATS_HH

#include "UtilizationStats.hh"

namespace TTAProgram {
    class Program;
}

class CompiledSimulation;

/**
 * Calculates processor utilization data for compiled simulations
 */
class CompiledSimUtilizationStats : public UtilizationStats {
public:
    CompiledSimUtilizationStats();
    virtual ~CompiledSimUtilizationStats();

    virtual void calculate(
        const TTAProgram::Program& program, 
        const CompiledSimulation& compiledSim);

private:

};

#endif
