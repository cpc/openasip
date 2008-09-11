/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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

    CostDatabase& costDatabase(const HDB::HDBManager& hdb)
        throw (Exception);
    void addCostDatabase(
        CostDatabase* costDatabase, const HDB::HDBManager& hdb);
    bool hasCostDatabase(const HDB::HDBManager& hdb);
    int costDatabaseCount();
    CostDatabase& costDatabase(unsigned int index)
        throw (OutOfRange);
    std::string hdbPath(unsigned int index)
        throw (OutOfRange);

private:
    /// CostDatabase registry must be created with instance() method.
    CostDatabaseRegistry();
    /// All created CostDatabasess are stored in this map.
    std::map<const HDB::HDBManager*, CostDatabase*> registry_;
    /// Unique instance of the class.
    static CostDatabaseRegistry* instance_;
    
};

#endif
