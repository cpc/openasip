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
 * @file RelationalDBQueryResult.cc
 *
 * Empty implementation of the interface, just to be able to add Doxygen
 * comments for methods.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
RelationalDBQueryResult::hasNext() {
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

/**
 * Binds int type variable to a prepared sql statement.
 */
void
RelationalDBQueryResult::bindInt(
    unsigned int /*position*/, int /*value*/) {
    return;
}

/**
 * Binds string type variable to a prepared sql statement.
 */
void
RelationalDBQueryResult::bindString(
    unsigned int /*position*/, const std::string& /*value*/) {
    return;
}

/**
 * Resets s prepared sql statement.
 */
void
RelationalDBQueryResult::reset() {
    return;
}

