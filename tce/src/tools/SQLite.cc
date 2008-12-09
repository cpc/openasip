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
