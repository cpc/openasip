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
