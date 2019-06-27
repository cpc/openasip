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
 * @file CostDBEntryStatsFU.hh
 *
 * Declaration of CostDBEntryStatsFU class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari M�ntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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

    virtual double energyActive() const;
    virtual double energyOperation(const std::string& name) const;
    virtual bool hasEnergyOperation(const std::string& name) const;

    virtual void setEnergyActive(double energy);
    virtual void setEnergyOperation(const std::string& name, double energy);

protected:
    virtual CostDBEntryStats* createStats() const;

private:
    /// Copying not allowed.
    CostDBEntryStatsFU(const CostDBEntryStatsFU&);
    /// Assignment not allowed.
    CostDBEntryStatsFU& operator=(const CostDBEntryStatsFU&);
};

#endif
