/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SimulationStatisticsCalculator.hh
 *
 * Declaration of SimulationStatisticsCalculator class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
