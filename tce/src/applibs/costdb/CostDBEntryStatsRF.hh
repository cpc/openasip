/**
 * @file CostDBEntryStatsRF.hh
 *
 * Declaration of CostDBEntryStatsRF class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
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
