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
 * @file SQLite.cc
 *
 * Implementation of SQLite class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: red
 */

#include <string>
using std::string;

#include "sqlite3.h"

#include "SQLite.hh"

/**
 * Constructor.
 */
SQLite::SQLite() : activeConnection_(NULL) {
}

/**
 * Destructor.
 */
SQLite::~SQLite() {
    close(*activeConnection_);
}

/**
 * Connects to a database.
 *
 * Possible old connection is closed. SQLite implementation doesn't support
 * multiple active connections currently. If database file is not found,
 * creation of new database file with the given name is attempted.
 *
 * @param database The filename of the SQLite database.
 * @param login Login name to use for the database connection.
 * @param password Password to use for the database connection.
 * @param readOnly Should the database be opened in read-only mode only?
 * @return Connection "handle" which is used to query the DB, etc.
 * @exception RelationalDBException Thrown if connection failed.
 */
RelationalDBConnection&
SQLite::connect(
    const std::string& database,
    const std::string&,
    const std::string&,
    bool)
    throw (RelationalDBException) {

    if (activeConnection_ != NULL) {
        close(*activeConnection_);
        activeConnection_ = NULL;
    }

    sqlite3* connection = NULL;
    int value = sqlite3_open(database.c_str(), &connection);

    if (value != SQLITE_OK) {
        string error = sqlite3_errmsg(connection);
        throw RelationalDBException(
            __FILE__, __LINE__, "SQLite::connect()", error);
    }

    activeConnection_ = new SQLiteConnection(connection);
    return *activeConnection_;
}

/**
 * Closes a database connection.
 *
 * Close should be idempotent, that is, it can be called arbitrary amount of
 * times to a connection without an error.
 *
 * @param connection The database connection to close.
 * @exception RelationalDBException Thrown if closing the connection failed.
 */
void
SQLite::close(const RelationalDBConnection&)
    throw (RelationalDBException) {

    if (activeConnection_ == NULL) {
        return;
    }

    delete activeConnection_;
    activeConnection_ = NULL;
}
