/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file CostEstimationData.hh
 *
 * Declaration of CostEstimationData class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
