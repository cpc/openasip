/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file HDBManager.hh
 *
 * Declaration of HDBManager class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#ifndef TTA_HDB_MANAGER_HH
#define TTA_HDB_MANAGER_HH

#include <string>
#include <list>
#include <set>
#include <vector>

#include "BlockImplementationFile.hh"
#include "Exception.hh"
#include "DataObject.hh"
#include "DBTypes.hh"
#include "HDBTypes.hh"
#include "CostEstimationData.hh"
#include "SQLiteConnection.hh"

class SQLite;
class RelationalDBConnection;

namespace TTAMachine {
class FunctionUnit;
class PipelineElement;
}

namespace HDB {

class FUEntry;
class RFEntry;
class FUArchitecture;
class RFArchitecture;
class FUImplementation;
class RFImplementation;
class CostFunctionPlugin;
class HDBRegistry;

/**
 * Class used to access hardware database (HDB). 
 *
 * Provides methods to create new HDB and to search, add, and remove
 * blocks from/to HDB. There can exist only one HDBManager instance
 * per database at the same time because HDBManager creates a
 * connection to the database in its constructor and does not close
 * the connection unless the HDBManager instance is deleted. And there
 * cannot be several connections to the same database at the same
 * time.
 */
class HDBManager {
public:
    friend class HDBManagerTest;

    typedef std::list<DataObject> DataObjectList;
    virtual ~HDBManager();
    static void createNew(const std::string& file)
        throw (UnreachableStream);

    std::string fileName() const;

    RowID addCostFunctionPlugin(const CostFunctionPlugin& plugin) const
        throw (Exception);
    virtual void removeCostFunctionPlugin(RowID pluginID) const;

    RowID addFUArchitecture(const FUArchitecture& architecture) const
        throw (InvalidData);
    bool canRemoveFUArchitecture(RowID archID) const;
    virtual void removeFUArchitecture(RowID archID) const
        throw (InvalidData);

    RowID addFUEntry() const;
    virtual void removeFUEntry(RowID id) const;

    RowID addFUImplementation(const FUEntry& entry) const
        throw (InvalidData);

    virtual void removeFUImplementation(RowID implementationID) const;

    void setArchitectureForFU(RowID fuID, RowID archID) const
        throw (InvalidData);
    void unsetArchitectureForFU(RowID fuID) const
        throw (InvalidData);

    RowID addRFArchitecture(const RFArchitecture& architecture) const;
    bool canRemoveRFArchitecture(RowID archID) const;
    virtual void removeRFArchitecture(RowID archID) const
        throw (InvalidData);

    RowID addRFEntry() const;
    virtual void removeRFEntry(RowID id) const;

    RowID addRFImplementation(
        const RFImplementation& implementation,
        RowID rfEntryID)
        throw (InvalidData);

    virtual void removeRFImplementation(RowID implID) const;

    void setArchitectureForRF(RowID rfID, RowID archID) const
        throw (InvalidData);

    void unsetArchitectureForRF(RowID rfID) const;

    void setCostFunctionPluginForFU(RowID fuID, RowID pluginID) const;
    void unsetCostFunctionPluginForFU(RowID fuID) const;
    void setCostFunctionPluginForRF(RowID rfID, RowID pluginID) const;
    void unsetCostFunctionPluginForRF(RowID rfID) const;

    std::set<RowID> fuEntryIDs() const;
    std::set<RowID> rfEntryIDs() const;
    std::set<RowID> busEntryIDs() const;
    std::set<RowID> socketEntryIDs() const;
    
    std::set<RowID> fuArchitectureIDs() const;
    std::set<RowID> fuArchitectureIDsByOperationSet(
        const std::set<std::string>& operationNames) const;
    std::set<RowID> rfArchitectureIDs() const;

    RowID fuEntryIDOfImplementation(RowID implID) const
        throw (KeyNotFound);

    RowID rfEntryIDOfImplementation(RowID implID) const
        throw (KeyNotFound);

    FUEntry* fuByEntryID(RowID id) const
        throw (KeyNotFound);

    RFEntry* rfByEntryID(RowID id) const
        throw (KeyNotFound);

    virtual FUArchitecture* fuArchitectureByID(RowID id) const
        throw (KeyNotFound);

    virtual RFArchitecture* rfArchitectureByID(RowID id) const
        throw (KeyNotFound);

    std::set<RowID> fuEntriesByArchitecture(
        const TTAMachine::FunctionUnit& fu) const;

    std::set<RowID> rfEntriesByArchitecture(
        int readPorts,
        int writePorts,
        int bidirPorts,
        int maxReads,
        int maxWrites,
        int latency,
        bool guardSupport,
        int guardLatency = 0,
        int width = 0,
        int size = 0) const;

