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
 * @file InterPassData.hh
 *
 * Declaration of InterPassData.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTER_PASS_DATA_HH
#define TTA_INTER_PASS_DATA_HH

#include <map>
#include <string>
#include "Exception.hh"

class InterPassDatum;

/** 
 * Class for storing inter-pass data of any type.
 *
 * Provides a generic "key-value" interface and shortcut methods for
 * accessing the most frequently used data.
 */
class InterPassData {
public:
    InterPassData();
    virtual ~InterPassData();

    InterPassDatum& datum(const std::string& key)
        throw (KeyNotFound);
    bool hasDatum(const std::string& key) const;
    void setDatum(const std::string& key, InterPassDatum* datum);
    void removeDatum(const std::string& key);
private:
    /// container for the data
    std::map<std::string, InterPassDatum*> data_;
};

#endif

