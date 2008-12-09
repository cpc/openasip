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
 * @file NullSocketCodeTable.hh
 *
 * Declaration of NullSocketCodeTable class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_SOCKET_CODE_TABLE_HH
#define TTA_NULL_SOCKET_CODE_TABLE_HH

#include "SocketCodeTable.hh"

/**
 * A null version of SocketCodeTable class.
 */
class NullSocketCodeTable : public SocketCodeTable {
public:
    static NullSocketCodeTable& instance();

private:
    NullSocketCodeTable();
    virtual ~NullSocketCodeTable();

    /// The only instance.
    static NullSocketCodeTable instance_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
