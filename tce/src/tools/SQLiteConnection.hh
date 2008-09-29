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

