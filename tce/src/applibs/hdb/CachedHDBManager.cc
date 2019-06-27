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
 * @file CachedHDBManager.cc
 *
 * Implementation of CachedHDBManager class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "CachedHDBManager.hh"
#include "SQLiteConnection.hh"
#include "MapTools.hh"
#include "FUArchitecture.hh"
#include "RFArchitecture.hh"
#include "FUImplementation.hh"
#include "RFImplementation.hh"
#include "CostFunctionPlugin.hh"
#include "DataObject.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "HDBTypes.hh"
#include "HDBRegistry.hh"

using namespace HDB;

/**
 * The Constructor.
 *
 * @param hdbFile HDBFile to open.
 * @throw IOException if an error occured opening the HDB file.
 */
CachedHDBManager::CachedHDBManager(const std::string& hdbFile)
    : HDBManager(hdbFile) {
    lastModificationTime_ = FileSystem::lastModificationTime(hdbFile);
    lastSizeInBytes_ = FileSystem::sizeInBytes(hdbFile);
}

/**
 * The Destructor.
 *
 * Deletes all cached items.
 */
CachedHDBManager::~CachedHDBManager() {
    MapTools::deleteAllValues(fuArchCache_);
    MapTools::deleteAllValues(rfArchCache_);
    MapTools::deleteAllValues(fuImplCache_);
    MapTools::deleteAllValues(rfImplCache_);

    costEstimationPluginValueCache_.clear();     
}


/**
 * Returns a CachedHDBManager instance for the given file.
 *
 * Only one instance is created per file.
 *
 * @param hdbFile Name of the HDB file to open.
 * @return An instance of HDBManager.
 * @exception IOException If connection to the DB cannot be established.
 */
CachedHDBManager&
CachedHDBManager::instance(const std::string& hdbFile) {
    HDBRegistry* registry = &HDBRegistry::instance();
    if (!registry->hasHDB(hdbFile)) {
        registry->addHDB(new CachedHDBManager(hdbFile));
    }
    return registry->hdb(hdbFile);
}

/**
 * Creates a new HDB and returns CachedHDBManager instance for accessing it.
 *
 * @param file Full path of the new HDB file.
 * @return CachedHDBManager instance accessing the newly created HDB.
 * @throw UnreachableStream If the HDB creation was not succesful.
 */
CachedHDBManager&
CachedHDBManager::createNew(const std::string& file) {
    HDBManager::createNew(file);
    return instance(file);
}

/**
 * Removes FU architecture from the HDB.
 *
 * Removes architecture from the cache and calls base class function to
 * actually remove the FU architecture.
 *
 * @param archID ID of the FU architecture to remove.
 */
void
CachedHDBManager::removeFUArchitecture(RowID archID) const {
    // Remove architecture from the cache.
    std::map<RowID, FUArchitecture*>::iterator iter =
        fuArchCache_.find(archID);

    if (iter != fuArchCache_.end()) {
        delete (*iter).second;
        fuArchCache_.erase(iter);
    }

    HDBManager::removeFUArchitecture(archID);
}

/**
 * Removes FU implementation from the HDB.
 *
 * Removes implementation from the cache and calls base class function to
 * actually remove the FU implementation.
 *
 * @param id ID of the FU implementation to remove.
 */
void
CachedHDBManager::removeFUImplementation(RowID id) const {

    RowID entryID = fuEntryIDOfImplementation(id);

    // Remove implementation from the cache.
    std::map<RowID, FUImplementation*>::iterator iter =
        fuImplCache_.find(entryID);

    if (iter != fuImplCache_.end()) {
        delete (*iter).second;
        fuImplCache_.erase(iter);
    }

    HDBManager::removeFUImplementation(id);
}


/**
 * Removes RF architecture from the HDB.
 *
 * Removes architecture from the cache and calls base class function to
 * actually remove the RF architecture.
 *
 * @param archID ID of the RF architecture to remove.
 */
