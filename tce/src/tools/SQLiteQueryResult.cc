/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SQLiteQueryResult.cc
 *
 * Implementation of SQLiteQueryResult class.
 *
 * @author Pekka J��skel�inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
SQLiteQueryResult::SQLiteQueryResult(
    sqlite3_stmt* statement,
    SQLiteConnection* connection,
    bool init) :
    statement_(statement), 
    connection_(connection),
    dataInitialized_(init) {

    // initialize columnNames_ and nextData_
    if (init) {
        next();
    }
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
SQLiteQueryResult::hasNext() {
    if (!dataInitialized_) {
        next();
    }
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

    dataInitialized_ = true;
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

/**
 * Binds int to sqlite statement at given position (1->)
 */
void
SQLiteQueryResult::bindInt(unsigned int position, int value) {
    connection_->throwIfSQLiteError(sqlite3_bind_int(statement_, position, value));
}

/**
 * Binds string to sqlite statement at given position (1->)
 */
void
SQLiteQueryResult::bindString(unsigned int position, const std::string& value) {
    connection_->throwIfSQLiteError(sqlite3_bind_text(statement_, position, value.c_str(),
                -1, NULL));
}

/**
 * Resets compiled sqlite statement for new bindings and execution.
 */
void
SQLiteQueryResult::reset() {
    connection_->throwIfSQLiteError(sqlite3_reset(statement_));
    // reset doesn't clear bindings
    //connection_->throwIfSQLiteError(sqlite3_clear_bindings(statement_));
    columnNames_.clear();
    currentData_.clear();
    nextData_.clear();
    dataInitialized_ = false;
}

