/**
 * @file RelationalDBConnection.hh
 *
 * Class definition of RelationalDBConnection interface.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_RELATIONAL_DB_CONNECTION_HH
#define TTA_RELATIONAL_DB_CONNECTION_HH

#include <string>

#include "Exception.hh"
#include "DBTypes.hh"

class RelationalDBQueryResult;

/**
 * Classes that implement this interface can be used as relational database
 * connection handles.
 */
class RelationalDBConnection {
public:
    virtual ~RelationalDBConnection();

    virtual int updateQuery(const std::string& queryString)
	throw (RelationalDBException) = 0;
    virtual void DDLQuery(const std::string& queryString)
	throw (RelationalDBException) = 0;
    virtual RelationalDBQueryResult* query(const std::string& queryString)
	throw (RelationalDBException) = 0;

    virtual void beginTransaction()
	throw (RelationalDBException) = 0;
    virtual void rollback()
	throw (RelationalDBException) = 0;
    virtual void commit()
	throw (RelationalDBException) = 0;

    virtual RowID lastInsertRowID() = 0;
};

#endif

