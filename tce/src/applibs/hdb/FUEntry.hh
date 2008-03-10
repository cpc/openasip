/**
 * @file FUEntry.hh
 *
 * Declaration of FUEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_ENTRY_HH
#define TTA_FU_ENTRY_HH

#include <string>

#include "HDBEntry.hh"
#include "Exception.hh"

namespace HDB {

class FUImplementation;
class FUArchitecture;

/**
 * Represents an FU entry in HDB.
 */
class FUEntry : public HDBEntry {
public:
    FUEntry();
    virtual ~FUEntry();

    virtual bool hasImplementation() const;
    FUImplementation& implementation() const
        throw (NotAvailable);
    void setImplementation(FUImplementation* implementation);
    
    virtual bool hasArchitecture() const;
    FUArchitecture& architecture() const
        throw (NotAvailable);
    void setArchitecture(FUArchitecture* architecture);

private:
    /// Architecture of the entry.
    FUArchitecture* architecture_;
    /// Implementation of the entry.
    FUImplementation* implementation_;
};
}

#endif
