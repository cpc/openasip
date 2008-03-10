/**
 * @file HWBlockArchitecture.hh
 *
 * Declaration of HWBlockArchitecture class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HW_BLOCK_ARCHITECTURE_HH
#define TTA_HW_BLOCK_ARCHITECTURE_HH

#include "DBTypes.hh"
#include "Exception.hh"

/**
 * An abstract base class for FUArchitecture and RFArchitecture.
 */
class HWBlockArchitecture {
public:
    virtual ~HWBlockArchitecture();

    bool hasID() const;
    void setID(RowID id);
    RowID id() const
        throw (NotAvailable);

protected:
    HWBlockArchitecture();

private:
    bool hasID_;
    RowID id_;
};

#endif
