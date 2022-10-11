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
 * @file CostEstimates.hh
 *
 * Declaration of CostEstimates class that represents set of cost
 * estimates.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATES_HH
#define TTA_COST_ESTIMATES_HH

#include <map>

#include "Application.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"

namespace TTAProgram {
    class Program;
}

/**
 * This class represents cost estimates for a machine configuration.
 *
 * Class stores estimates of area (in gates), longest path delay (in nano 
 * seconds) and energy consumptions (in milli joules)
 * when the configuration is used to run different programs. Area and longest
 * path delay are constants to one configuration while there can be multiple
 * programs executed so multiple energy consumption estimations and cycle
 * counts as well. Each energy consumption is bound to one program.
 */
class CostEstimates {
public:
    CostEstimates();
    virtual ~CostEstimates();
    void setArea(double area);
    void setLongestPathDelay(double delay);
    void setEnergy(const TTAProgram::Program& program, double energy);
    void setCycleCount(
        const TTAProgram::Program& program, ClockCycleCount cycles);
    double area() const;
    double longestPathDelay() const;
    int energies() const;
    double energy(int index) const;
    double energy(const TTAProgram::Program& program) const;

private:
    /// Map containing programs and energies consumed in running the programs.
    std::map<const TTAProgram::Program*, double> energyMap_;
    /// Area estimation value (in gates).
    double area_;
    /// Longest path delay estimation value (in nano seconds).
    double longestPathDelay_;
};

#endif
