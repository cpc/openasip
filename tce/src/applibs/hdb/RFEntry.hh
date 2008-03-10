/**
 * @file RFEntry.hh
 *
 * Declaration of RFEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_ENTRY_HH
#define TTA_RF_ENTRY_HH

#include "Exception.hh"
#include "HDBEntry.hh"

namespace HDB {

class RFArchitecture;
class RFImplementation;

/**
 * Represents an RF entry in HDB.
 */
class RFEntry : public HDBEntry {
public:
    RFEntry();
    virtual ~RFEntry();

    virtual bool hasImplementation() const;
    void setImplementation(RFImplementation* implementation);
    RFImplementation& implementation() const
        throw (NotAvailable);

    virtual bool hasArchitecture() const;
    void setArchitecture(RFArchitecture* architecture);
    RFArchitecture& architecture() const
        throw (NotAvailable);

private:
    /// Architecture of the entry.
    RFArchitecture* architecture_;
    /// Implementation of the entry.
    RFImplementation* implementation_;
};
}

#endif

