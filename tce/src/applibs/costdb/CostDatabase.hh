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
 * @file CostDatabase.hh
 *
 * Declaration of CostDatabase class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_DATABASE_HH
#define TTA_COST_DATABASE_HH


#include <vector>
#include <map>
#include <string>
#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wkeyword-macro")
#include <boost/regex.hpp>
POP_CLANG_DIAGS

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
    boost::smatch getValues(const std::string& text, const std::string& regex);

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
