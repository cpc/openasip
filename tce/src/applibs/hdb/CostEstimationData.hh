/**
 * @file CostEstimationData.hh
 *
 * Declaration of CostEstimationData class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATION_DATA_HH
#define TTA_COST_ESTIMATION_DATA_HH

#include "DBTypes.hh"
#include "DataObject.hh"

/**
 * Class that represents one row in the cost estimation data table.
 */
class CostEstimationData {
public:
    CostEstimationData();
    virtual ~CostEstimationData();

    void setName(const std::string& name);
    bool hasName() const;
    std::string name() const
        throw (NotAvailable);

    void setValue(const DataObject& value);
    bool hasValue() const;
    DataObject value() const
        throw (NotAvailable);

    void setFUReference(RowID fuEntryID);
    bool hasFUReference() const;
    RowID fuReference() const
        throw (NotAvailable);

    void setRFReference(RowID rfEntryID);
    bool hasRFReference() const;
    RowID rfReference() const
        throw (NotAvailable);

    void setBusReference(RowID busEntryID);
    bool hasBusReference() const;
    RowID busReference() const
        throw (NotAvailable);

    void setSocketReference(RowID socketEntryID);
    bool hasSocketReference() const;
    RowID socketReference() const
        throw (NotAvailable);

    void setPluginID(RowID pluginID);
    bool hasPluginID() const;
    RowID pluginID() const
        throw (NotAvailable);

private:
    /// Is the data name set?
    bool hasName_;
    /// Name of the data.
    std::string name_;

    /// Is the value set?
    bool hasValue_;
    /// Value data.
    DataObject value_;

    /// Is the fu reference set?
    bool hasFUReference_;
    /// FU reference.
    RowID fuReference_;

    /// Is the RF reference set?
    bool hasRFReference_;
    /// RF reference.
    RowID rfReference_;

    /// Is the bus reference set?
    bool hasBusReference_;
    /// Bus reference.
    RowID busReference_;

    /// Is the socket reference set?
    bool hasSocketReference_;
    /// Socket reference.
    RowID socketReference_;

    /// Is the plugin ID set?
    bool hasPluginID_;
    /// Plugin ID.
    RowID pluginID_;
};

#include "CostEstimationData.icc"

#endif
