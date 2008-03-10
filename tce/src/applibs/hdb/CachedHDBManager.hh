/**
 * @file CachedHDBManager.hh
 *
 * Declaration of CachedHDBManager class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maattae@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CACHED_HDB_MANAGER_HH
#define TTA_CACHED_HDB_MANAGER_HH

#include <map>
#include "HDBManager.hh"
#include "Exception.hh"

namespace HDB {

/**
 * Cached interface to HDBs.
 *
 * HDBManager class queries HDB file every time the manager is queried
 * for an object in the database. Queries to the database are time consuming,
 * especially when constructing FU and RF implementation objects based on
 * data scattered across multiple HDB tables.
 * CachedHDBManager caches the queried objects in memory, resulting in
 * significant speedups if the same objects are queried repeatedly without
 * any modifications to the related data in the HDB.
 *
 * IMPORTANT NOTE:
 *
 * If a new function which modifies or removes cached objects is added
 * to the HDBManager, it must be derived here to remove corresponding objects
 * from the cache! Please check that all functions you are using that
 * modify cached data are reimplemented to remove cached objects.
 */
class CachedHDBManager : public HDBManager {
public:
    static CachedHDBManager& instance(const std::string& hdbFile) 
        throw (IOException);

    virtual ~CachedHDBManager();

    static CachedHDBManager& createNew(const std::string& fileName)
        throw (UnreachableStream);

    // Functions invalidating cached objects.
    virtual void removeFUArchitecture(RowID archID) const
        throw (InvalidData);

    virtual void removeFUImplementation(RowID id) const;

    virtual void removeRFArchitecture(RowID archID) const
        throw (InvalidData);

    virtual void removeRFImplementation(RowID archID) const;
    
    // Functions below may invalidate cost estimation values
    virtual void modifyCostFunctionPlugin(
        RowID id, const CostFunctionPlugin& plugin) 
        throw (InvalidData, KeyNotFound);

    virtual void removeCostFunctionPlugin(RowID pluginID) const;

    virtual void removeFUEntry(RowID id) const;

    virtual void removeRFEntry(RowID id) const;

    virtual void removeBusEntry(RowID id) const;

    virtual void removeSocketEntry(RowID id) const;

    virtual void removeCostEstimationData(RowID id) const;

    virtual void modifyCostEstimationData(RowID id, const CostEstimationData& data)
        throw (InvalidData, KeyNotFound);

    // Queries using cache.
    virtual FUArchitecture* fuArchitectureByID(RowID id) const
        throw (KeyNotFound);

    virtual RFArchitecture* rfArchitectureByID(RowID id) const
        throw (KeyNotFound);

    virtual DataObject costEstimationDataValue(
        const std::string& valueName,
        const std::string& pluginName) const
        throw (KeyNotFound);
    
    const FUArchitecture& fuArchitectureByIDConst(RowID id) const;
    const RFArchitecture& rfArchitectureByIDConst(RowID id) const;

private:

    CachedHDBManager(const std::string& hdbFile)
        throw (IOException);

    // Private queries using cache.
    virtual RFImplementation* createImplementationOfRF(RowID id) const;
    virtual FUImplementation* createImplementationOfFU(
        FUArchitecture& architecture, RowID id) const;

    /// FU Architecture cache.
    mutable std::map<RowID, FUArchitecture*> fuArchCache_;
    /// RF Architecture cache.
    mutable std::map<RowID, RFArchitecture*> rfArchCache_;
    /// FU Implementation cache.
    mutable std::map<RowID, FUImplementation*> fuImplCache_;
    /// RF Implementation cache.
    mutable std::map<RowID, RFImplementation*> rfImplCache_;
    /// Cost estimation plugin value cache (pluginName/valueName)
    mutable std::map<std::string, std::map<std::string, DataObject> > 
        costEstimationPluginValueCache_;
};

} // End namespace HDB.
#endif
