/**
 * @file SQLiteQueryResult.hh
 *
 * Class definition of SQLiteQueryResult class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_SQLITE_QUERY_RESULT_HH
#define TTA_SQLITE_QUERY_RESULT_HH

#include <vector>
#include <string>
#include <cstddef>
#include "sqlite3.h"

#include "SQLiteConnection.hh"
#include "RelationalDBQueryResult.hh"
#include "DataObject.hh"

/**
 * Implementation of RelationalDBQueryResult interface for SQLite.
 *
 */
class SQLiteQueryResult : public RelationalDBQueryResult {
public:
    SQLiteQueryResult(sqlite3_stmt* statement,
		      SQLiteConnection* connection);
    virtual ~SQLiteQueryResult();

    virtual int columns() const;
    virtual std::string columnName(std::size_t columnIndex) const;
    virtual const DataObject& data(std::size_t columnIndex) const;
    virtual const DataObject& data(const std::string& name) const;
    virtual bool hasNext() const;
    virtual bool next();

private:
    /// single DataObject is used to return the column data with data()
    mutable DataObject columnData_;
    /// the compiled SQLite statement handle
    sqlite3_stmt* statement_;
    /// sqlite connection handle
    SQLiteConnection* connection_;
    /// column names
    std::vector<std::string> columnNames_;
    /// data of the current row
    std::vector<DataObject> currentData_;
    /// data of the next row
    std::vector<DataObject> nextData_;
};

#endif
