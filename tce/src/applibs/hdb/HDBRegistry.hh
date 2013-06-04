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
 * @file HDBRegistry.hh
 *
 * Declaration of HDBRegistry class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
#ifndef TTA_HDB_REGISTRY_HH
#define TTA_HDB_REGISTRY_HH

#include <string>
#include <map>
#include <vector>
#include "Exception.hh"
#include "CachedHDBManager.hh"

namespace HDB {

/**
 * Registry for multiple HDBs.
 */
class HDBRegistry {
public:
    static HDBRegistry& instance();
    virtual ~HDBRegistry();

    CachedHDBManager& hdb(const std::string fileName)
        throw (Exception);
    void loadFromSearchPaths();
    void addHDB(CachedHDBManager* hdbManager);
    bool hasHDB(const std::string& hdbFile);
    int hdbCount();
    CachedHDBManager& hdb(unsigned int index) throw (OutOfRange);
    std::string hdbPath(unsigned int index) throw (OutOfRange);
    int hdbErrorCount();
    std::string hdbErrorMessage(unsigned int index) throw (OutOfRange);
    void removeDeadHDBPaths();

private:
    /// HDB registry must be created with instance() method.
    HDBRegistry();
    /// all opened HDBs are stored in this map
    std::map<const std::string, CachedHDBManager*> registry_;
    /// errors found during loading HDBs are strored in this vector
    std::vector<std::string> errorMessages_;
    /// Unique instance of the class.
    static HDBRegistry* instance_;
};

}

#endif
