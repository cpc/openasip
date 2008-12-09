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
 * @file SQLiteConnection.hh
 *
 * Class definition of SQLiteConnection class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_SQLITE_CONNECTION_HH
#define TTA_SQLITE_CONNECTION_HH

#include <string>
#include "sqlite3.h"

#include "FileSystem.hh"
#include "RelationalDBConnection.hh"
#include "Exception.hh"

/**
 * Implementation of RelationalDBConnection interface for SQLite library.
 */
class SQLiteConnection : public RelationalDBConnection {
public:
    SQLiteConnection(sqlite3* connection);
    virtual ~SQLiteConnection();

    virtual int updateQuery(const std::string& queryString)
	throw (RelationalDBException);
    virtual void DDLQuery(const std::string& queryString)
	throw (RelationalDBException);
    virtual RelationalDBQueryResult* query(const std::string& queryString)
	throw (RelationalDBException);

    virtual void beginTransaction()
	throw (RelationalDBException);
    virtual void rollback()
	throw (RelationalDBException);
    virtual void commit()
	throw (RelationalDBException);

    virtual RowID lastInsertRowID();

    void throwIfSQLiteError(int result)
	throw (RelationalDBException);

    void finalizeQuery(sqlite3_stmt* statement)
	throw (RelationalDBException);

private:

    sqlite3_stmt* compileQuery(const std::string& queryString)
	throw (RelationalDBException);

    /// SQLite connection handle is saved to this
    sqlite3* connection_;

    /// data in the "current" row in the result set, NULL if there's none
    const char** currentData_;

    /// data in the possible next row in the result set, NULL if there's none
    const char** nextData_;

    bool transactionActive_;
};

#endif