void
CachedHDBManager::removeRFArchitecture(RowID archID) const {
    // Remove architecture from the cache.
    std::map<RowID, RFArchitecture*>::iterator iter =
        rfArchCache_.find(archID);

    if (iter != rfArchCache_.end()) {
        delete (*iter).second;
        rfArchCache_.erase(iter);
    }

    HDBManager::removeRFArchitecture(archID);
}

/**
 * Removes RF implementation from the HDB.
 *
 * Removes implementation from the cache and calls base class function to
 * actually remove the RF implementation.
 *
 * @param id ID of the RF implementation to remove.
 */
void
CachedHDBManager::removeRFImplementation(RowID id) const {

    RowID entryID = rfEntryIDOfImplementation(id);

    // Remove implementation from the cache.
    std::map<RowID, RFImplementation*>::iterator iter =
        rfImplCache_.find(entryID);

    if (iter != rfImplCache_.end()) {
        delete (*iter).second;
        rfImplCache_.erase(iter);
    }

    HDBManager::removeRFImplementation(id);
}

/**
 * Returns FU architecture with the given ID.
 *
 * First the fu architecture cache is searched for the ID, if the architecture
 * is found in the cache, a copy of the cached item is returned. If the
 * architecture was not found in the cache, a DB query is made using the
 * base class implementation and the item is then added to the cache.
 *
 * @param id ID of the fu architecture.
 * @return FUArchitecture object with the give ID.
 * @throw KeyNotFound if an architecture with the given id was not found.
 */
FUArchitecture*
CachedHDBManager::fuArchitectureByID(RowID id) const {
    validateCache();

    std::map<RowID, FUArchitecture*>::iterator iter = fuArchCache_.find(id);
    if (iter != fuArchCache_.end()) {
        return new FUArchitecture(*(*iter).second);
    }

    FUArchitecture* arch = HDBManager::fuArchitectureByID(id);
    fuArchCache_[id] = arch;
    return new FUArchitecture(*arch);
}

/**
 * Returns RF architecture with the given ID.
 *
 * First the RF architecture cache is searched for the ID, if the architecture
 * is found in the cache, a copy of the cached item is returned. If the
 * architecture was not found in the cache, a DB query is made using the
 * base class implementation and the item is then added to the cache.
 *
 * @param id ID of the fu architecture.
 * @return RFArchitecture object with the give ID.
 * @throw KeyNotFound if an architecture with the given id was not found.
 */
RFArchitecture*
CachedHDBManager::rfArchitectureByID(RowID id) const {
    validateCache();

    std::map<RowID, RFArchitecture*>::iterator iter = rfArchCache_.find(id);
    if (iter != rfArchCache_.end()) {
        return new RFArchitecture(*(*iter).second);
    }

    RFArchitecture* arch = HDBManager::rfArchitectureByID(id);
    rfArchCache_[id] = arch;
    return new RFArchitecture(*arch);
}

/**
 * Returns cost estimation data which is not connected to any machine 
 * implementation id.
 *
 * This version assumes that there's only one entry with given parameters.
 * Caches values by plugin name.
 * 
 * @param valueName Name of the value to fetch.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain cost estimation 
 *                        data with the given arguments.
 */
