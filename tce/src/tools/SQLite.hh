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
 * @file SQLite.hh
 *
 * Class definition of SQLite class.
 *
 * @author Pekka J��skel�inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
        const std::string& database, const std::string& login = "",
        const std::string& password = "", bool readOnly = false);

    virtual void close(const RelationalDBConnection& connection);

private:
    /// Only one simultaneous active connection per SQLite instance allowed
    /// currently.
    SQLiteConnection* activeConnection_;

};

#endif

