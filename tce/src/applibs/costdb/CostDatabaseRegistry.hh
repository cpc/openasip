/**
 * @file CostDatabaseRegistry.hh
 *
 * Declaration of CostDatabaseRegistry class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
