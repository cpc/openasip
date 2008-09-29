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
 * Class stores estimates of area (in gates), longest path delay (in nano 
 * seconds), energy consumptions (in milli joules) and cycle count (in cycles)
 * when the configuration is used to run different programs. Area and longest
 * path delay are constants to one configuration while there can be multiple
 * programs runned and so multiple energy consumption estimations and cycle
 * counts as well. Each energy consumption and cycle count is bound to one 
 * program.
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
    double energy(int index) const
        throw (OutOfRange);
    double energy(const TTAProgram::Program& program) const 
        throw (KeyNotFound);
    int cycleCounts() const;
    ClockCycleCount cycleCount(int index) const
        throw (OutOfRange);
    ClockCycleCount cycleCount(const TTAProgram::Program& program) const 
        throw (KeyNotFound);
    bool hasCycleCount(const TTAProgram::Program& program) const;

private:
    /// Map containing programs and energies consumed in running the programs.
    std::map<const TTAProgram::Program*, double> energyMap_;
    /// Map containing programs and cycle counts of the programs.
    std::map<const TTAProgram::Program*, ClockCycleCount> cycleMap_;
    /// Area estimation value (in gates).
    double area_;
    /// Longest path delay estimation value (in nano seconds).
    double longestPathDelay_;
};

#endif
