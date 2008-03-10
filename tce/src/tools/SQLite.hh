/**
 * @file SQLite.hh
 *
 * Class definition of SQLite class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_SQLITE_DB_HH
#define TTA_SQLITE_DB_HH

#include <string>

#include "RelationalDB.hh"
#include "SQLiteConnection.hh"

/**
 * Implementation of RelationalDB interface for SQLite library.
 */
class SQLite : public RelationalDB {
public:
    SQLite();
    virtual ~SQLite();

    virtual RelationalDBConnection& connect(
	const std::string& database,
	const std::string& login = "",
	const std::string& password = "",
	bool readOnly = false)
	throw (RelationalDBException);

    virtual void close(const RelationalDBConnection& connection)
	throw (RelationalDBException);

private:

    /// Only one simultaneous active connection per SQLite instance allowed
    /// currently.
    SQLiteConnection* activeConnection_;

};

#endif

