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
