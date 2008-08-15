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
 * @file CostDatabase.hh
 *
 * Declaration of CostDatabase class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_DATABASE_HH
#define TTA_COST_DATABASE_HH


#include <vector>
#include <map>
#include <string>
#include <boost/regex.hpp>

#include "CostDBTypes.hh"
#include "Exception.hh"

namespace HDB {
    class HDBManager;
}

class CostDBEntry;
class SearchStrategy;
class EntryKeyFieldProperty;
class EntryKeyProperty;
class CostDBEntryKey;
class CostDatabaseRegistry;


/**
 * Stores database entries and search strategy for making queries.
 */
class CostDatabase {
public:

    static CostDatabase& instance(const HDB::HDBManager& hdb)
        throw (Exception);
    virtual ~CostDatabase();

    void buildDefaultCostDatabase()
        throw (Exception);
    void buildRegisterFiles(const std::string& rfEstimatorPluginName)
        throw (Exception);
    void buildFunctionUnits(const std::string& fuEstimatorPluginName)
        throw (Exception);
    void buildBuses(const std::string& busEstimatorPluginName)
        throw (Exception);
    void buildSockets(const std::string& socketEstimatorPluginName)
        throw (Exception);
    bool isRegisterFilesBuilt();
    bool isFunctionUnitsBuilt();
    bool isBusesBuilt();
    bool isSocketsBuilt();

    void insertEntry(CostDBEntry* entry) throw (ObjectAlreadyExists);
    void setSearchStrategy(SearchStrategy* strategy);
    CostDBTypes::EntryTable search(
        const CostDBEntryKey& searchKey,
        const CostDBTypes::MatchTypeTable& match) const throw (KeyNotFound);
private:
    /// CostDatabase must be created with instance() method.
    CostDatabase(const HDB::HDBManager& hdb);

    /// Finds string matches using regular expressions.
    boost::smatch getValues(std::string text, std::string regex);

    /// Value type for map of search types.
    typedef std::pair<
        const EntryKeyProperty*, CostDBTypes::MatchTypeTable> MatchTypeMapType;
    /// Map of database entries according to entry type.
    typedef std::map<
        const EntryKeyProperty*, CostDBTypes::EntryTable> EntryMap;
    /// Search type for each entry type.
    typedef std::map<
        const EntryKeyProperty*,CostDBTypes::MatchTypeTable> MatchTypeMap;
    
    /// Search strategy used for queries.
    SearchStrategy* searchStrategy_;
    /// Database entries.
    EntryMap entries_;
    /// HDB used for creating cost database.
    const HDB::HDBManager& hdb_;
    /// Flag to note is register files built
    bool registerFilesBuilt_;
    /// Flag to note is function units built
    bool functionUnitsBuilt_;
    /// Flag to note is buses built
    bool busesBuilt_;
    /// Flag to note is sockets built
    bool socketsBuilt_;

    /// Unique instance of the class.
    static CostDatabase* instance_;
    /// Registry of CostDatabases.
    CostDatabaseRegistry* registry_;
    /// Copying not allowed.
    CostDatabase(const CostDatabase&);
    /// Assignment not allowed.
    CostDatabase& operator=(const CostDatabase&);
};

#endif
