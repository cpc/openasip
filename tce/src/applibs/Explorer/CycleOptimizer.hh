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
