/**
 * @file SimulationStatistics.hh
 *
 * Declaration of SimulationStatistics class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_STATISTICS_HH
#define TTA_SIMULATION_STATISTICS_HH

#include <vector>

namespace TTAProgram {
    class Program;
}

class SimulationStatisticsCalculator;
class InstructionMemory;

/**
 * Calculates simulation statistics using user-given calculation classes.
 */
class SimulationStatistics {
public:
    SimulationStatistics(
        const TTAProgram::Program& programData, 
        const InstructionMemory& executionCounts);
    virtual ~SimulationStatistics();

    void addStatistics(SimulationStatisticsCalculator& statisticsType);
    void calculate();

private:
    /// All registered statistics types are stored in this container.
    std::vector<SimulationStatisticsCalculator*> statisticsTypes_;
    /// The program used in calculating the statistics.
    const TTAProgram::Program& program_;
    /// The execution counts of instructions and moves in the program.
    const InstructionMemory& executionCounts_;
};
#endif

