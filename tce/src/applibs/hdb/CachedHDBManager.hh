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
 * @file CachedHDBManager.hh
 *
 * Declaration of CachedHDBManager class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maattae-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CACHED_HDB_MANAGER_HH
#define TTA_CACHED_HDB_MANAGER_HH

#include <map>
#include "HDBManager.hh"
#include "RelationalDBQueryResult.hh"
#include "Exception.hh"
#include "CostEstimationData.hh"

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
    static CachedHDBManager& instance(const std::string& hdbFile);

    virtual ~CachedHDBManager();

    static CachedHDBManager& createNew(const std::string& fileName);

    // Functions invalidating cached objects.
    virtual void removeFUArchitecture(RowID archID) const;

    virtual void removeFUImplementation(RowID id) const;

    virtual void removeRFArchitecture(RowID archID) const;

    virtual void removeRFImplementation(RowID archID) const;

    // Functions below may invalidate cost estimation values
    virtual void modifyCostFunctionPlugin(
        RowID id, const CostFunctionPlugin& plugin);

    virtual void removeCostFunctionPlugin(RowID pluginID) const;

    virtual void removeFUEntry(RowID id) const;

    virtual void removeRFEntry(RowID id) const;

    virtual void removeBusEntry(RowID id) const;

    virtual void removeSocketEntry(RowID id) const;

    virtual void removeCostEstimationData(RowID id) const;

    virtual void modifyCostEstimationData(
        RowID id, const CostEstimationData& data);

    // Queries using cache.
    virtual FUArchitecture* fuArchitectureByID(RowID id) const;

    virtual RFArchitecture* rfArchitectureByID(RowID id) const;

    virtual DataObject costEstimationDataValue(
        const std::string& valueName, const std::string& pluginName) const;

    const FUArchitecture& fuArchitectureByIDConst(RowID id) const;
    const RFArchitecture& rfArchitectureByIDConst(RowID id) const;

    // Functions to manually delete stored queries
    virtual void deleteCostEstimationDataIDsQueries() const;

    // Queries using query cache
    virtual std::set<RowID> costEstimationDataIDs(
        const CostEstimationData& match, bool useCompiledQueries = false,
        RelationalDBQueryResult* compiledQuery = NULL) const;

private:
    CachedHDBManager(const std::string& hdbFile);

    // Private queries using cache.
    virtual RFImplementation* createImplementationOfRF(RowID id) const;
    virtual FUImplementation* createImplementationOfFU(
        FUArchitecture& architecture, RowID id) const;

    // Checks if cache is invalid.
    void validateCache() const;

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

    /// map of cached (compiled) queries for costEstimatioDataIDs function
    mutable std::map<short int, RelationalDBQueryResult*>
        costEstimationDataIDsQueries_;

    /// used to detect modifications to the HDB file (which invalidates cache)
    mutable std::time_t lastModificationTime_;
    /// used to detect modifications to the HDB file (which invalidates cache)
    mutable uintmax_t lastSizeInBytes_;
};

} // End namespace HDB.
#endif
