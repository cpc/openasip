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
 * @file RelationalDBQueryResult.hh
 *
 * Class definition of RelationalDBQueryResult interface.
 *
 * @author Pekka Jääskeläinen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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
    virtual bool hasNext() = 0;
    virtual bool next() = 0;
    virtual void bindInt(unsigned int position, int value);
    virtual void bindString(unsigned int position, const std::string& value);
    virtual void reset();
};

#endif

