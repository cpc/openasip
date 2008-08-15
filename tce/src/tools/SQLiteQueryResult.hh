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
 * @file SQLiteQueryResult.hh
 *
 * Class definition of SQLiteQueryResult class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_SQLITE_QUERY_RESULT_HH
#define TTA_SQLITE_QUERY_RESULT_HH

#include <vector>
#include <string>
#include <cstddef>
#include "sqlite3.h"

#include "SQLiteConnection.hh"
#include "RelationalDBQueryResult.hh"
#include "DataObject.hh"

/**
 * Implementation of RelationalDBQueryResult interface for SQLite.
 *
 */
class SQLiteQueryResult : public RelationalDBQueryResult {
public:
    SQLiteQueryResult(sqlite3_stmt* statement,
		      SQLiteConnection* connection);
    virtual ~SQLiteQueryResult();

    virtual int columns() const;
    virtual std::string columnName(std::size_t columnIndex) const;
    virtual const DataObject& data(std::size_t columnIndex) const;
    virtual const DataObject& data(const std::string& name) const;
    virtual bool hasNext() const;
    virtual bool next();

private:
    /// single DataObject is used to return the column data with data()
    mutable DataObject columnData_;
    /// the compiled SQLite statement handle
    sqlite3_stmt* statement_;
    /// sqlite connection handle
    SQLiteConnection* connection_;
    /// column names
    std::vector<std::string> columnNames_;
    /// data of the current row
    std::vector<DataObject> currentData_;
    /// data of the next row
    std::vector<DataObject> nextData_;
};

#endif
