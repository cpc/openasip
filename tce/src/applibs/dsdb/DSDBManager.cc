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
 * @file DSDBManager.cc
 *
 * Implementation of DSDBManager class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <utility>
#include "DSDBManager.hh"
#include "SQLite.hh"
#include "RelationalDBConnection.hh"
#include "RelationalDBQueryResult.hh"
#include "ADFSerializer.hh"
#include "IDFSerializer.hh"
#include "Conversion.hh"
#include "Machine.hh"
#include "MachineImplementation.hh"
#include "DataObject.hh"

using std::pair;
using std::map;
using std::set;
using std::vector;
using std::make_pair;
using std::string;
using namespace CostEstimator;

const string CREATE_ARCH_TABLE =
    "CREATE TABLE architecture ("
    "       id INTEGER PRIMARY KEY,"
    "       adf_xml VARCHAR)";

const string CREATE_IMPL_TABLE =
    "CREATE TABLE implementation ("
    "       id INTEGER PRIMARY KEY,"
    "       idf_xml VARCHAR,"
    "       lpd DOUBLE,"
    "       area INTEGER)";

const string CREATE_MACHINE_CFG_TABLE =
    "CREATE TABLE machine_configuration ("
    "       id INTEGER PRIMARY KEY,"
    "       architecture REFERENCES architecture(id) NOT NULL,"
    "       implementation REFERENCES implementation(id))";

const string CREATE_APPLICATION_TABLE =
    "CREATE TABLE application ("
    "       id INTEGER PRIMARY KEY,"
    "       path varchar)";

const string CREATE_CYCLE_COUNT_TABLE =
    "CREATE TABLE cycle_count ("
    "       architecture REFERENCES architecture(id) NOT NULL,"
    "       application REFERENCES application(id) NOT NULL,"
    "       cycles BIGINT NOT NULL)";

const string CREATE_ENERGY_ESTIMATE_TABLE =
    "CREATE TABLE energy_estimate ("
    "       implementation REFERENCES implementation(id) NOT NULL,"
    "       application REFERENCES application(id) NOT NULL,"
    "       energy_estimate DOUBLE NOT NULL)";


/**
 * The Constructor.
 *
 * Loads a DSDB from the given file.
 *
 * @param file DSDB file to load.
 * @throw IOException if the DSDB file couldn't be succesfully loaded.
 */
DSDBManager::DSDBManager(const std::string& file)
    throw (IOException):
    db_(new SQLite()), dbConnection_(NULL), file_(file) {

   

    if (!FileSystem::fileExists(file)) {
        string msg = "File '" + file + "' doesn't exist.";
        throw FileNotFound(__FILE__, __LINE__, __func__, msg);
    }

    try {
        dbConnection_ = &db_->connect(file);
    } catch (const RelationalDBException& exception) {
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    }

}

/**
 * The Destructor.
 *
 * Closes the database connection.
 */
DSDBManager::~DSDBManager() {
    db_->close(*dbConnection_);
    delete db_;
}

/**
 * Creates a new DSDB file, and a DSDB manager for it.
 *
 * @param file Full path to the DSDB file to create.
 * @return An instance of DSDBManager with the newly created empty DSDB.
 * @throw IOException if the DSDB file creation was not succesful.
 */
DSDBManager*
DSDBManager::createNew(const std::string& file)
    throw (IOException) {

    if (!FileSystem::fileIsCreatable(file)) {
        const string procName = "DSDBManager::createNew";
        throw IOException(__FILE__, __LINE__, procName);
    }

    try {
        SQLite db;
        RelationalDBConnection& connection = db.connect(file);
    
        // create tables to the database
        connection.DDLQuery(CREATE_ARCH_TABLE);
        connection.DDLQuery(CREATE_IMPL_TABLE);
        connection.DDLQuery(CREATE_MACHINE_CFG_TABLE);
        connection.DDLQuery(CREATE_APPLICATION_TABLE);
        connection.DDLQuery(CREATE_CYCLE_COUNT_TABLE);
        connection.DDLQuery(CREATE_ENERGY_ESTIMATE_TABLE);

        db.close(connection);
    } catch (const Exception& e) {
        debugLog(
            std::string("Initialization of DSDB failed. ") + 
            e.errorMessage());
        std::cerr << e.errorMessage() << std::endl;
        assert(false);
    }
        
    return new DSDBManager(file);
}

