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
 * @file CostDBEntryStatsFU.hh
 *
 * Declaration of CostDBEntryStatsFU class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_STATS_FU_HH
#define TTA_COSTDB_ENTRY_STATS_FU_HH


#include "CostDBEntryStats.hh"


/**
 * Stores area, delay and energy statistics of function units.
 */
class CostDBEntryStatsFU: public CostDBEntryStats {
public:
    CostDBEntryStatsFU(double areaData, double delayData);
    virtual ~CostDBEntryStatsFU();
    CostDBEntryStatsFU(
        const CostDBEntryStatsFU& stats1,
        const CostDBEntryStatsFU& stats2,
        double coefficient);

    virtual double energyActive() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyOperation(const std::string& name) const 
        throw (WrongSubclass, KeyNotFound);
    virtual bool hasEnergyOperation(const std::string& name) const
        throw (WrongSubclass);

    virtual void setEnergyActive(double energy)
        throw (WrongSubclass);
    virtual void setEnergyOperation(const std::string& name, double energy)
        throw (WrongSubclass);

protected:
    virtual CostDBEntryStats* createStats() const;

private:
    /// Copying not allowed.
    CostDBEntryStatsFU(const CostDBEntryStatsFU&);
    /// Assignment not allowed.
    CostDBEntryStatsFU& operator=(const CostDBEntryStatsFU&);
};

#endif
