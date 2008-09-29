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
 * @file SQLiteQueryResult.cc
 *
 * Implementation of SQLiteQueryResult class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: red
 */

#include <string>
using std::string;

#include "Application.hh"
#include "SQLiteQueryResult.hh"
#include "DataObject.hh"
#include "SQLiteConnection.hh"

#include "Conversion.hh"

/**
 * Constructor.
 *
 * @param virtualMachine Compiled SQLite virtual machine for the query.
 */
SQLiteQueryResult::SQLiteQueryResult(sqlite3_stmt* statement,
				     SQLiteConnection* connection) :
    statement_(statement), connection_(connection) {

    statement_ = statement;
    // initialize columnNames_ and nextData_
    next();
}

/**
 * Destructor.
 *
 * SQLite virtual machine is freed.
 *
 */
SQLiteQueryResult::~SQLiteQueryResult() {
    try {
        connection_->finalizeQuery(statement_);
    } catch (const RelationalDBException& e) {
        Application::writeToErrorLog(
            __FILE__, __LINE__, "~SQLiteQueryResult()", e.errorMessage());
    }
}


/**
 * Returns the number of columns in the result set.
 *
 * @return Number of columns, UNKNOWN_INDEX if unknown (if not supported by
 * the driver).
 */
int
SQLiteQueryResult::columns() const {
    return columnNames_.size();
}

/**
 * Returns the name (title) of a column in the result set.
 *
 * @param columnIndex Index of the column of which title interested.
 * @return Title. Empty string if unknown (if not supported by the driver or
 * index out of bounds).
 */
std::string
SQLiteQueryResult::columnName(std::size_t columnIndex) const {
    if (columnIndex >= columnNames_.size()) {
        return "";
    }

    return columnNames_[columnIndex];
}

/**
 * Returns the data of a column in the current row in the result set.
 *
 * @param columnIndex Index of the column of which data to return.
 * @return The data. Returns NullDataObject if the index is out of bounds.
 *
 */
const DataObject&
SQLiteQueryResult::data(std::size_t columnIndex) const {

    if (currentData_.size() == 0 || columnIndex >= currentData_.size()) {
        return NullDataObject::instance();
    }
    return currentData_.at(columnIndex);
}

/**
 * Returns the data of a column in the current row in the result set.
 *
 * This implementation is just to silence Intel compiler's warnings about
 * "partial implementation" of data() (because it's overriden function).
 *
 * @param name Name of the column of which data to return.
 * @return The data. Returns NullDataObject if the column cannot be found.
 *         Also returns NullDataObject in case the feature is not supported
 *         by the database implementation.
 *
 */
const DataObject&
SQLiteQueryResult::data(const std::string& name) const {
    return RelationalDBQueryResult::data(name);
}


/**
 * Queries if the result set contains more rows.
 *
 * @return True if there are more rows that can be accessed with next().
 */
bool
SQLiteQueryResult::hasNext() const {
    return nextData_.size() > 0;
}

/**
 * Advances the row cursor to next row in the result set.
 *
 * In case the current row is the last row this method does nothing but
 * returns false.
 *
 * @return True if there are still more rows to fetch.
 */
bool
SQLiteQueryResult::next() {

    if (currentData_.size() > 0 && !hasNext()) {
        return false;
    }

    std::vector<std::string> columnNames;

    assert(statement_ != NULL);

    int columnCount = 0;
    int dataCount = 0;

    int result = sqlite3_step(statement_);
    columnCount = sqlite3_column_count(statement_);
    for (int i = 0; i < columnCount; i++) {
        columnNames.push_back(sqlite3_column_name(statement_, i));
    }
    currentData_ = nextData_;
    if (result == SQLITE_ROW) {
        nextData_.clear();
        dataCount = sqlite3_data_count(statement_);
        for (int i = 0; i < dataCount; i++) {
            char* columnText = (char*)sqlite3_column_text(statement_, i);
            DataObject data;
            if (columnText == NULL) {
                data.setNull();
            } else {
                data.setString(columnText);
            }
            nextData_.push_back(data);
        }
    } else if (result == SQLITE_DONE) {
        nextData_.clear();
    } else {
        // error occured
        nextData_.clear();
        return false;
    }

    // check if it's the initialization call when one should save column
    // names and column count
    if (columnNames_.size() == 0 && columnNames.size() != 0) {
        columnNames_ = columnNames;
    }
    return nextData_.size() > 0;
}

