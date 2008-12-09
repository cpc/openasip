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
 * @file CostDBEntryStatsRF.hh
 *
 * Declaration of CostDBEntryStatsRF class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_STATS_RF_HH
#define TTA_COSTDB_ENTRY_STATS_RF_HH


#include "CostDBEntryStats.hh"
#include "Exception.hh"


/**
 * Stores area, delay and energy statistics of register files.
 */
class CostDBEntryStatsRF: public CostDBEntryStats {
public:
    CostDBEntryStatsRF(double areaData, double delayData);
    virtual ~CostDBEntryStatsRF();
    CostDBEntryStatsRF(
        const CostDBEntryStatsRF& stats1,
        const CostDBEntryStatsRF& stats2,
        double coefficient);

    virtual double energyActive() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyRead() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyWrite() const
        throw (WrongSubclass, KeyNotFound);
    virtual double energyReadWrite(int reads, int writes) const
        throw (WrongSubclass, KeyNotFound);

    virtual void setEnergyActive(double energy)
        throw (WrongSubclass);
    virtual void setEnergyRead(double energy)
        throw (WrongSubclass);
    virtual void setEnergyWrite(double energy)
        throw (WrongSubclass);
    virtual void setEnergyReadWrite(int reads, int writes, double energy)
        throw (WrongSubclass);

protected:
    virtual CostDBEntryStats* createStats() const;

private:
    /// Map of energies.
    typedef std::map<const std::string, double> EnergyMap;
    /// Map of delays.
    typedef std::map<const std::string, double> DelayMap;

    static std::string generateReadWriteString(int reads, int writes);

    /// Copying not allowed.
    CostDBEntryStatsRF(const CostDBEntryStatsRF&);
    /// Assignment not allowed.
    CostDBEntryStatsRF& operator=(const CostDBEntryStatsRF&);
};

#endif
