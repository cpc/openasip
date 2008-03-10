/**
 * @file CycleOptimizer.hh
 *
 * Optimizes the cycle count of the architecture by generating architectures
 * with greater amount of resources so that the resources won't be the limiting
 * factor.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CYCLE_OPTIMIZER_HH
#define TTA_CYCLE_OPTIMIZER_HH

#include <vector>
#include "DSDBManager.hh"
#include "DBTypes.hh"
#include "Exception.hh"


/**
 * Class that optimizes cycle count of the processor architecture cycle count
 * for given applications by adding resources.
 *
 * Resources are added until the scheduler can no longer generate better 
 * schedules.
 */
class CycleOptimizer {
public:
    CycleOptimizer(DSDBManager& dsdb, RowID startConfID, int superioty)
        throw (OutOfRange);
    virtual ~CycleOptimizer();
    
    std::vector<RowID> optimize()
        throw (KeyNotFound, Exception);
    RowID minimizeRegisterFiles(
        RowID confToMinimize, ClockCycleCount maxCycles)
        throw (KeyNotFound);
    RowID minimizeFunctionUnits(
        RowID confToMinimize, ClockCycleCount maxCycles)
        throw (KeyNotFound);
    RowID minimizeBuses(
        RowID confToMinimize, ClockCycleCount maxCycles)
        throw (KeyNotFound);
    RowID minimizeBusFURF(
        RowID confToMinimize, ClockCycleCount maxCycles)
        throw (KeyNotFound);

private:
    /// Set of result architectures.
    std::vector<RowID> resultIDs_;
    /// DSDB manager to use.
    DSDBManager& dsdb_;
    /// Start configuration ID in the DSDB.
    RowID startConfID_;
    /// Percent value of how much better schedules is wanted to continue
    /// optimization
    int superioty_;
};

#endif
