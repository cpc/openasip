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
 * @file CostDatabaseRegistry.hh
 *
 * Declaration of CostDatabaseRegistry class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_COST_DATABASE_REGISTRY_HH
#define TTA_COST_DATABASE_REGISTRY_HH

#include <string>
#include <map>
#include "Exception.hh"
#include "CostDBTypes.hh"
#include "HDBManager.hh"

class CostDatabase;

/**
 * Registry for multiple CostDatabases.
 */
class CostDatabaseRegistry {
public:
    static CostDatabaseRegistry& instance();
    virtual ~CostDatabaseRegistry();

    CostDatabase& costDatabase(const HDB::HDBManager& hdb);
    void addCostDatabase(
        CostDatabase* costDatabase, const HDB::HDBManager& hdb);
    bool hasCostDatabase(const HDB::HDBManager& hdb);
    int costDatabaseCount();
    CostDatabase& costDatabase(unsigned int index);
    std::string hdbPath(unsigned int index);

private:
    /// CostDatabase registry must be created with instance() method.
    CostDatabaseRegistry();
    /// All created CostDatabasess are stored in this map.
    std::map<const HDB::HDBManager*, CostDatabase*> registry_;
    /// Unique instance of the class.
    static CostDatabaseRegistry* instance_;
    
};

#endif
