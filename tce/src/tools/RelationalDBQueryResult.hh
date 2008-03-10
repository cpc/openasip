/**
 * @file RelationalDBQueryResult.hh
 *
 * Class definition of RelationalDBQueryResult interface.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 */

#ifndef TTA_RELATIONAL_DB_QUERY_RESULT_HH
#define TTA_RELATIONAL_DB_QUERY_RESULT_HH

#include <string>
#include <cstddef>

class DataObject;

/**
 * Classes that implement this interface can be used as relational database
 * query result handles.
 */
class RelationalDBQueryResult {
public:
    virtual ~RelationalDBQueryResult();

    static const int UNKNOWN_INDEX = -1;
    virtual int columns() const;
    virtual int column(const std::string& name) const;
    virtual std::string columnName(std::size_t columnIndex) const;
    virtual const DataObject& data(std::size_t column) const = 0;
    virtual const DataObject& data(const std::string& name) const;
    virtual bool hasNext() const = 0;
    virtual bool next() = 0;
};

#endif

