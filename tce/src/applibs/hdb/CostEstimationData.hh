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
 * @file CostEstimationData.hh
 *
 * Declaration of CostEstimationData class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
    std::string name() const;

    void setValue(const DataObject& value);
    bool hasValue() const;
    DataObject value() const;

    void setFUReference(RowID fuEntryID);
    bool hasFUReference() const;
    RowID fuReference() const;

    void setRFReference(RowID rfEntryID);
    bool hasRFReference() const;
    RowID rfReference() const;

    void setBusReference(RowID busEntryID);
    bool hasBusReference() const;
    RowID busReference() const;

    void setSocketReference(RowID socketEntryID);
    bool hasSocketReference() const;
    RowID socketReference() const;

    void setPluginID(RowID pluginID);
    bool hasPluginID() const;
    RowID pluginID() const;

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
