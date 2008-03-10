/**
 * @file RelationalDB.hh
 *
 * Class definition of RelationalDB interface.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#include <string>

#ifndef TTA_RELATIONAL_DB_HH
#define TTA_RELATIONAL_DB_HH

#include "Exception.hh"

class RelationalDBConnection;

/**
 * Classes that implement this interface can be used as relational database
 * handlers.
 */
class RelationalDB {
public:
    virtual ~RelationalDB();

    virtual RelationalDBConnection& connect(
	const std::string& database,
	const std::string& login = "",
	const std::string& password = "",
	bool readOnly = false)
	throw (RelationalDBException) = 0;

    virtual void close(const RelationalDBConnection& connection)
	throw (RelationalDBException) = 0;

};

#endif

