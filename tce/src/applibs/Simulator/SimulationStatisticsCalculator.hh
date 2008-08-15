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
