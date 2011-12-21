/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file CIStringSet.hh
 *
 * Declaration of CIStringSet type.
 *
 * @author Pekka Jääskeläinen 2010
 
 */

#ifndef TTA_CI_STRING_SET_HH
#define TTA_CI_STRING_SET_HH

#include <cstring>
#include "TCEString.hh"

namespace TCETools {

    struct CaseInsensitiveCmp {
        bool operator()(const TCEString& s1, const TCEString& s2) const {
            return strcmp(s1.lower().c_str(), s2.lower().c_str()) < 0;
        }
    };

    // set for storing case insensitive strings (such as OSAL operation names)
    typedef std::set<TCEString, CaseInsensitiveCmp> CIStringSet;
}

#endif
