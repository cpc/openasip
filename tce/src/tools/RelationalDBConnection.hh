/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file RelationalDBConnection.hh
 *
 * Class definition of RelationalDBConnection interface.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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

