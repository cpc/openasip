/**
 * @file CostEstimates.hh
 *
 * Declaration of CostEstimates class that represents set of cost
 * estimates.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
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
