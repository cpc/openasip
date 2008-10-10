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
RelationalDBConnection::updateQuery(const std::string&)
    throw (RelationalDBException) {
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
RelationalDBConnection::DDLQuery(const std::string&)
    throw (RelationalDBException) {
}

/**
 * Performs a data retrieval query (SELECT).
 *
 * @param queryString The query string.
 * @return A handle to the query result set. Caller owns the instance. Deleting
 * the instance should free all resources connected to the query.
 * @exception RelationalDBException In case a database error occured.
 */
RelationalDBQueryResult*
RelationalDBConnection::query(const std::string&)
    throw (RelationalDBException) {
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
RelationalDBConnection::beginTransaction()
    throw (RelationalDBException) {
}

/**
 * Ends the current database transaction unsuccessfully and rollbacks all
 * the changes the queries in the transaction would have done.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
RelationalDBConnection::rollback()
    throw (RelationalDBException) {
}

/**
 * Ends the current database transaction successfully and commits all
 * the changes in the transaction to the database.
 *
 * @exception RelationalDBException In case a database error occured.
 */
void
RelationalDBConnection::commit()
    throw (RelationalDBException) {
}
