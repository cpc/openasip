/**
 * @file RelationalDBQueryResult.cc
 *
 * Empty implementation of the interface, just to be able to add Doxygen
 * comments for methods.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: red
 * @todo Change returning UNKNOWN_INDEX to throwing an exception.
 */

#include "RelationalDBQueryResult.hh"

#include "DataObject.hh"

/**
 * Destructor.
 */
RelationalDBQueryResult::~RelationalDBQueryResult() {
}

/**
 * Returns the number of columns in the result set.
 *
 * @return Number of columns, UNKNOWN_INDEX if unknown (if not supported by
 * the driver).
 */
int
RelationalDBQueryResult::columns() const {
    return UNKNOWN_INDEX;
}

/**
 * Returns the index of column with the given name.
 *
 * @param name Name of the column.
 * @return Index of the column with given name. UNKNOWN_INDEX if not found.
 */
int
RelationalDBQueryResult::column(const std::string& name) const {
    for (int i = 0; i < columns(); ++i) {
        if (columnName(i) == name) {
            return i;
        }
    }
    return UNKNOWN_INDEX;
}

/**
 * Returns the name (title) of a column in the result set.
 *
 * @param columnIndex Index of the column of which title interested.
 * @return Title. Empty string if unknown (if not supported by the driver or
 * index out of bounds).
 */
std::string
RelationalDBQueryResult::columnName(std::size_t) const {
    return "";
}

/**
 * Returns the data of a column in the current row in the result set.
 *
 * Returns a DataObject with isNull() set in case the value from DB
 * is NULL.
 *
 * @param columnIndex Index of the column of which data to return.
 * @return The data. Returns NullDataObject if the index is out of bounds.
 *
 */
const DataObject&
RelationalDBQueryResult::data(std::size_t) const {
    return NullDataObject::instance();
}

/**
 * Returns the data of a column in the current row in the result set.
 *
 * @param name Name of the column of which data to return.
 * @return The data. Returns NullDataObject if the column cannot be found.
 *         Also returns NullDataObject in case the feature is not supported
 *         by the database implementation.
 *
 */
const DataObject&
RelationalDBQueryResult::data(const std::string& name) const {

    int columnIndex = column(name);
    if (columnIndex != UNKNOWN_INDEX) {
        return data(columnIndex);
    }

    return NullDataObject::instance();
}

/**
 * Queries if the result set contains more rows.
 *
 * @return True if there are more rows that can be accessed with next().
 */
bool
RelationalDBQueryResult::hasNext() const {
    return false;
}

/**
 * Advances the row cursor to next row in the result set.
 *
 * In case the current row is the last row this method does nothing.
 *
 * @return True if there are still more rows to fetch. *
 */
bool
RelationalDBQueryResult::next() {
    return false;
}
