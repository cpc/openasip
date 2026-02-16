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
 * @file CostDBEntryStats.hh
 *
 * Declaration of CostDBEntryStats class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
        double coefficient);
    virtual CostDBEntryStats* copy() const;

    virtual double area() const;
    virtual double delay() const;
    virtual double delayPort(const std::string& port) const;

    virtual void setDelay(const std::string& port, double delay);

    virtual double energyActive() const;
    virtual double energyIdle() const;
    virtual double energyOperation(const std::string& name) const;
    virtual bool hasEnergyOperation(const std::string& name) const;
    virtual double energyRead() const;
    virtual double energyWrite() const;
    virtual double energyReadWrite(int reads, int writes) const;

    virtual void setEnergyActive(double energy);
    virtual void setEnergyIdle(double energy);
    virtual void setEnergyOperation(const std::string& name, double energy);
    virtual void setEnergyRead(double energy);
    virtual void setEnergyWrite(double energy);
    virtual void setEnergyReadWrite(int reads, int writes, double energy);

protected:
    virtual bool hasEnergy(const std::string& key) const;
    virtual double findEnergy(const std::string& key) const;
    virtual void addEnergy(const std::string& name, double energy);
    virtual bool hasDelay(const std::string& key) const;
    virtual double findDelay(const std::string& key) const;
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

    CostDBEntryStats(const CostDBEntryStats&) = delete;
    CostDBEntryStats& operator=(const CostDBEntryStats&) = delete;

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
};

#include "CostDBEntryStats.icc"

#endif
