/**
 * @file SimulationStatisticsCalculator.hh
 *
 * Declaration of SimulationStatisticsCalculator class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_STATISTICS_CALCULATOR_HH
#define TTA_SIMULATION_STATISTICS_CALCULATOR_HH

namespace TTAProgram {
    class Instruction;
}

class ExecutableInstruction;

/**
 * An interface for simulation statistics calculators.
 *
 * SimulationStatistics calls the calculate method with each instruction in
 * the simulated program. Later on, different types of simulation data can
 * be fed by using different methods.
 */
class SimulationStatisticsCalculator {
public:
    virtual void calculate(
        const TTAProgram::Instruction& instructionData, 
        const ExecutableInstruction& executionCounts) = 0;

    SimulationStatisticsCalculator();
    virtual ~SimulationStatisticsCalculator();
};
#endif
