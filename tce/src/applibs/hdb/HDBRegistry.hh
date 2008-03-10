/**
 * @file HDBRegistry.hh
 *
 * Declaration of HDBRegistry class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
