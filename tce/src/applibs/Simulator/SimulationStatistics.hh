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

