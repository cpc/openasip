/*
    Copyright (c) 2002-2014 Tampere University of Technology.

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
 * @file SQLiteConnection.cc
 *
 * Implementation of SQLiteConnection class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: red
 */

#include <string>
using std::string;

#include "SQLiteConnection.hh"
#include "SQLiteQueryResult.hh"

/**
 * Constructor.
 *
 * @param connection A pointer to a SQLite connection handle.
 */
SQLiteConnection::SQLiteConnection(sqlite3* connection) :
    connection_(connection), transactionActive_(false) {
}

/**
 * Destructor.
 *
 * Closes the connection.
 */
SQLiteConnection::~SQLiteConnection() {
    sqlite3_close(connection_);
}

/**
 * Performs a query that changes the database (UPDATE/INSERT/DELETE).
 *
 * Does not autocommit the changes in case the update is in the middle of
 * transaction.
 *
 * @param queryString The query string.
 * @return Number of rows affected by the change.
 * @exception RelationalDBException In case a database error occured.
 */
int
SQLiteConnection::updateQuery(const std::string& queryString) {
    if (connection_ == NULL) {
        throw RelationalDBException(
            __FILE__, __LINE__, "SQLiteConnection::updateQuery()",
            "Not connected!");
    }

    // "virtual machine" used by SQLite to execute the statements
    sqlite3_stmt* stmt = compileQuery(queryString);

    assert(stmt != NULL);

    throwIfSQLiteError(sqlite3_step(stmt));
    finalizeQuery(stmt);

    return sqlite3_changes(connection_);
}

/**
 * Performs a SQL Data Definition Language query, that is a query that may
 * change the structure of the database (CREATE TABLE, etc.).
 *
 * Does not autocommit the changes in case the update is in the middle of
 * transaction.
 *
 * @param queryString The query string.
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::DDLQuery(const std::string& queryString) {
    updateQuery(queryString);
}

/**
 * Performs a data retrieval query (SELECT).
 *
 * @param queryString The query string.
 * @return A handle to the query result set.
 * @exception RelationalDBException In case a database error occured.
 */
RelationalDBQueryResult*
SQLiteConnection::query(const std::string& queryString, bool init) {
    sqlite3_stmt* stmt = compileQuery(queryString);
    SQLiteQueryResult* result = new SQLiteQueryResult(stmt, this, init);
    return result;
}

/**
 * Starts a new database transaction.
 *
 * Also ends the possible previous transaction successfully (commit).
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::beginTransaction() {
    if (transactionActive_) {
        commit();
    }
    updateQuery("BEGIN;");
    transactionActive_ = true;
}

/**
 * Ends the current database transaction unsuccessfully and rollbacks all
 * the changes the queries in the transaction would have done.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::rollback() {
    updateQuery("ROLLBACK;");
    transactionActive_ = false;
}

/**
 * Ends the current database transaction successfully and commits all
 * the changes in the transaction to the database.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
SQLiteConnection::commit() {
    updateQuery("COMMIT;");
    transactionActive_ = false;
}

/**
 * Returns the row ID of the most recent insert in the database.
 *
 * @return The row ID.
 */
RowID
SQLiteConnection::lastInsertRowID() {
    return sqlite3_last_insert_rowid(connection_);
}


/**
 * Checks if database has given table by name.
 *
 * @param tableName Name of the table
 * @return True if db has the table. Otherwise false.
 * @exception RelationalDBException In case a database error occurred or
 *                                  call was made in the middle of an active
 *                                  transaction.
 */
bool
SQLiteConnection::tableExistsInDB(const std::string& tableName) {
    if (transactionActive_) {
        throw RelationalDBException(__FILE__, __LINE__,
            "SQLiteConnection::tableExistsInDB()",
            "Illegal call during active transaction.");
    }

    string query = "SELECT count(*) "
        "FROM sqlite_master "
        "WHERE type = 'table' and name = '" + tableName + "';";

    RelationalDBQueryResult* result = this->query(query, false);
    assert(result->hasNext());
    result->next();
    const DataObject& count = result->data(0);

    int intBoolValue = 0;

    try {
        intBoolValue = count.integerValue(); // boolValue is zero if DataObject
                                             // has NULL value.
    } catch (NumberFormatException& e) {
        throw RelationalDBException(__FILE__, __LINE__,
            "SQLiteConnection::tableExistsInDB()",
            "Exception from DataObject: " + e.errorMessage());
    }
    return intBoolValue;
}

/**
 * Return number of entries in the given table.
 *
 * @param tableName Name of the table.
 * @return Number of entries in table.
 * @exception RelationalDBException In case a database error occurred,
 * call was made in the middle of an active transaction or the table does not
 * exists.
 */
int
SQLiteConnection::rowCountInTable(const std::string& tableName) {
    if (!tableExistsInDB(tableName)) {
        throw RelationalDBException(__FILE__, __LINE__,
            "SQLiteConnection::rowCountInTable()",
            "Table was not found.");
    }
    string query = "SELECT count(*) FROM " + tableName + ";";

    RelationalDBQueryResult* result = this->query(query, false);
    assert(result->hasNext());
    result->next();
    const DataObject& count = result->data(0);
    int countAsInt = -1;

    try {
        countAsInt = count.integerValue(); // boolValue is zero if DataObject
                                           // has NULL value.
    } catch (NumberFormatException& e) {
        throw RelationalDBException(__FILE__, __LINE__,
            "SQLiteConnection::tableExistsInDB()",
            "Exception from DataObject: " + e.errorMessage());
    }
    assert(countAsInt > -1);
    return countAsInt;
}

/**
 * Throws a RelationalDBException if result value indicates an SQLite error.
 *
 * @param result The value from a SQLite API call.
 * @exception RelationalDBException Thrown if result is not SQLITE_OK.
 */
void
SQLiteConnection::throwIfSQLiteError(int result) {
    if (result != SQLITE_OK && result != SQLITE_ROW &&
        result != SQLITE_DONE) {

        string error = sqlite3_errmsg(connection_);
        throw RelationalDBException(__FILE__, __LINE__, "", error);
    }
}

/**
 * Compiles a SQLite query.
 *
 * @param queryString The SQL statement to compile.
 * @return The SQLite virtual machine that should be used to execute the query.
 * @exception RelationalDBException In case a database error occured.
 */
sqlite3_stmt*
SQLiteConnection::compileQuery(const std::string& queryString) {
    // "virtual machine" used by SQLite to execute the statements
    sqlite3_stmt* stmt = NULL;
    const char* dummy = NULL;

    throwIfSQLiteError(sqlite3_prepare(
        connection_, queryString.c_str(), queryString.length(),
        &stmt, &dummy));
    return stmt;
}

/**
 * Finalizes a SQLite query, frees the virtual machine.
 *
 * Does nothing if the statement is NULL.
 *
 * @param statement The SQLite statement to free.
 * @exception RelationalDBException In case there was errors (can be
 * cumulated from a previous sqlite3_step() call).
 */
void
SQLiteConnection::finalizeQuery(sqlite3_stmt* statement) {
    if (statement == NULL) {
        return;
    }

    throwIfSQLiteError(sqlite3_finalize(statement));
}