/**
 * Returns absolute path of the database file.
 *
 * @return Database file path.
 */
std::string
DSDBManager::dsdbFile() const {
    return FileSystem::absolutePathOf(file_);
}

/**
 * Adds machine architecture to the database.
 *
 * @param mom Machine architecture to add.
 * @return RowID of the added architecture.
 */
RowID
DSDBManager::addArchitecture(const TTAMachine::Machine& mom) 
    throw (RelationalDBException) {

    string adf = "";
    try {
        dbConnection_->beginTransaction();
        ADFSerializer serializer;
        serializer.setDestinationString(adf);
        serializer.writeState(mom.saveState());
        
        // limit the size of the adf
        if (adf.size() >= 5000000) {
            RelationalDBException error(
                    __FILE__, __LINE__, __func__,
                    "ADF size too big.");
            throw error;
        }
    } catch (const SerializerException& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    RowID id = -1;
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO architecture(id, adf_xml) VALUES"
                "(NULL,\'" + adf + "\');"));

        id = dbConnection_->lastInsertRowID();
        dbConnection_->commit();
    } catch (const RelationalDBException& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }

    return id;
}

/**
 * Adds machine implementation to the database.
 *
 * @param impl Machine implementation to add.
 * @param longestPathDelay Longest path delay in seconds.
 * @param area Area as number of gates.
 * @return RowID of the added architecture.
 */
RowID
DSDBManager::addImplementation(
    const IDF::MachineImplementation& impl,
    double longestPathDelay,
    AreaInGates area)
    throw (KeyNotFound) {

    RowID id = -1;
    try {
        dbConnection_->beginTransaction();
        IDF::IDFSerializer serializer;
        string idf = "";
        serializer.setDestinationString(idf);
        serializer.writeState(impl.saveState());
    
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO implementation("
                "id, idf_xml, lpd, area) VALUES"
                "(NULL,\'" + idf + "\'," +
                Conversion::toString(longestPathDelay) + ", "+
                Conversion::toString(area) + ");"));

        id = dbConnection_->lastInsertRowID();
        dbConnection_->commit();
    } catch (const RelationalDBException& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }

    return id;
}

/**
 * Adds a new machine configuration to the database.
 *
 * @param conf Configuration to add.
 * @return RowID of the new configuration.
 * @throw KeyNotFound if the configuration contained unknown IDs.
 */
RowID
DSDBManager::addConfiguration(const MachineConfiguration& conf)
    throw (KeyNotFound) {

    RowID id = -1;

    if (!hasArchitecture(conf.architectureID)) {
        std::string msg =
            "Architecture with ID " +
            Conversion::toString(conf.architectureID) +
            "not found.";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }

    if (conf.hasImplementation && !hasImplementation(conf.implementationID)) {
        std::string msg =
            "Implementation with ID " +
            Conversion::toString(conf.implementationID) +
            "not found.";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }

    try {
        dbConnection_->beginTransaction();
        std::string implID;
        if (conf.hasImplementation) {
            implID = Conversion::toString(conf.implementationID);
        } else {
            implID = "NULL";
        }

        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO machine_configuration("
                "id, architecture, implementation) VALUES"
                "(NULL,") +
            Conversion::toString(conf.architectureID) + ", " +
            implID + ");");

        id = dbConnection_->lastInsertRowID();
        dbConnection_->commit();
    } catch (const RelationalDBException& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }

    return id;
}

/**
 * Returns machine configuration with the give ID.
 *
 * @param id ID of the machine configuration to return.
 * @return Machine configuration with the give ID.
 * @throw KeyNotFound If a configuration with the given ID was not found.
 */
DSDBManager::MachineConfiguration
DSDBManager::configuration(RowID id) const
    throw (KeyNotFound) {

    if (!hasConfiguration(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no configuration with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT architecture, implementation FROM "
            "machine_configuration WHERE id=" +
            Conversion::toString(id) + ";");

    } catch (const Exception& e) {
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        abortWithError("No rows in result!");
    }

    result->next();
    MachineConfiguration conf;
    conf.architectureID = result->data(0).integerValue();
    if (result->data(1).isNull()) {
        conf.hasImplementation = false;
        conf.implementationID = -1;
    } else {
        conf.hasImplementation = true;
        conf.implementationID = result->data(1).integerValue();
    }
    delete result;
    return conf;
}

