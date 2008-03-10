/**
 * @file SQLite.cc
 *
 * Implementation of SQLite class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
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