DataObject
CachedHDBManager::costEstimationDataValue(
    const std::string& valueName, const std::string& pluginName) const {
    validateCache();

    std::map<std::string, std::map<std::string, DataObject> >::iterator iter =
        costEstimationPluginValueCache_.find(pluginName);

    if (iter != costEstimationPluginValueCache_.end()) {
        std::map<std::string, DataObject>::iterator it = 
            (*iter).second.find(valueName);
        if (it != (*iter).second.end()) {
            return (*it).second;
        } else {
            DataObject dataObject = HDBManager::costEstimationDataValue(valueName, pluginName);
            (*iter).second[valueName] = dataObject;
            return dataObject;
        }
    } else {
        std::map<std::string, DataObject> temp;
        DataObject dataObject = HDBManager::costEstimationDataValue(valueName, pluginName);
        temp[valueName] = dataObject;
        costEstimationPluginValueCache_[pluginName] = temp;
        return dataObject;
    }
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::modifyCostFunctionPlugin.
 * 
 * @param id 
 * @param plugin 
 * @exception InvalidData
 * @exception KeyNotFound
 */
void
CachedHDBManager::modifyCostFunctionPlugin(
    RowID id, const CostFunctionPlugin& plugin) {
    // can't say by rowid what plugin is modified
    costEstimationPluginValueCache_.clear();     

    HDBManager::modifyCostFunctionPlugin(id, plugin);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeCostFunctionPlugin.
 * 
 * @param pluginID 
 */
void
CachedHDBManager::removeCostFunctionPlugin(RowID pluginID) const {

    // can't say by rowid what plugin is removed
    costEstimationPluginValueCache_.clear();     

    HDBManager::removeCostFunctionPlugin(pluginID);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeFUEntry.
 * 
 * @param id
 */
void
CachedHDBManager::removeFUEntry(RowID id) const {

    costEstimationPluginValueCache_.clear();     
    HDBManager::removeFUEntry(id);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeRFEntry.
 * 
 * @param id
 */
void
CachedHDBManager::removeRFEntry(RowID id) const {

    costEstimationPluginValueCache_.clear();     
    HDBManager::removeRFEntry(id);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeBusEntry.
 * 
 * @param id
 */
void
CachedHDBManager::removeBusEntry(RowID id) const {

    costEstimationPluginValueCache_.clear();     
    HDBManager::removeBusEntry(id);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeSocketEntry.
 * 
 * @param id
 */
void
CachedHDBManager::removeSocketEntry(RowID id) const {

    costEstimationPluginValueCache_.clear();     
    HDBManager::removeSocketEntry(id);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::removeCostEstimationData.
 * 
 * @param id
 */
void
CachedHDBManager::removeCostEstimationData(RowID id) const {   

    costEstimationPluginValueCache_.clear();     
    HDBManager::removeCostEstimationData(id);
}

/** 
 * Cached (invalidates cache) function of
 * HDBManager::modifyCostEstimationData.
 * 
 * @param id 
 * @param data
 * @exception InvalidData
 * @exception KeyNotFound
 */
void
CachedHDBManager::modifyCostEstimationData(
    RowID id, const CostEstimationData& data) {
    costEstimationPluginValueCache_.clear();     
    HDBManager::modifyCostEstimationData(id, data);
}

/**
 * Returns implementation of a RF with the given index.
 *
 * Returns NULL if the RF doesn't have implementation.
 *
 * First the RF implementation cache is searched for the ID,
 * if the implementation is found in the cache, a copy of the cached item is
 * returned. If the implementation was not found in the cache,
 * a DB query is made using the base class implementation and the item
 * is then added to the cache.
 *
 *
 * @param id ID of the RF implementation.
 * @return RF implementation.
 */
RFImplementation*
CachedHDBManager::createImplementationOfRF(RowID id) const {

    validateCache();

    std::map<RowID, RFImplementation*>::iterator iter = rfImplCache_.find(id);
    if (iter != rfImplCache_.end()) {
        return new RFImplementation(*(*iter).second);
    }

    RFImplementation* impl = HDBManager::createImplementationOfRF(id);

    if (impl != NULL) {
        rfImplCache_[id] = impl;
        return new RFImplementation(*impl);
    } else {
        return NULL;
    }
}

/**
 * Returns implementation of a FU with the given index.
 *
 * Returns NULL if the FU doesn't have implementation.
 *
 * First the FU implementation cache is searched for the ID,
 * if the implementation is found in the cache, a copy of the cached item is
 * returned. If the implementation was not found in the cache,
 * a DB query is made using the base class implementation and the item
 * is then added to the cache.
 *
 * @param id ID of the FU implementation.
 * @return FU implementation.
 */
FUImplementation*
CachedHDBManager::createImplementationOfFU(
    FUArchitecture& architecture, RowID id) const {

    validateCache();

    std::map<RowID, FUImplementation*>::iterator iter = fuImplCache_.find(id);
    if (iter != fuImplCache_.end()) {
        return new FUImplementation(*(*iter).second);
    }

    FUImplementation* impl =
        HDBManager::createImplementationOfFU(architecture, id);

    if (impl != NULL) {
        fuImplCache_[id] = impl;
        return new FUImplementation(*impl);
    } else {
        return NULL;
    }
}


const FUArchitecture&
CachedHDBManager::fuArchitectureByIDConst(RowID id) const {

    validateCache();

    std::map<RowID, FUArchitecture*>::iterator iter = fuArchCache_.find(id);
    if (iter == fuArchCache_.end()) {
        FUArchitecture* arch = HDBManager::fuArchitectureByID(id);
        fuArchCache_[id] = arch;
    }
    return *fuArchCache_[id];
}


const RFArchitecture&
CachedHDBManager::rfArchitectureByIDConst(RowID id) const {

    validateCache();

    std::map<RowID, RFArchitecture*>::iterator iter = rfArchCache_.find(id);
    if (iter == rfArchCache_.end()) {
        RFArchitecture* arch = HDBManager::rfArchitectureByID(id);
        rfArchCache_[id] = arch;
    }
    return *rfArchCache_[id];
}


/**
 * Wrapper for HDBManager costEstimationDataIDs.
 *
 * Compiles and caches queries for HDBManager costEstimationDataIDs function.
 */
std::set<RowID>
CachedHDBManager::costEstimationDataIDs(
    const CostEstimationData& match, 
    bool useCompiledQueries,
    RelationalDBQueryResult* compiledQuery) const {

    if (!useCompiledQueries) {
        return HDBManager::costEstimationDataIDs(match);
    }
    
    // query hash (bit vector might be better suited for this)
    short int queryHash = 0;
    // calculate query hash only to identify the query
    HDBManager::createCostEstimatioDataIdsQuery(match, NULL, NULL, &queryHash);

    // check if query already exists
    std::map<short int,RelationalDBQueryResult*>::iterator it = 
        costEstimationDataIDsQueries_.find(queryHash);

    if (costEstimationDataIDsQueries_.end() != it) {
        // if query already existed use it
        return HDBManager::costEstimationDataIDs(match, true, it->second); 
    } else {
        // else create a new compiledQuery and use it
        assert(queryHash != 0);
        std::string query = "";
        // create bindable query
        HDBManager::createCostEstimatioDataIdsQuery(match, &query, NULL, NULL, true);
        compiledQuery = HDBManager::getDBConnection()->query(query, false);
        costEstimationDataIDsQueries_[queryHash] = compiledQuery;
        return HDBManager::costEstimationDataIDs(match, true, compiledQuery); 
    }
}


/**
 * Deletes cached costEstimationDataIDs queries.
 */
void 
CachedHDBManager::deleteCostEstimationDataIDsQueries() const {
    std::map<short int,RelationalDBQueryResult*>::iterator it = 
        costEstimationDataIDsQueries_.begin();

    while (it != costEstimationDataIDsQueries_.end()) {
        delete it->second;
        it->second = NULL;
        ++it;
    }
    costEstimationDataIDsQueries_.clear();
}


/**
 * Checks if HDB file is modified. In case it is, invalid cache is cleared.
 */
void
CachedHDBManager::validateCache() const {
    std::string hdbFile = HDBManager::fileName();
    std::time_t modTime = FileSystem::lastModificationTime(hdbFile);
    uintmax_t byteSize = FileSystem::sizeInBytes(hdbFile);
    
    if (modTime == std::time_t(-1) || 
        byteSize == static_cast<uintmax_t>(-1)) {
        // problems accessing the file
        return;
    } else if (lastModificationTime_ == modTime && 
               lastSizeInBytes_ == byteSize) {
        // no changes
        return;
    }

    // delete all cached items
    MapTools::deleteAllValues(fuArchCache_);
    MapTools::deleteAllValues(rfArchCache_);
    MapTools::deleteAllValues(fuImplCache_);
    MapTools::deleteAllValues(rfImplCache_);
    costEstimationPluginValueCache_.clear();

    // set current size and modification time
    lastModificationTime_ = modTime;
    lastSizeInBytes_ = byteSize;
}
