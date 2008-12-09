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
 * @file NullRFPortCode.cc
 *
 * Implementation of NullRFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullRFPortCode.hh"
#include "BinaryEncoding.hh"
#include "SocketCodeTable.hh"

BinaryEncoding NullRFPortCode::bem_;
SocketCodeTable NullRFPortCode::socketCodes_("NULL", bem_);
NullRFPortCode NullRFPortCode::instance_;

/**
 * The constructor.
 */
NullRFPortCode::NullRFPortCode() :
    RFPortCode("NULL", 0, 0, 0, socketCodes_) {
}


/**
 * The destructor.
 */
NullRFPortCode::~NullRFPortCode() {
}


/**
 * Returns the only instance of NullRFPortCode.
 *
 * @return The only instance.
 */
NullRFPortCode&
NullRFPortCode::instance() {
    return instance_;
}