/**
 * Removes configuration with the give ID from the database.
 *
 * Arhcitecture and implementation that are referenced by the configuration
 * are not removed.
 * 
 * @param id ID of the configuration to remove.
 * @throw KeyNotFound If a configuration ID was not found in the database.
 */
void
DSDBManager::removeConfiguration(RowID id)
    throw (KeyNotFound) {

    if (!hasConfiguration(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no configuration with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    try {
        dbConnection_->beginTransaction();
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM machine_configuration "
                "WHERE id=" + Conversion::toString(id) + ";"));

        dbConnection_->commit();
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}


   
/**
 * Adds a new application to the database.
 *
 * @param path Path of the application test case.
 * @return RowID of the added application.
 */
RowID
DSDBManager::addApplication(const std::string& path) {

    RowID id = -1;
    try {
        dbConnection_->beginTransaction();
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO application(id, path) VALUES"
                "(NULL,\"" + path + "\");"));

        id = dbConnection_->lastInsertRowID();
        dbConnection_->commit();
    } catch (const RelationalDBException& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
    return id;
}

/**
 * Adds an energy estimate for application on specific implementation.
 *
 * @param application RowID of the application.
 * @param implementation RowID of the machine implementation.
 * @param energy Application energy estimate as joules.
 */
void
DSDBManager::addEnergyEstimate(
    RowID application, RowID implementation, double energyEstimate)
    throw (KeyNotFound) {

    if (!hasApplication(application)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no application with id '%d'."
            "Can't add an energy estimate.") 
            % file_ % application).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }
    if (!hasImplementation(implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'."
            "Can't add an energy estimate.") 
            % file_ % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    std::string q =
        "INSERT INTO energy_estimate ("
        "application, implementation, energy_estimate) VALUES(" +
        Conversion::toString(application) + ", " +
        Conversion::toString(implementation) + ", " +
        Conversion::toString(energyEstimate) + ");";

    dbConnection_->updateQuery(q);
}

/**
 * Adds cycle count of an application on specific architecture.
 *
 * @param application RowID of the application.
 * @param architecture RowID of the machine architecture.
 * @param count Cycle count
 */
void
DSDBManager::addCycleCount(
    RowID application, RowID architecture, ClockCycleCount count)
    throw (KeyNotFound) {

    if (!hasApplication(application)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no application with id '%d'."
            "Can't add a cycle count.") 
            % file_ % application).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }
    if (!hasArchitecture(architecture)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no architecture with id '%d'."
            "Can't add a cycle count.") 
            % file_ % architecture).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    std::string q =
        "INSERT INTO cycle_count(application, architecture, cycles) VALUES(" +
        Conversion::toString(application) + ", " +
        Conversion::toString(architecture) + ", " +
        Conversion::toString(count) + ");";

    dbConnection_->updateQuery(q);
}

/**
 * Sets the longest path delay estimate for an implementation.
 *
 * @param implementation RowID of the machine implementation.
 * @param delay Longest path delay estimate of implementation in nanoseconds.
 */
