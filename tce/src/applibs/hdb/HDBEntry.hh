/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file HDBEntry.hh
 *
 * Declaration of HDBEntry class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
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
    
