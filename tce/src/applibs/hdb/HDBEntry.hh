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
    
