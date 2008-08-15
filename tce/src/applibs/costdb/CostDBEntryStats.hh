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
 * @file CostDBEntryStats.hh
 *
 * Declaration of CostDBEntryStats class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_STATS_HH
#define TTA_COSTDB_ENTRY_STATS_HH


#include <map>

#include "Exception.hh"


/**
 * Stores area, delay and energy statistics.
 */
class CostDBEntryStats {
public:
    CostDBEntryStats(double areaData, double internalDelayData);
    virtual ~CostDBEntryStats();
    CostDBEntryStats(
        const CostDBEntryStats& stats1, const CostDBEntryStats& stats2,
        double coefficient)
        throw (KeyNotFound);
    virtual CostDBEntryStats* copy() const;

    virtual double area() const;
    virtual double delay() const;
    virtual double delayPort(const std::string& port) const
        throw (WrongSubclass, KeyNotFound);

    virtual void setDelay(const std::string& port, double delay)
        throw (WrongSubclass);

    virtual double energyActive() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyIdle() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyOperation(const std::string& name) const
        throw (WrongSubclass, KeyNotFound);
    virtual bool hasEnergyOperation(const std::string& name) const
        throw (WrongSubclass);
    virtual double energyRead() const 
        throw (WrongSubclass, KeyNotFound);
    virtual double energyWrite() const 
        throw (WrongSubclass, KeyNotFound);
    virtual double energyReadWrite(int reads, int writes) const
        throw (WrongSubclass, KeyNotFound);

    virtual void setEnergyActive(double energy)
        throw (WrongSubclass);
    virtual void setEnergyIdle(double energy)
        throw (WrongSubclass);
    virtual void setEnergyOperation(const std::string& name, double energy)
        throw (WrongSubclass);
    virtual void setEnergyRead(double energy)
        throw (WrongSubclass);
    virtual void setEnergyWrite(double energy)
        throw (WrongSubclass);
    virtual void setEnergyReadWrite(int reads, int writes, double energy)
        throw (WrongSubclass);

protected:
    virtual bool hasEnergy(const std::string& key) const;
    virtual double findEnergy(const std::string& key) const throw (KeyNotFound);
    virtual void addEnergy(const std::string& name, double energy);
    virtual bool hasDelay(const std::string& key) const;
    virtual double findDelay(const std::string& key) const throw (KeyNotFound);
    virtual void addDelay(const std::string& name, double delay);
    virtual CostDBEntryStats* createStats() const;

    /// String for active energy.
    static const std::string ENERGY_ACTIVE;
    /// String for idle energy.
    static const std::string ENERGY_IDLE;
    /// String for read energy.
    static const std::string ENERGY_READ;
    /// String for write energy.
    static const std::string ENERGY_WRITE;
    /// String for reads and writes energy.
    static const std::string ENERGY_READ_WRITE;

private:
    /// Map of energies.
    typedef std::map<const std::string, double> EnergyMap;
    /// Map of delays.
    typedef std::map<const std::string, double> DelayMap;

    /// area in gates.
    double area_;
    /// delay in nanoseconds.
    double delay_;
    /// input and output delays in nanoseconds.
    DelayMap delays_;
    /// Energies of one specific type of usage of a resource.
    /// Usually the map contains energies for active and idle usage of
    /// a resource. However, in case of FUs the active energy does not
    /// exist and but energies are defined for each operation.
    EnergyMap energies_;

    /// Copying not allowed.
    CostDBEntryStats(const CostDBEntryStats&);
    /// Assignment not allowed.
    CostDBEntryStats& operator=(const CostDBEntryStats&);
};

#include "CostDBEntryStats.icc"

#endif