    virtual DataObject costEstimationDataValue(
        const std::string& valueName,
        const std::string& pluginName) const
        throw (KeyNotFound);

    DataObject costEstimationDataValue(RowID entryId) const
        throw (KeyNotFound);

    RowID addFUCostEstimationData(
        RowID fuID,
        const std::string& valueName,
        const std::string& value,
        RowID pluginID) const;

    RowID addRFCostEstimationData(
        RowID rfID,
        const std::string& valueName,
        const std::string& value,
        RowID pluginID) const;
    

    DataObject fuCostEstimationData(
        const std::string& valueName,
        RowID implementationId,
        const std::string& pluginName) const
        throw (KeyNotFound);

    DataObject rfCostEstimationData(
        const std::string& valueName,
        RowID implementationId,
        const std::string& pluginName) const
        throw (KeyNotFound);

    RowID addBusEntry() const;
    virtual void removeBusEntry(RowID id) const;

    RowID addBusCostEstimationData(
        RowID busID,
        const std::string& valueName,
        const std::string& value,
        RowID pluginID) const;

    DataObject busCostEstimationData(
        const std::string& valueName,
        RowID busID,
        const std::string& pluginName) const
        throw (KeyNotFound);

    DataObjectList* busCostEstimationDataList(
        const std::string& valueName,
        RowID implementationId,
        const std::string& pluginName) const
        throw (KeyNotFound);

    RowID addSocketEntry() const;
    virtual void removeSocketEntry(RowID id) const;
    
    RowID addSocketCostEstimationData(
        RowID socketID,
        const std::string& valueName,
        const std::string& value,
        RowID pluginID) const;

    DataObject socketCostEstimationData(
        const std::string& valueName,
        RowID socketID,
        const std::string& pluginName) const
        throw (KeyNotFound);

    DataObjectList* socketCostEstimationDataList(
        const std::string& valueName,
        RowID implementationID,
        const std::string& pluginName) const
        throw (KeyNotFound);

    std::set<RowID> fuCostEstimationDataIDs(RowID fuImplID) const;

    std::set<RowID> rfCostEstimationDataIDs(RowID rfImplID) const;
    std::set<RowID> busCostEstimationDataIDs(RowID busID) const;
    std::set<RowID> socketCostEstimationDataIDs(RowID socketID) const;

    CostEstimationData costEstimationData(RowID id) const
        throw (KeyNotFound);

    std::set<RowID> costFunctionPluginIDs() const;
    std::set<RowID> costFunctionPluginDataIDs(RowID pluginID) const;
    CostFunctionPlugin* costFunctionPluginByID(RowID pluginID) const
        throw (Exception);

    virtual std::set<RowID> costEstimationDataIDs(
        const CostEstimationData& match,
        bool useCompiledQueries = false,
        RelationalDBQueryResult* compiledQuery = NULL) const;

    RowID addCostEstimationData(const CostEstimationData& data) const
        throw (InvalidData, KeyNotFound);

    virtual void removeCostEstimationData(RowID id) const;

    virtual void modifyCostEstimationData(RowID id, const CostEstimationData& data)
        throw (InvalidData, KeyNotFound);

    virtual void modifyCostFunctionPlugin(RowID id, 
        const CostFunctionPlugin& plugin)
        throw (InvalidData, KeyNotFound);

    virtual void deleteCostEstimationDataIDsQueries() const = 0;

    std::list<std::string> blockSourceFile();

protected:
    virtual RFImplementation* createImplementationOfRF(RowID id) const;
    virtual FUImplementation* createImplementationOfFU(
        FUArchitecture& architecture,
        RowID id) const;
    void createCostEstimatioDataIdsQuery(
        const CostEstimationData& match, 
        std::string* query,
        RelationalDBQueryResult* compiledQuery = NULL,
        short int* queryHash = NULL,
        bool createBindableQuery = false) const;
    RelationalDBConnection* getDBConnection() const; 

    HDBManager(const std::string& hdbFile) 
        throw (IOException);
    

private:
    
    // Struct PipelineElementUsage
    struct PipelineElementUsage {
        std::set<const TTAMachine::PipelineElement*> usage1;
        std::set<const TTAMachine::PipelineElement*> usage2;
    };

    // typedef for PipelineElemtnUsageTable
    typedef std::vector<PipelineElementUsage> PipelineElementUsageTable;

