/**
 * @file HDBEntry.hh
 *
 * Declaration of HDBEntry class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_ENTRY_HH
#define TTA_HDB_ENTRY_HH

#include "DBTypes.hh"
#include "Exception.hh"

namespace HDB {

class CostFunctionPlugin;

/**
 * An abstract base class for FUEntry and RFEntry.
 */
class HDBEntry {
public:
    virtual ~HDBEntry();

    bool hasID() const;
    void setID(RowID id);
    RowID id() const
        throw (NotAvailable);

    virtual bool hasArchitecture() const = 0;
    virtual bool hasImplementation() const = 0;

    bool hasCostFunction() const;
    CostFunctionPlugin& costFunction() const
        throw (NotAvailable);
    void setCostFunction(CostFunctionPlugin* costFunction);

    std::string hdbFile() const;
    void setHDBFile(const std::string& file);

protected:
    HDBEntry();

private:
    /// Tells whether the entry has an ID set.
    bool hasID_;
    /// ID of the entry in HDB.
    RowID id_;
    /// Cost function of the entry.
    CostFunctionPlugin* costFunction_;
    /// The HDB file that contains the entry.
    std::string hdbFile_;
};
}

#endif
    