void
DSDBManager::setLongestPathDelayEstimate(
    RowID implementation, double delay)
    throw (KeyNotFound) {

    if (!hasImplementation(implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'.") 
            % file_ % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    std::string q =
        "UPDATE implementation SET "
        "lpd=" +
        Conversion::toString(delay) + " WHERE implementation.id=" +
        Conversion::toString(implementation) + ";";

    dbConnection_->updateQuery(q);
}

/**
 * Sets the area estimate for an implementation.
 *
 * @param implementation RowID of the machine implementation.
 * @param area Implementation area estimate in gates.
 */
void
DSDBManager::setAreaEstimate(
    RowID implementation, AreaInGates areaEstimate)
    throw (KeyNotFound) {

    if (!hasImplementation(implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'.") 
            % file_ % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    std::string q =
        "UPDATE implementation SET "
        "area=" +
        Conversion::toString(areaEstimate) + " WHERE implementation.id=" +
        Conversion::toString(implementation) + ";";        

    dbConnection_->updateQuery(q);
}

/**
 * Returns the machine architecture of the given id as string.
 *
 * @param id RowID of the machine architecture.
 * @return The architecture as string.
 */
std::string
DSDBManager::architectureString(RowID id) const
    throw (KeyNotFound) {

    if (!hasArchitecture(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no architecture with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            "SELECT adf_xml FROM architecture WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (const Exception& e) {
        delete result;
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        delete result;
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& adfData = result->data(0);

    std::string arch = adfData.stringValue();
    delete result;
    return arch;
}

/**
 * Returns machine architecture with the given id.
 *
 * @param id RowID of the machine architecture.
 * @return Machine object model of the architecture.
 */
TTAMachine::Machine*
DSDBManager::architecture(RowID id) const
    throw (KeyNotFound) {

    const std::string adf = architectureString(id);
    ADFSerializer serializer;
    serializer.setSourceString(adf);
    ObjectState* state = serializer.readState();
    TTAMachine::Machine* mach = new TTAMachine::Machine();
    mach->loadState(state);

    return mach;
}

/**
 * Writes the machine architecture of given ID to file.
 *
 * @param id RowID of the machine architecture.
 * @param path File path where the ADF file is written.
 */
void
DSDBManager::writeArchitectureToFile(RowID id, const std::string& path) const
    throw (KeyNotFound, IOException) {

    const std::string adf = architectureString(id);
    ADFSerializer serializer;
    serializer.setSourceString(adf);
    serializer.setDestinationFile(path);
    try {
        ObjectState* state = serializer.readState();
        serializer.writeState(state);
    } catch (const SerializerException& exception) {
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    } 
}

/**
 * Returns machine implementation with the given id as string.
 *
 * @param id RowID of the machine implementation.
 * @return The implementation as string.
 */
std::string
DSDBManager::implementationString(RowID id) const
    throw (KeyNotFound) {

    if (!hasImplementation(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT idf_xml FROM implementation WHERE id=" +
            Conversion::toString(id) + ";");

    } catch (const Exception& e) {
        delete result;
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        delete result;
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& idfData = result->data(0);

    std::string implString = idfData.stringValue(); 
    delete result;
    return implString;
}

/**
 * Returns machine implementation with the given id.
 *
 * @param id RowID of the machine implementation.
 * @return MachineImplementation object containing the implementation.
 */
IDF::MachineImplementation*
DSDBManager::implementation(RowID id) const
    throw (KeyNotFound) {

    const std::string idf = implementationString(id);
    IDF::IDFSerializer serializer;
    serializer.setSourceString(idf);
    ObjectState* state = serializer.readState();
    IDF::MachineImplementation* impl = new IDF::MachineImplementation();
    impl->loadState(state);

    return impl;
}

/**
 * Writes the machine implementation of given ID to file.
 *
 * @param id RowID of the machine implementation.
 * @param path File path where the IDF file is written.
 */
void
DSDBManager::writeImplementationToFile(RowID id, const std::string& path) const
    throw (KeyNotFound, IOException) {
    
    const std::string idf = implementationString(id);
    IDF::IDFSerializer serializer;
    serializer.setSourceString(idf);
    serializer.setDestinationFile(path);
    try {
        ObjectState* state = serializer.readState();
        serializer.writeState(state);
    } catch (const SerializerException& exception) {
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    }        
}
    
/**
 * Writes the machine configuration to files path.{idf,adf}.
 *
 * @param conf MachineConfiguration to be written to a path.
 * @param path Path where the ADF and possibly the IDF of the configuration
 *             will be written.
 */
void 
DSDBManager::writeConfigurationToFile(
    const MachineConfiguration& conf, 
    const std::string& path)
    throw (KeyNotFound, IOException) {

    const std::string adfFile = path + ".adf";
    const std::string idfFile = path + ".idf";

    writeArchitectureToFile(conf.architectureID, adfFile);
    if (conf.hasImplementation) {
        writeImplementationToFile(conf.implementationID, idfFile);
    }
}

/**
 * Returns path of an application with the given ID.
 *
 * @param id RowID of the application test case.
 * @return Path of the application test case.
 */
std::string
DSDBManager::applicationPath(RowID id) const
    throw (KeyNotFound) {

    if (!hasApplication(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no application with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            "SELECT path FROM application WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (const Exception& e) {
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        abortWithError("No rows in result!");
    }

    result->next();
    const std::string path = result->data(0).stringValue();
    delete result;
    return path;
}


/**
 * Checks if energy estimate exists for an application and implementation
 * with given IDs.
 *
 * @param application ID of the application.
 * @param implementation ID of the machine implementation.
 * @return True, if an energy estimate exists in the DB.
 */ 
bool
DSDBManager::hasEnergyEstimate(RowID application, RowID implementation)
    const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT energy_estimate FROM energy_estimate WHERE application=" +
            Conversion::toString(application) + " AND " +
            "implementation=" + Conversion::toString(implementation) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Checks if the database contains application with the given RowID.
 *
 * @param id ID of the application.
 * @return True, if an application with the given ID exists in the DB.
 */ 
bool
DSDBManager::hasApplication(RowID id) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT path FROM application WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Checks if the database contains application with the given application path.
 *
 * @param applicationPath Path of the application.
 * @return True, if an application with the given path exists in the DB.
 */
bool
DSDBManager::hasApplication(const std::string& applicationPath) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT * FROM application WHERE path='" +
            applicationPath + "';");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Removes the application that has the given ID.
 * @param id Application ID to remove.
 * @exception KeyNotFound If the application ID was not found in the database.
 */
void
DSDBManager::removeApplication(RowID id)
    throw (KeyNotFound) {

    if (!hasApplication(id)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no application with id '%d'.") 
            % file_ % id).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    try {
        dbConnection_->beginTransaction();
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM application "
                "WHERE id=" + Conversion::toString(id) + ";"));

        dbConnection_->commit();
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Checks if the database contains architecture with the given RowID.
 *
 * @param id ID of the architecture.
 * @return True, if an architecture with the given ID exists in the DB.
 */ 
bool
DSDBManager::hasArchitecture(RowID id) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT id FROM architecture WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Checks if the database contains configuration with the given RowID.
 *
 * @param id ID of the configuration.
 * @return True, if a configuration with the given ID exists in the DB.
 */ 
bool
DSDBManager::hasConfiguration(RowID id) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT id FROM machine_configuration WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Checks if the database contains implementation with the given RowID.
 *
 * @param id ID of the implementation
 * @return True, if implementation with the given ID exists in the DB.
 */ 
bool
DSDBManager::hasImplementation(RowID id) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT id FROM implementation WHERE id=" +
            Conversion::toString(id) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Returns energy estimate for an application on specific implementation.
 * 
 * @param application RowID of the application.
 * @param implementation RowID of the implementation.
 * @return Energy estimate in joules.
 * @exception KeyNotFound If an energy estimate was not found.
 */
double
DSDBManager::energyEstimate(RowID application, RowID implementation) const
    throw (KeyNotFound) {

    if (!hasEnergyEstimate(application, implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no energy estimate with an application id "
            "'%d', and with an implementation id '%d'.") 
            % file_ % application % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT energy_estimate FROM energy_estimate WHERE application=" +
            Conversion::toString(application) + " AND implementation=" +
            Conversion::toString(implementation) + ";");

    } catch (const Exception& e) {
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& data = result->data(0);
    double energyEstimate = data.doubleValue();
    delete result;
    return energyEstimate;
}


/**
 * Checks if cycle count exists for an application and architecture
 * with given IDs.
 *
 * @param application ID of the application.
 * @param architecture ID of the machine architecture
 * @return True, if a cycle count exists in the DB.
 */ 
bool
DSDBManager::hasCycleCount(RowID application, RowID architecture) const {

    RelationalDBQueryResult* result = NULL;
    
    try {
        result = dbConnection_->query(
            "SELECT cycles FROM cycle_count WHERE application=" +
            Conversion::toString(application) + " AND " +
            "architecture=" + Conversion::toString(architecture) + ";");
    } catch (Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}


/**
 * Returns cycle count for an application on specific architecture.
 * 
 * @param application RowID of the application.
 * @param architecture RowID of the architecture.
 * @return Cycle count in cycles.
 * @exception KeyNotFound If cycle count was not found in DB.
 */
ClockCycleCount 
DSDBManager::cycleCount(RowID application, RowID architecture) const
    throw (KeyNotFound) {
    
    if (!hasCycleCount(application, architecture)) {
        const std::string error = (boost::format(
            "No cycle count found for application in DSDB file '%s'") 
            % file_).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT cycles FROM cycle_count WHERE application=" +
            Conversion::toString(application) + " AND architecture=" +
            Conversion::toString(architecture) + ";");

    } catch (const Exception& e) {
        delete result;
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        delete result;
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& data = result->data(0);
    ClockCycleCount count = static_cast<ClockCycleCount>(data.doubleValue());
    delete result;
    result = NULL;
    return count;
}

/**
 * Returns longest path delay estimate for an specific implementation.
 * 
 * @param implementation RowID of the machine implementation.
 * @return Longest path delay in nanoseconds.
 * @exception KeyNotFound If the implmentation was not found in DB.
 */
double
DSDBManager::longestPathDelayEstimate(RowID implementation) const
    throw (KeyNotFound) {
    
    if (!hasImplementation(implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'.") 
            % file_ % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT lpd FROM implementation WHERE id=" +
            Conversion::toString(implementation) + ";");

    } catch (const Exception& e) {
        delete result;
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        delete result;
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& data = result->data(0);
    double delay = data.doubleValue();
    delete result;
    return delay;
}

/**
 * Returns area estimate for an specific implementation.
 * 
 * @param implementation RowID of the machine implementation.
 * @return Area estimate in gates.
 * @exception KeyNotFound If the implementation was not found in DB.
 */
AreaInGates
DSDBManager::areaEstimate(RowID implementation) const
    throw (KeyNotFound) {
    
    if (!hasImplementation(implementation)) {
        const std::string error = (boost::format(
            "DSDP file '%s' has no implementation with id '%d'.") 
            % file_ % implementation).str();
        throw KeyNotFound(__FILE__, __LINE__, __func__, error);
    }

    RelationalDBQueryResult* result = NULL;

    try {
        result = dbConnection_->query(
            "SELECT area FROM implementation WHERE id=" +
            Conversion::toString(implementation) + ";");

    } catch (const Exception& e) {
        delete result;
        abortWithError(e.errorMessage());
    }

    if (!result->hasNext()) {
        delete result;
        abortWithError("No rows in result!");
    }

    result->next();
    const DataObject& data = result->data(0);
    AreaInGates area = data.integerValue();
    delete result;
    return area;
}

/**
 * Returns IDs of all applications in the database.
 *
 * @return RowIDs of all applications in the database.
 */
std::set<RowID>
DSDBManager::applicationIDs() const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* result = NULL;
    try {
        std::string theQuery =
            std::string("SELECT id FROM application;");

        result = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        delete result;
        assert(false);        
    } 

    std::set<RowID> ids;

    while (result->hasNext()) {
        result->next();

        ids.insert(result->data("id").integerValue());
    }

    delete result;
    return ids;
}

/**
 * Returns IDs of all machine architectures in the database.
 *
 * @return RowIDs of all architectures in the database.
 */
std::set<RowID>
DSDBManager::architectureIDs() const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string("SELECT id FROM architecture;");

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        delete queryResult;
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    return ids;
}

/**
 * Returns IDs of all machine configurations in the database.
 *
 * @return RowIDs of all configurations in the database.
 */
std::set<RowID>
DSDBManager::configurationIDs() const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string("SELECT id FROM machine_configuration;");

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        delete queryResult;
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    return ids;
}

/**
 * Returns IDs of all configurations referencing the given architecture.
 *
 * @param architectureID ID of the architecture to search configurations for.
 * @return Configuration IDs of a machine architecture.
 */
std::set<RowID>
DSDBManager::archConfigurationIDs(RowID architectureID) const
    throw (KeyNotFound) {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            "SELECT id FROM machine_configuration WHERE architecture=" +
            Conversion::toString(architectureID) + ";";

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        delete queryResult;
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    return ids;
}


/**
 * Returs set of ConfigurationCosts ordered by the given ordering.
 *
 * @param ordering Ordering of the results.
 * @return ConfigurationCosts of all configurations in the DSDB ordered by
 * the given option.
 */
std::vector<DSDBManager::ConfigurationCosts>
DSDBManager::applicationCostEstimatesByConf(Order ordering) const {

    RelationalDBQueryResult* appResult = NULL;
    try {
        appResult = dbConnection_->query("select * from application;");
    }
    catch (const Exception& e) {
        delete appResult;
        abortWithError(e.errorMessage());
    }
    vector<ApplicationData> appData;
   
    while (appResult->hasNext()) {
        appResult->next();
        ApplicationData data;
        data.id = appResult->data(0).stringValue();
        data.name = appResult->data(1).stringValue();
        appData.push_back(data);
    }
    delete appResult;
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(
            "SELECT id from machine_configuration;");
    } catch (const Exception& e) {
        delete queryResult;
        abortWithError(e.errorMessage());
    }
   
    vector<ConfigurationCosts> res;

    set<ConfigurationCosts, idComparator> results;

    while (queryResult->hasNext()) {
        queryResult->next();
        for (unsigned int i = 0; i < appData.size(); i++) {
            ConfigurationCosts cc;
            cc.configurationID = queryResult->data(0).integerValue();
            cc.application = appData.at(i).name;
            RelationalDBQueryResult* impResult = NULL;
            
            try {
                impResult = dbConnection_->query(
                    "select lpd, area from implementation, machine_configuration "
                    "where machine_configuration.id=" + queryResult->data(0).stringValue() +
                    " and machine_configuration.implementation = implementation.id limit 1;");
            } catch (const Exception& e) {
                delete impResult;
                abortWithError(e.errorMessage());
            }
            if (impResult->hasNext()) {
                impResult->next();
                cc.longestPathDelay = impResult->data(0).doubleValue();
                cc.area = impResult->data(1).doubleValue();
            } else {
                cc.longestPathDelay = NULL;
                cc.area = NULL;
            }
            RelationalDBQueryResult* energyResult = NULL;
            try {
                energyResult = dbConnection_->query(
                    "select energy_estimate from energy_estimate, machine_configuration, application "
                    "where application.id=" + appData[i].id + " and machine_configuration.id="
                    + queryResult->data(0).stringValue() + " and machine_configuration.implementation="
                    "energy_estimate.implementation and application.id=energy_estimate.application;");
             } catch (const Exception& e) {
                 delete energyResult;
                 abortWithError(e.errorMessage());
             }
             if (energyResult->hasNext()) {
                 energyResult->next();
                 cc.energyEstimate = energyResult->data(0).doubleValue();
             } else {
                 cc.energyEstimate = NULL;
             }
             delete energyResult;

             RelationalDBQueryResult* cycleResult = NULL;
             try {
                 cycleResult = dbConnection_->query(
                     "select cycles from cycle_count, application, machine_configuration where application.id="
                     + appData.at(i).id + " and machine_configuration.id=" + 
                     queryResult->data(0).stringValue() +
                     " and machine_configuration.architecture=cycle_count.architecture and application.id="
                     "cycle_count.application;");
             } catch (const Exception& e) {
                 delete cycleResult;
                 abortWithError(e.errorMessage());
             }
             if (cycleResult->hasNext()) {
                 cycleResult->next();
                 cc.cycleCount = cycleResult->data(0).integerValue();
             } else {
                 cc.cycleCount = NULL;
             }
             delete cycleResult;
            
             results.insert(cc);
         }
     }   
     delete queryResult;   
     if (ordering == ORDER_BY_CYCLE_COUNT) {
         set<ConfigurationCosts, cycleComparator> cycleResults;
         for (set<ConfigurationCosts, idComparator>::iterator i = results.begin(); i != results.end(); i++) {
             cycleResults.insert(*i);
         }
        
         for (set<ConfigurationCosts, cycleComparator>::iterator i = cycleResults.begin(); i != cycleResults.end(); i++) {
             res.push_back(*i);
         }
         return res;
    } else if (ordering == ORDER_BY_APPLICATION) {
        set<ConfigurationCosts, appComparator> appResults;
        for (set<ConfigurationCosts, idComparator>::iterator i = results.begin(); i != results.end(); i++) {
           appResults.insert(*i);
        }
        for (set<ConfigurationCosts, appComparator>::iterator i = appResults.begin(); i != appResults.end(); i++) {
            res.push_back(*i);
        }
    } else if (ordering == ORDER_BY_ENERGY_ESTIMATE) {
        set<ConfigurationCosts, energyComparator> energyResults;
        for (set<ConfigurationCosts, idComparator>::iterator i = results.begin(); i != results.end(); i++) {
           energyResults.insert(*i);
        }
        for (set<ConfigurationCosts, energyComparator>::iterator i = energyResults.begin(); i != energyResults.end(); i++) {
            res.push_back(*i);
        }
    } else {
        for (set<ConfigurationCosts, idComparator>::iterator i = results.begin(); i != results.end(); i++) {
            res.push_back(*i);
        }
    }
   
    return res;
}

/**
 * Returns the number of applications in the dsdb.
 *
 * @return The number of applications in the dsdb.
 */
int
DSDBManager::applicationCount() const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            "SELECT COUNT(id) FROM application;";

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        delete queryResult;
        assert(false);        
    }

    int result = 0;
    if (queryResult->hasNext()) {
        queryResult->next();
        result = queryResult->data(0).integerValue();
    }
    delete queryResult;
    return result;
}