    bool hasFUEntry(RowID id) const;
    bool hasRFEntry(RowID id) const;
    bool hasBusEntry(RowID id) const;
    bool hasSocketEntry(RowID id) const;
    bool hasCostEstimationDataByID(RowID id) const;
    bool hasCostFunctionPluginByID(RowID id) const;
    bool fuEntryHasArchitecture(RowID id) const
        throw (KeyNotFound);
    bool rfEntryHasArchitecture(RowID id) const
        throw (KeyNotFound);
    bool containsOperation(const std::string& opName) const;
    bool containsImplementationFile(const std::string& pathToFile) const;
    bool containsFUArchitecture(RowID id) const;
    bool containsRFArchitecture(RowID id) const;
    RowID fuArchitectureID(RowID fuEntryID) const
        throw (NotAvailable);
    RowID rfArchitectureID(RowID rfEntryID) const
        throw (NotAvailable);
    bool hasColumn(
        const std::string& table, const std::string& columnName) const;
    int addBooleanColumn(
        const std::string& table, const std::string& newcolumn);

    CostFunctionPlugin* createCostFunctionOfFU(RowID id) const;
    CostFunctionPlugin* createCostFunctionOfRF(RowID id) const;

    void addPortsAndBindingsToFUArchitecture(
        FUArchitecture& architecture,
        RowID id) const;
    void addOperationPipelinesToFUArchitecture(
        FUArchitecture& architecture,
        RowID id) const;
    std::string resolveArchitecturePort(
        const FUArchitecture& architecture,
        RowID entryID,
        const std::string& implementedPort) const;
    void addOpcodesToImplementation(
        FUImplementation& implementation,
        RowID entryID) const;
    void addDataPortsToImplementation(
        FUImplementation& implementation,
        FUArchitecture& architecture,
        RowID entryID) const;
    void addFUExternalPortsToImplementation(
        FUImplementation& implementation,
        RowID entryID) const;
    void addRFExternalPortsToImplementation(
        RFImplementation& implementation,
        RowID entryID) const;
    void addFUParametersToImplementation(
        FUImplementation& implementation,
        RowID entryID) const;
    void addRFParametersToImplementation(
        RFImplementation& implementation,
        RowID entryID) const;
    void addBlockImplementationFiles(
        FUImplementation& implementation,
        RowID entryID) const;
    void addDataPortsToImplementation(
        RFImplementation& implementation,
        RowID entryID) const;
    void addBlockImplementationFiles(
        RFImplementation& implementation,
        RowID entryID) const;
    void addBlockImplementationFileToHDB(
        const BlockImplementationFile& file) const
        throw (RelationalDBException);
    void createCostEstimatioDataIdsQuery(
        const CostEstimationData& match, 
        std::string& query) const;

    static bool isMatchingArchitecture(
        const TTAMachine::FunctionUnit& fu, const FUArchitecture& arch);
    static bool areCompatiblePipelines(
        const PipelineElementUsageTable& table);
    static void insertFileFormats(RelationalDBConnection& connection);
    static BlockImplementationFile::Format fileFormat(
        const std::string& formatString);
    static std::string formatString(BlockImplementationFile::Format format);
    static std::string directionString(HDB::Direction direction);
    static std::string fuEntryByIDQuery(RowID id);
    static std::string rfEntryByIDQuery(RowID id);
    static std::string busEntryByIDQuery(RowID id);
    static std::string socketEntryByIDQuery(RowID id);
    static std::string fuArchitectureByIDQuery(RowID id);
    static std::string fuPortsAndBindingsByIDQuery(RowID id);
    static std::string ioUsageDataByIDQuery(RowID id);
    static std::string resourceUsageDataByIDQuery(RowID id);
    static std::string fuImplementationByIDQuery(RowID id);
    static std::string opcodesByIDQuery(RowID id);
    static std::string fuImplementationDataPortsByIDQuery(RowID id);
    static std::string fuExternalPortsByIDQuery(RowID id);
    static std::string rfExternalPortsByIDQuery(RowID id);
    static std::string fuPortBindingByNameQuery(
        RowID fuID,
        const std::string& portName);
    static std::string fuImplementationParametersByIDQuery(RowID id);
    static std::string rfImplementationParametersByIDQuery(RowID id);
    static std::string fuSourceFilesByIDQuery(RowID id);
    static std::string rfArchitectureByIDQuery(RowID id);
    static std::string rfImplementationByIDQuery(RowID id);
    static std::string rfImplementationByIDQuery2(RowID id);
    static std::string rfImplementationDataPortsByIDQuery(RowID id);
    static std::string rfSourceFilesByIDQuery(RowID id);
    
    /// Handle to the database.
    SQLite* db_;
    /// Handle to the database connection.
    RelationalDBConnection* dbConnection_;
    /// The HDB file to manager.
    std::string hdbFile_;
    /// Unique instance of the HDBManager.
    static HDBManager* instance_;
};

}

#endif
