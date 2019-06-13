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
 * @file RelationalDBConnection.cc
 *
 * Empty implementation of the interface, just to be able to add Doxygen
 * comments for methods.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: red
 */

#include "RelationalDBConnection.hh"

/**
 * Destructor.
 */
RelationalDBConnection::~RelationalDBConnection() {
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
RelationalDBConnection::updateQuery(const std::string&) {
    return 0;
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
RelationalDBConnection::DDLQuery(const std::string&) {}

/**
 * Performs a data retrieval query (SELECT).
 *
 * @param queryString The query string.
 * @return A handle to the query result set. Caller owns the instance. Deleting
 * the instance should free all resources connected to the query.
 * @exception RelationalDBException In case a database error occured.
 */
RelationalDBQueryResult*
RelationalDBConnection::query(const std::string&, bool /*init*/) {
    return NULL;
}

/**
 * Starts a new database transaction.
 *
 * Also ends the possible previous transaction successfully (commit).
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
RelationalDBConnection::beginTransaction() {}

/**
 * Ends the current database transaction unsuccessfully and rollbacks all
 * the changes the queries in the transaction would have done.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
RelationalDBConnection::rollback() {}

/**
 * Ends the current database transaction successfully and commits all
 * the changes in the transaction to the database.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
RelationalDBConnection::commit() {}

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
RelationalDBConnection::tableExistsInDB(const std::string&) {
    return false;
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
RelationalDBConnection::rowCountInTable(const std::string&) {
    return false;
}
