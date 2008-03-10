/**
 * @file SQLiteConnection.hh
 *
 * Class definition of SQLiteConnection class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
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

