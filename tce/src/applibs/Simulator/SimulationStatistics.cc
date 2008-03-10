/**
 * @file SimulationStatistics.hh
 *
 * Implementation of SimulationStatistics class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SimulationStatistics.hh"
#include "SimulationStatisticsCalculator.hh"
#include "Program.hh"
#include "NullInstruction.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"

/**
 * Constructs a new simulation statistics calculation loop.
 *
 * @param programData The program the statistics are calculated from.
 * @param executionCounts Execution counts of instructions and moves in the
 *                        program.
 */
SimulationStatistics::SimulationStatistics(
    const TTAProgram::Program& programData, 
    const InstructionMemory& executionCounts) :
    program_(programData), executionCounts_(executionCounts) {
}


/**
 * Destructor.
 */
SimulationStatistics::~SimulationStatistics() {
}

/**
 * Adds a new statistics type to be included in simulation statistics
 * calculation.
 *
 * @param statisticsType New statistics type.
 */
void
SimulationStatistics::addStatistics(
    SimulationStatisticsCalculator& statisticsType) {
    statisticsTypes_.push_back(&statisticsType);
}

/**
 * Calculates simulation statistics by going through the program and
 * invoking calculate() for all statistics types.
 */
void 
SimulationStatistics::calculate() {

    if (statisticsTypes_.size() == 0)
        return;

    const TTAProgram::Instruction* currentInstruction = 
        &program_.instructionAt(program_.startAddress().location());
    while (currentInstruction != &TTAProgram::NullInstruction::instance()) {

        const ExecutableInstruction& execInstr = 
            executionCounts_.instructionAtConst(
                currentInstruction->address().location());
        if (execInstr.executionCount() == 0) {
            currentInstruction = 
                &program_.nextInstruction(*currentInstruction);
            continue;
        }

        for (std::size_t i = 0; i < statisticsTypes_.size(); ++i) {
            statisticsTypes_[i]->calculate(*currentInstruction, execInstr);
        }

        currentInstruction = &program_.nextInstruction(*currentInstruction);
    }
}
