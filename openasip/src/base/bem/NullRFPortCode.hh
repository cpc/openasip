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
 * @file NullRFPortCode.hh
 *
 * Declaration of NullRFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_RF_PORT_CODE_HH
#define TTA_NULL_RF_PORT_CODE_HH

#include "RFPortCode.hh"

class SocketCodeTable;
class BinaryEncoding;

/**
 * A null version of RFPortCode class.
 */
class NullRFPortCode : public RFPortCode {
public:
    static NullRFPortCode& instance();

private:
    NullRFPortCode();
    virtual ~NullRFPortCode();

    /// The only instance.
    static NullRFPortCode instance_;
    /// The parent socket code table.
    static SocketCodeTable socketCodes_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
