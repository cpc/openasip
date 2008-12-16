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
 * @file SQLiteQueryResult.hh
 *
 * Class definition of SQLiteQueryResult class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
    SQLiteQueryResult(
        sqlite3_stmt* statement,
        SQLiteConnection* connection,
        bool init = true);
    virtual ~SQLiteQueryResult();

    virtual int columns() const;
    virtual std::string columnName(std::size_t columnIndex) const;
    virtual const DataObject& data(std::size_t columnIndex) const;
    virtual const DataObject& data(const std::string& name) const;
    virtual bool hasNext();
    virtual bool next();
    virtual void bindInt(unsigned int position, int value);
    virtual void bindString(unsigned int position, const std::string& value);
    virtual void reset();

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
    /// has next() been called for this query
    bool dataInitialized_;
};

#endif
