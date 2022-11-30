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
 * @file HDBManager.cc
 *
 * Implementation of HDBManager class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <list>
#include <map>
#include <boost/format.hpp>

#include "HDBManager.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "FUExternalPort.hh"
#include "RFExternalPort.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"
#include "BlockImplementationFile.hh"
#include "CostFunctionPlugin.hh"

#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"

#include "SQLite.hh"
#include "RelationalDBQueryResult.hh"
#include "DataObject.hh"
#include "Application.hh"
#include "MapTools.hh"
#include "AssocTools.hh"
#include "SetTools.hh"

#include "FUValidator.hh"
#include "MachineValidatorResults.hh"
#include "Conversion.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

// Remove this define when opcode editing should be disabled.
// Also edit HDBEditor/FUImplementationDialog.hh/.cc to hide the ability to
// modify opcodes
#define ALLOW_OPCODE_EDITING

const bool READ_ACTION = true;
const bool WRITE_ACTION = false;

const string IN_DIRECTION = "IN";
const string OUT_DIRECTION = "OUT";
const string BIDIR_DIRECTION = "BIDIR";
const string VHDL_FORMAT = "VHDL";
const string VERILOG_FORMAT = "Verilog";
const string VHDL_SIM_FORMAT = "VHDL simulation";
const string VERILOG_SIM_FORMAT = "Verilog simulation";

const int DEFAULT_PORT_WIDTH = 32;

/// Possible cost function plugin types
const std::string COST_PLUGIN_TYPE_FU = "fu";
const std::string COST_PLUGIN_TYPE_RF = "rf";
const std::string COST_PLUGIN_TYPE_DECOMP = "decomp";
const std::string COST_PLUGIN_TYPE_ICDEC = "icdec";

// database table Creation Queries (CQ)
const string CQ_FU =
    "CREATE TABLE fu ("
    "       id INTEGER PRIMARY KEY,"
    "       architecture REFERENCES fu_architecture(id),"
    "       cost_function REFERENCES cost_function_plugin(id));";

const string CQ_FU_ARCHITECTURE =
    "CREATE TABLE fu_architecture("
    "   id INTEGER PRIMARY KEY);";

const string CQ_PIPELINE_RESOURCE =
    "CREATE TABLE pipeline_resource("
    "       id INTEGER PRIMARY KEY,"
    "       fu_arch REFERENCES fu_architecture(id) NOT NULL);";

const string CQ_OPERATION_PIPELINE =
    "CREATE TABLE operation_pipeline("
    "       id INTEGER PRIMARY KEY,"
    "       fu_arch REFERENCES fu_architecture(id) NOT NULL,"
    "       operation REFERENCES operation(id) NOT NULL);";

const string CQ_PIPELINE_RESOURCE_USAGE =
    "CREATE TABLE pipeline_resource_usage("
    "       id INTEGER PRIMARY KEY,"
    "       cycle INTEGER NOT NULL,"
    "       resource REFERENCES pipeline_resource(id) NOT NULL,"
    "       pipeline REFERENCES operation_pipeline(id) NOT NULL);";

const string CQ_IO_USAGE =
    "CREATE TABLE io_usage("
    "       id INTEGER PRIMARY KEY,"
    "       cycle INTEGER NOT NULL,"
    "       io_number INTEGER NOT NULL,"
    "       action BOOLEAN NOT NULL,"
    "       pipeline REFERENCES operation_pipeline(id) NOT NULL);";

const string CQ_OPERATION =
    "CREATE TABLE operation("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT UNIQUE NOT NULL);";

const string CQ_FU_DATA_PORT =
    "CREATE TABLE fu_data_port("
    "       id INTEGER PRIMARY KEY,"
    "       triggers BOOLEAN NOT NULL,"
    "       sets_opcode BOOLEAN NOT NULL,"
    "       guard_support BOOLEAN NOT NULL,"
    "       width INTEGER,"
    "       fu_arch REFERENCES fu_architecture(id));";

const string CQ_IO_BINDING =
    "CREATE TABLE io_binding("
    "       id INTEGER PRIMARY KEY,"
    "       io_number INTEGER NOT NULL,"
    "       port REFERENCES fu_data_port(id) NOT NULL,"
    "       operation REFERENCES operation(id) NOT NULL);";

const string CQ_FU_IMPLEMENTATION =
    "CREATE TABLE fu_implementation("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       opcode_port TEXT,"
    "       clk_port TEXT NOT NULL,"
    "       rst_port TEXT NOT NULL,"
    "       glock_port TEXT NOT NULL,"
    "       glock_req_port TEXT,"
    "       fu REFERENCES fu(id) UNIQUE NOT NULL);";

const string CQ_OPCODE_MAP =
    "CREATE TABLE opcode_map("
    "       id INTEGER PRIMARY KEY,"
    "       opcode INTEGER NOT NULL,"
    "       operation REFERENCES operation(id) NOT NULL,"
    "       fu_impl REFERENCES fu_implementation(id) NOT NULL);";

const string CQ_FU_PORT_MAP =
    "CREATE TABLE fu_port_map("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       width_formula TEXT NOT NULL,"
    "       load_port TEXT,"
    "       guard_port TEXT,"
    "       fu_impl REFERENCES fu_implementation(id) NOT NULL,"
    "       arch_port REFERENCES fu_data_port(id) NOT NULL,"
    "       UNIQUE (name, fu_impl));";

const string CQ_FU_EXTERNAL_PORT =
    "CREATE TABLE fu_external_port("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       direction TEXT NOT NULL,"
    "       width_formula TEXT NOT NULL,"
    "       description TEXT,"
    "       fu_impl REFERENCES fu_implementation(id) NOT NULL, "
    "       UNIQUE (name, fu_impl));";

const string CQ_RF_EXTERNAL_PORT =
    "CREATE TABLE rf_external_port("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       direction TEXT NOT NULL,"
    "       width_formula TEXT NOT NULL,"
    "       description TEXT,"
    "       rf_impl REFERENCES rf_implementation(id) NOT NULL, "
    "       UNIQUE (name, rf_impl));";

const string CQ_FU_IMPLEMENTATION_PARAMETER =
    "CREATE TABLE fu_implementation_parameter("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       type TEXT,"
    "       value TEXT,"
    "       fu_impl REFERENCES fu_implementation(id) NOT NULL);";

const string CQ_RF_IMPLEMENTATION_PARAMETER =
    "CREATE TABLE rf_implementation_parameter("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       type TEXT,"
    "       value TEXT,"
    "       rf_impl REFERENCES rf_implementation(id) NOT NULL);";

const string CQ_FU_EXT_PORT_PARAMETER_DEPENDENCY =
    "CREATE TABLE fu_ext_port_parameter_dependency("
    "       id INTEGER PRIMARY KEY,"
    "       port REFERENCES fu_external_port(id) NOT NULL,"
    "       parameter REFERENCES fu_implementation_parameter(id) NOT NULL);";

const string CQ_RF_EXT_PORT_PARAMETER_DEPENDENCY =
    "CREATE TABLE rf_ext_port_parameter_dependency("
    "       id INTEGER PRIMARY KEY,"
    "       port REFERENCES rf_external_port(id) NOT NULL,"
    "       parameter REFERENCES rf_implementation_parameter(id) NOT NULL);";

const string CQ_RF =
    "CREATE TABLE rf("
    "       id INTEGER PRIMARY KEY,"
    "       architecture REFERENCES rf_architecture(id),"
    "       cost_function REFERENCES cost_function_plugin(id));";

const string CQ_RF_ARCHITECTURE =
    "CREATE TABLE rf_architecture("
    "       id INTEGER PRIMARY KEY,"
    "       size INTEGER,"
    "       width INTEGER,"
    "       read_ports INTEGER NOT NULL,"
    "       write_ports INTEGER NOT NULL,"
    "       bidir_ports INTEGER NOT NULL,"
    "       latency INTEGER NOT NULL,"
    "       max_reads INTEGER NOT NULL,"
    "       max_writes INTEGER NOT NULL,"
    "       guard_support BOOLEAN NOT NULL,"
    "       guard_latency INTEGER NOT NULL,"
    "       zero_register BOOLEAN DEFAULT FALSE);";

const string CQ_RF_IMPLEMENTATION =
    "CREATE TABLE rf_implementation("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       size_param TEXT,"
    "       width_param TEXT,"
    "       clk_port TEXT NOT NULL,"
    "       rst_port TEXT NOT NULL,"
    "       glock_port TEXT NOT NULL,"
    "       guard_port TEXT,"
    "       rf REFERENCES rf(id) NOT NULL,"
    "       sac_param INTEGER DEFAULT 0);"; // Separate address cycle

const string CQ_RF_DATA_PORT =
    "CREATE TABLE rf_data_port("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL,"
    "       direction TEXT NOT NULL,"
    "       load_port TEXT NOT NULL,"
    "       opcode_port TEXT NOT NULL,"
    "       opcode_port_width_formula TEXT NOT NULL,"
    "       rf_impl REFERENCES rf_implementation(id),"
    "       UNIQUE (name, rf_impl));";

const string CQ_BLOCK_SOURCE_FILE =
    "CREATE TABLE block_source_file("
    "       id INTEGER PRIMARY KEY,"
    "       file TEXT NOT NULL,"
    "       format REFERENCES format(id));";

const string CQ_RF_SOURCE_FILE =
    "CREATE TABLE rf_source_file("
    "       id INTEGER PRIMARY KEY,"
    "       rf_impl REFERENCES rf_implementation(id),"
    "       file REFERENCES block_source_file(id));";

const string CQ_FU_SOURCE_FILE =
    "CREATE TABLE fu_source_file("
    "       id INTEGER PRIMARY KEY,"
    "       fu_impl REFERENCES fu_implementation(id),"
    "       file REFERENCES block_source_file(id));";

const string CQ_FORMAT =
    "CREATE TABLE format("
    "       id INTEGER PRIMARY KEY,"
    "       format TEXT NOT NULL);";

const string CQ_BUS =
    "CREATE TABLE bus ("
    "       id INTEGER PRIMARY KEY);";

const string CQ_SOCKET =
    "CREATE TABLE socket ("
    "       id INTEGER PRIMARY KEY);";

const string CQ_COST_FUNCTION_PLUGIN = 
    "CREATE TABLE cost_function_plugin("
    "       id INTEGER PRIMARY KEY, "
    "       description TEXT, "
    "       name TEXT, "
    "       plugin_file_path TEXT NOT NULL, "
    "       type STRING NOT NULL);"; /// type: {'fu'|'rf'|'decomp'|'icdec'}

const string CQ_COST_ESTIMATION_DATA =
    "CREATE TABLE cost_estimation_data("
    "       id INTEGER PRIMARY KEY, "
    "       plugin_reference REFERENCES cost_function_plugin(id), "
    "       rf_reference REFERENCES rf(id), "
    "       fu_reference REFERENCES fu(id), "
    "       bus_reference REFERENCES bus(id), "
    "       socket_reference REFERENCES socket(id), "
    "       name TEXT, "
    "       value TEXT);";

const string CQ_FU_PORT_MAP_ARCH_INDEX =
    "CREATE INDEX fu_port_map_arch_index ON fu_port_map(arch_port)";

const string CQ_FU_IMPL_ENTRY_INDEX =
    "CREATE INDEX fu_impl_entry_index ON fu_implementation(fu)";

const string CQ_RF_IMPL_ENTRY_INDEX =
    "CREATE INDEX rf_impl_entry_index ON rf_implementation(rf)";

const string CQ_OPERATION_IMPLEMENTATION_RESOURCE_SOURCE_FILE =
    "CREATE TABLE operation_implementation_resource_source_file("
    "       id INTEGER PRIMARY KEY,"
    "       resource REFERENCES operation_implementation_resource(id),"
    "       file REFERENCES block_source_file(id)"
    ");";

const string CQ_OPERATION_IMPLEMENTATION_SOURCE_FILE =
    "CREATE TABLE operation_implementation_source_file("
    "       id INTEGER PRIMARY KEY,"
    "       operation REFERENCES operation_implementation(id),"
    "       file REFERENCES block_source_file(id)"
    ");";

const string CQ_OPERATION_IMPLEMENTATION_RESOURCE =
    "CREATE TABLE operation_implementation_resource("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL"
    ");";

const string CQ_OPERATION_IMPLEMENTATION =
    "CREATE TABLE operation_implementation("
    "       id INTEGER PRIMARY KEY,"
    "       name TEXT NOT NULL"
    ");";

const string CQ_OPERATION_IMPLEMENTATION_RESOURCES =
    "CREATE TABLE operation_implementation_resources("
    "       id INTEGER PRIMARY KEY,"
    "       operation REFERENCES operation_implementation(id),"
    "       resource REFERENCES operation_implementation_resource(id),"
    "       count INTEGER NOT NULL"
    ");";

const string CQ_OPERATION_IMPLEMENTATION_VARIABLE =
    "CREATE TABLE operation_implementation_variable("
    "       id INTEGER PRIMARY KEY,"
    "       operation REFERENCES operation_implementation(id),"
    "       name TEXT NOT NULL,"
    "       width TEXT NOT NULL,"
    "       type TEXT NOT NULL,"
    "       language TEXT NOT NULL"
    ");";

namespace HDB {

HDBManager* HDBManager::instance_ = NULL;

/**
 * The constructor.
 *
 * @param hdbFile Name of the HDB file to be managed by the manager.
 * @exception IOException If connection to the DB cannot be established.
 */
HDBManager::HDBManager(const std::string& hdbFile)
    : db_(new SQLite()), dbConnection_(NULL), hdbFile_(hdbFile) {
    if (!FileSystem::fileExists(hdbFile)) {
        string errorMsg = "File '" + hdbFile + "' doesn't exist.";
        throw FileNotFound(__FILE__, __LINE__, __func__, errorMsg);
    }

    if (!FileSystem::fileIsReadable(hdbFile)) {
        string errorMsg = "File '" + hdbFile + "' has no read rights.";
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    try {
        dbConnection_ = &db_->connect(hdbFile);
    } catch (const RelationalDBException& exception) {
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    }

    // Update outdated HDB.
    // Version 0 indicates db without version number also.
    int dbVersion = dbConnection_->version();

    // Version 0 -> 1
    if (dbVersion < 1) {
        // Initial fu-gen tables and fugen stuff.
        dbConnection_->DDLQuery(CQ_OPERATION_IMPLEMENTATION_RESOURCE);
        dbConnection_->DDLQuery(CQ_OPERATION_IMPLEMENTATION);
        dbConnection_->DDLQuery(
            CQ_OPERATION_IMPLEMENTATION_RESOURCE_SOURCE_FILE);
        dbConnection_->DDLQuery(CQ_OPERATION_IMPLEMENTATION_SOURCE_FILE);
        dbConnection_->DDLQuery(CQ_OPERATION_IMPLEMENTATION_RESOURCES);
        dbConnection_->updateQuery(std::string(
                "INSERT INTO format(id,format) VALUES"
                "(NULL,\"" + VHDL_SIM_FORMAT + "\");"));
        dbConnection_->updateQuery(std::string(
                "INSERT INTO format(id,format) VALUES"
                "(NULL,\"" + VERILOG_SIM_FORMAT + "\");"));
        dbConnection_->updateVersion(1);
    }

    // Version 1 -> 2
    if (dbVersion < 2) {
        // Variables for operation snippets.
        dbConnection_->DDLQuery(CQ_OPERATION_IMPLEMENTATION_VARIABLE);
        dbConnection_->updateVersion(2);
    }

    // Version 2 -> 3
    if (dbVersion < 3) {
        // ipxact to resource table
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation_resource ADD COLUMN "
            "ipxact TEXT;"));
        dbConnection_->updateVersion(3);
    }

    // Version 3 -> 4
    if (dbVersion < 4) {
        // support for post-operation (for LOADs) and
        //  operation bus definitions for FUGen.
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation ADD COLUMN "
            "bus_definition TEXT;"));
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation ADD COLUMN "
            "post_op_vhdl TEXT;"));
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation ADD COLUMN "
            "post_op_verilog TEXT;"));
        dbConnection_->updateVersion(4);
    }

    // Version 4 -> 5
    if (dbVersion < 5) {
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation_resource ADD COLUMN "
            "latency INTEGER;"));
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation ADD COLUMN "
            "default_verilog TEXT;"));
        dbConnection_->updateQuery(std::string(
            "ALTER TABLE operation_implementation ADD COLUMN "
            "default_vhdl TEXT;"));
        dbConnection_->updateVersion(5);
    }

    // Version 5 -> 6
    if (dbVersion < 6) {
        try {
            // rename default_vhdl and default_verilog to initial_*
            // and add latency column
            dbConnection_->updateQuery(std::string(
                "ALTER TABLE operation_implementation ADD COLUMN "
                "latency INTEGER;"));
            dbConnection_->updateQuery(std::string(
                "ALTER TABLE operation_implementation RENAME COLUMN "
                "default_vhdl TO initial_vhdl;"));
            dbConnection_->updateQuery(std::string(
                "ALTER TABLE operation_implementation RENAME COLUMN "
                "default_verilog TO initial_verilog;"));
            dbConnection_->updateVersion(6);
        } catch (const RelationalDBException& exception) {
            throw IOException(
                __FILE__, __LINE__, __func__, exception.errorMessage());
        }
        dbConnection_->updateVersion(6);
    }

    if (dbVersion < 7) {
        try {
            if (!hasColumn("rf_architecture", "zero_register")) {
                addBooleanColumn("rf_architecture", "zero_register");
            }
        } catch (const RelationalDBException& exception) {
            throw IOException(
                __FILE__, __LINE__, __func__, exception.errorMessage());
        }
        dbConnection_->updateVersion(7);
    }

}

/**
 * The destructor.
 */
HDBManager::~HDBManager() {
    db_->close(*dbConnection_);
    delete db_;
}


/**
 * Creates a new HDB to the given file.
 *
 * @param file The database file to be created.
 * @exception UnreachableStream If the given file exists already or cannot be
 *                              created for another reason.
 */
void
HDBManager::createNew(const std::string& file) {
    if (!FileSystem::fileIsCreatable(file)) {
        const string procName = "HDBManager::createNew";
        throw UnreachableStream(__FILE__, __LINE__, procName);
    }

    try {
        SQLite db;
        RelationalDBConnection& connection = db.connect(file);

        // create tables to the database
        connection.DDLQuery(CQ_FU);
        connection.DDLQuery(CQ_FU_ARCHITECTURE);
        connection.DDLQuery(CQ_PIPELINE_RESOURCE);
        connection.DDLQuery(CQ_OPERATION_PIPELINE);
        connection.DDLQuery(CQ_PIPELINE_RESOURCE_USAGE);
        connection.DDLQuery(CQ_IO_USAGE);
        connection.DDLQuery(CQ_OPERATION);
        connection.DDLQuery(CQ_FU_DATA_PORT);
        connection.DDLQuery(CQ_IO_BINDING);
        connection.DDLQuery(CQ_FU_IMPLEMENTATION);
        connection.DDLQuery(CQ_OPCODE_MAP);
        connection.DDLQuery(CQ_FU_PORT_MAP);
        connection.DDLQuery(CQ_FU_EXTERNAL_PORT);
        connection.DDLQuery(CQ_FU_IMPLEMENTATION_PARAMETER);
        connection.DDLQuery(CQ_FU_EXT_PORT_PARAMETER_DEPENDENCY);
        connection.DDLQuery(CQ_RF);
        connection.DDLQuery(CQ_RF_ARCHITECTURE);
        connection.DDLQuery(CQ_RF_IMPLEMENTATION);
        connection.DDLQuery(CQ_RF_IMPLEMENTATION_PARAMETER);
        connection.DDLQuery(CQ_RF_EXTERNAL_PORT);
        connection.DDLQuery(CQ_RF_EXT_PORT_PARAMETER_DEPENDENCY);
        connection.DDLQuery(CQ_RF_DATA_PORT);
        connection.DDLQuery(CQ_FORMAT);
        connection.DDLQuery(CQ_BUS);
        connection.DDLQuery(CQ_SOCKET);
        connection.DDLQuery(CQ_COST_FUNCTION_PLUGIN);
        connection.DDLQuery(CQ_COST_ESTIMATION_DATA);
        connection.DDLQuery(CQ_BLOCK_SOURCE_FILE);
        connection.DDLQuery(CQ_RF_SOURCE_FILE);
        connection.DDLQuery(CQ_FU_SOURCE_FILE);
        connection.DDLQuery(CQ_FU_PORT_MAP_ARCH_INDEX);
        connection.DDLQuery(CQ_FU_IMPL_ENTRY_INDEX);
        connection.DDLQuery(CQ_RF_IMPL_ENTRY_INDEX);

        // insert the contents to format table
        insertFileFormats(connection);
        db.close(connection);
    } catch (const Exception& e) {
        debugLog(
            std::string("Initialization of HDB failed. ") +
            e.errorMessage());
        // this should not normally happen, but only if our table creation
        // queries are broken, thus it's a program error and not input error
        assert(false);
    }
}

/**
 * Returns the file name of the HDB managed by this HDBManager.
 *
 * @return Absolute path to the HDB file.
 */
std::string
HDBManager::fileName() const {
    return FileSystem::absolutePathOf(hdbFile_);
}


/**
 * Add the given CostFunctionPlugin to the database.
 *
 * @param plugin The CostFunctionPlugin to add.
 * @return ID of the plugin added.
 * @exception Exception May throw InvalidData if the CostFunctionPlugin type
 * is unknown.
 */
RowID
HDBManager::addCostFunctionPlugin(const CostFunctionPlugin& plugin) const {
    RowID pluginID;
    try {
        dbConnection_->beginTransaction();

        // insert into cost_function_plugin table
        string description = plugin.description();
        string pluginFilePath = plugin.pluginFilePath();
        string type = "";
        switch (plugin.type()) {
        case CostFunctionPlugin::COST_FU:
            type = COST_PLUGIN_TYPE_FU;
            break;
        case CostFunctionPlugin::COST_RF:
            type = COST_PLUGIN_TYPE_RF;
            break;
        case CostFunctionPlugin::COST_DECOMP:
            type = COST_PLUGIN_TYPE_DECOMP;
            break;
        case CostFunctionPlugin::COST_ICDEC:
            type = COST_PLUGIN_TYPE_ICDEC;
            break;
        default:
            InvalidData ex(
                __FILE__, __LINE__, __func__,
                (boost::format("Illegal cost_function_plugin type %d.") %
                 type).str());
            throw ex;
            break;
        }
        string name = plugin.name();

        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO cost_function_plugin(id,description,name,"
                "plugin_file_path,type) VALUES"
                "(NULL,\"" + description + "\",\"" + name + "\",\"" +
                 pluginFilePath + "\",\"" + type + "\");"));
        pluginID = dbConnection_->lastInsertRowID();
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
    return pluginID;
}

/**
 * Removes the CostFunctionPlugin that has the given ID.
 *
 * @param pluginID ID of the cost plugin.
 */
void
HDBManager::removeCostFunctionPlugin(RowID pluginID) const {

    try {
        dbConnection_->beginTransaction();

        // remove from cost_function_plugin table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM cost_function_plugin "
                "WHERE id=" + Conversion::toString(pluginID) + ";"));
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM cost_estimation_data "
                "WHERE plugin_reference=" +
                Conversion::toString(pluginID) + ";"));

        dbConnection_->commit();
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Adds the given FU architecture to the database.
 *
 * @param arch The FU architecture to add.
 * @return ID of the architecture added. 
 * @exception InvalidData If the FU architecture is invalid.
 */
RowID
HDBManager::addFUArchitecture(const FUArchitecture& arch) const {
    // check the operand bindings of the FU
    FunctionUnit& fu = arch.architecture();
    MachineValidatorResults results;
    FUValidator::checkOperations(fu, results);
    FUValidator::checkOperandBindings(fu, results);
    if (results.errorCount() > 0) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, results.error(0).second);
    }
        
    RowID archID;
    std::map<FUPort*, RowID> portIDMap;

    try {
        dbConnection_->beginTransaction();

        // insert into fu_architecture table
        dbConnection_->updateQuery(
            std::string("INSERT INTO fu_architecture(id) VALUES(NULL);"));
        archID = dbConnection_->lastInsertRowID();

        // insert into fu_data_port table
        for (int i = 0; i < fu.operationPortCount(); i++) {
            FUPort* port = fu.operationPort(i);
            string query(
                "INSERT INTO fu_data_port(id,triggers,sets_opcode,"
                "guard_support,width,fu_arch) "
                "VALUES(NULL," + 
                Conversion::toString(port->isTriggering()) + "," + 
                Conversion::toString(port->isOpcodeSetting()) + "," +
                Conversion::toString(arch.hasGuardSupport(port->name()))
                + ",");
            if (arch.hasParameterizedWidth(port->name())) {
                query += "NULL,";
            } else {
                query += (Conversion::toString(port->width()) + ",");
            }
            query += (Conversion::toString(archID) + ");");
            dbConnection_->updateQuery(query);
            RowID portID = dbConnection_->lastInsertRowID();
            portIDMap.insert(std::pair<FUPort*, RowID>(port, portID));
        }

        // insert into operation table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            if (!containsOperation(operation->name())) {
                dbConnection_->updateQuery(
                    std::string(
                        "INSERT INTO operation(id,name) VALUES(NULL,\"" + 
                        operation->name() + "\");"));
            }
        }

        // insert into io_binding table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            for (int i = 0; i < fu.operationPortCount(); i++) {
                FUPort* port = fu.operationPort(i);
                if (operation->isBound(*port)) {
                    int io = operation->io(*port);
                    string query(
                        "INSERT INTO io_binding(id,io_number,port,operation)"
                        " VALUES(NULL," + Conversion::toString(io) + "," +
                        Conversion::toString(
                            MapTools::valueForKey<RowID>(portIDMap, port)) +
                        ",(SELECT id FROM operation WHERE "
                        "lower(name)=\"" + operation->name() + "\"));");
                    dbConnection_->updateQuery(query);
                }
            }
        }

        std::map<HWOperation*, RowID> pLineIDMap;

        // insert into operation pipeline table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO operation_pipeline(id,fu_arch,operation) "
                    "VALUES(NULL," + Conversion::toString(archID) +
                    ",(SELECT id FROM operation WHERE lower(name)=\"" + 
                    operation->name() + "\"));"));
            pLineIDMap.insert(
                std::pair<HWOperation*, RowID>(
                    operation, dbConnection_->lastInsertRowID()));
        }

        // insert into io_usage_table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            ExecutionPipeline* pLine = operation->pipeline();
            for (int cycle = 0; cycle < pLine->latency(); cycle++) {
                ExecutionPipeline::OperandSet readOperands = 
                    pLine->readOperands(cycle);
                ExecutionPipeline::OperandSet writtenOperands =
                    pLine->writtenOperands(cycle);
                for (ExecutionPipeline::OperandSet::const_iterator iter = 
                         readOperands.begin();
                     iter != readOperands.end(); iter++) {
                    dbConnection_->updateQuery(
                        std::string(
                            "INSERT INTO io_usage(id,cycle,io_number,action,"
                            "pipeline) VALUES(NULL," + 
                            Conversion::toString(cycle) + "," + 
                            Conversion::toString(*iter) + "," + 
                            Conversion::toString(READ_ACTION) + "," + 
                            Conversion::toString(
                                MapTools::valueForKey<RowID>(
                                    pLineIDMap, operation)) + ");"));
                }
                for (ExecutionPipeline::OperandSet::const_iterator iter =
                         writtenOperands.begin();
                     iter != writtenOperands.end(); iter++) {
                    dbConnection_->updateQuery(
                        std::string(
                            "INSERT INTO io_usage(id,cycle,io_number,action,"
                            "pipeline) VALUES(NULL," + 
                            Conversion::toString(cycle) + "," + 
                            Conversion::toString(*iter) + "," + 
                            Conversion::toString(WRITE_ACTION) + "," + 
                            Conversion::toString(
                                MapTools::valueForKey<RowID>(
                                    pLineIDMap, operation)) + ");"));
                }
            }
        }

        // insert into pipeline_resource table
        std::map<PipelineElement*, RowID> pipelineElementMap;
        for (int i = 0; i < fu.pipelineElementCount(); i++) {
            PipelineElement* element = fu.pipelineElement(i);
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO pipeline_resource(id,fu_arch) VALUES "
                    "(NULL," + Conversion::toString(archID) + ");"));
            pipelineElementMap.insert(
                std::pair<PipelineElement*, RowID>(
                    element, dbConnection_->lastInsertRowID()));
        }

        // insert into pipeline_resource_usage table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            ExecutionPipeline* pLine = operation->pipeline();
            for (int i = 0; i < fu.pipelineElementCount(); i++) {
                PipelineElement* element = fu.pipelineElement(i);
                for (int cycle = 0; cycle < pLine->latency(); cycle++) {
                    if (pLine->isResourceUsed(element->name(), cycle)) {
                        string resID = Conversion::toString(
                            MapTools::valueForKey<RowID>(
                                pipelineElementMap, element));
                        string pLineID = Conversion::toString(
                            MapTools::valueForKey<RowID>(
                                pLineIDMap, operation));
                        dbConnection_->updateQuery(
                            std::string(
                                "INSERT INTO pipeline_resource_usage(id,"
                                "cycle,resource,pipeline) VALUES(NULL," +
                                Conversion::toString(cycle) + "," +
                                resID + "," + pLineID + ");"));
                    }
                }
            }
        }                

        dbConnection_->commit();
        
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
    
    return archID;
}

/**
 * Tells whether the FU architecture that has the given ID can be removed
 * from the database. It can be removed only if no FU entry uses it.
 *
 * @param archID ID of the FU architecture.
 * @return True if the architecture can be removed, otherwise false.
 */
bool
HDBManager::canRemoveFUArchitecture(RowID archID) const {

    // check whether the architecture is used by FU entries
    try {
        RelationalDBQueryResult* queryResult = dbConnection_->query(
            std::string(
                "SELECT id FROM fu WHERE architecture=" +
                Conversion::toString(archID) + ";"));
        if (queryResult->hasNext()) {
            // there is an FU entry using the architecture
            delete queryResult;
            return false;
        } else {
            delete queryResult;
            return true;
        }
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Removes the FU architecture that has the given ID. The architecture is
 * removed only it is not used by any FU entry. Otherwise an exception is
 * thrown.
 *
 * @param archID ID of the FU architecture.
 * @exception InvalidData If the architecture cannot be removed since it
 *                          is used by some FU entry.
 */
void
HDBManager::removeFUArchitecture(RowID archID) const {
    if (!canRemoveFUArchitecture(archID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    try {
        dbConnection_->beginTransaction();

        // remove from io_binding table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM io_binding "
                "WHERE port IN "
                "(SELECT id "
                "FROM fu_data_port "
                "WHERE fu_arch=" + Conversion::toString(archID) + ");"));
        
        // remove from fu_data_port table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_data_port "
                "WHERE fu_arch=" + Conversion::toString(archID) + ";"));

        // remove from io_usage_table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM io_usage "
                "WHERE pipeline IN "
                "(SELECT id FROM operation_pipeline "
                "WHERE fu_arch=" + Conversion::toString(archID) + ");"));
        
        // remove from pipeline_resource_usage table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM pipeline_resource_usage "
                "WHERE pipeline IN "
                "(SELECT id FROM operation_pipeline "
                "WHERE fu_arch=" + Conversion::toString(archID) + ");"));
        
        // remove from pipeline_resource_table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM pipeline_resource "
                "WHERE fu_arch=" + Conversion::toString(archID) + ";"));
        
        // remove from operation_pipeline_table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM operation_pipeline "
                "WHERE fu_arch=" + Conversion::toString(archID) + ";"));
        
        // remove from fu_architecture table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_architecture "
                "WHERE id=" + Conversion::toString(archID) + ";"));
        
        dbConnection_->commit();

    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Adds an empty FU entry to the database.
 *
 * @param entry The FU entry.
 * @return ID of the added FU entry.
 */
RowID
HDBManager::addFUEntry() const {
    try {
        dbConnection_->updateQuery(
            std::string("INSERT INTO fu(id) VALUES(NULL);"));
        return dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}    


/**
 * Removes the FU entry that has the given ID from the database.
 *
 * The entry is removed entirely, meaning that all also FU implementation 
 * and cost estimation data of that entry are removed.
 *
 * @param id The ID of the FU entry.
 */
void
HDBManager::removeFUEntry(RowID id) const {

    if (!hasFUEntry(id)) {
        return;
    }

    // remove implementation
    try {
        // get the ID of the implementation
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM fu_implementation "
                "WHERE fu=" + Conversion::toString(id) + ";"));
        if (result->hasNext()) {
            result->next();
            const DataObject& implIDData = result->data(0);
            int implID = implIDData.integerValue();
            removeFUImplementation(implID);
        }
        delete result;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove cost estimation data
    try {
        // get the IDs of cost estimation datas
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM cost_estimation_data "
                "WHERE fu_reference=" + Conversion::toString(id) + ";"));
        while (result->hasNext()) {
            result->next();
            const DataObject& costIDData = result->data(0);
            int dataID = costIDData.integerValue();
            removeCostEstimationData(dataID);
        }
        delete result;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove from fu table
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu "
                "WHERE id=" + Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Adds the implementation of the given FU entry to the database.
 *
 * In practice the implementation is added for the FU entry that has the
 * same ID as the given FUEntry instance. The given FUEntry instance must
 * also have an architecture similar to the architecture of the FU entry
 * in the database. This is required in port mapping.
 *
 * @param entry The FU entry containing the implementation to add.
 * @return ID of the implementation that was added.
 * @exception InvalidData If the given FUEntry instance is invalid or
 *                          if the FU entry does not have architecture in
 *                          the database of if the FU entry has an
 *                          implementation already.
 */
RowID
HDBManager::addFUImplementation(const FUEntry& entry) const {
    if (!entry.hasID() || !entry.hasImplementation() ||
        !entry.hasArchitecture() || !hasFUEntry(entry.id())) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    FUImplementation& impl = entry.implementation();
    FUArchitecture& arch = entry.architecture();
    FunctionUnit& fu = arch.architecture();

    FUEntry* existingEntry = fuByEntryID(entry.id());
    if (existingEntry->hasImplementation() ||
        !existingEntry->hasArchitecture()) {
        delete existingEntry;
        throw InvalidData(__FILE__, __LINE__, __func__);
    }
    delete existingEntry;
    existingEntry = NULL;

    RowID archID = fuArchitectureID(entry.id());
    RowID implID;

    try {
        dbConnection_->beginTransaction();

        // insert into fu_implementation table
        string module = impl.moduleName();
        string opcPort = impl.opcodePort();
        string clkPort = impl.clkPort();
        string rstPort = impl.rstPort();
        string glockPort = impl.glockPort();
        string glockReqPort = impl.glockReqPort();

        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO fu_implementation(id,name,opcode_port,"
                "clk_port,rst_port,glock_port,glock_req_port,fu) VALUES"
                "(NULL,\"" + module + "\",\"" + opcPort + "\",\"" +
                clkPort + "\",\"" + rstPort + "\",\"" + glockPort + "\",\""
                + glockReqPort + "\"," + Conversion::toString(entry.id())
                + ");"));
        implID = dbConnection_->lastInsertRowID();
        
        // insert into fu_port_map table
        for (int i = 0; i < impl.architecturePortCount(); i++) {
            FUPortImplementation& portImpl = impl.architecturePort(i);
            string name = portImpl.name();
            string widthFormula = portImpl.widthFormula();
            string loadPort = portImpl.loadPort();
            string guardPort = portImpl.guardPort();
            string archPortName = portImpl.architecturePort();
            if (!fu.hasOperationPort(archPortName)) {
                throw InvalidData(__FILE__, __LINE__, __func__);
            }
            FUPort* port = fu.operationPort(archPortName);
            bool portAdded = false;
            for (int j = 0; j < fu.operationCount(); j++) {
                HWOperation* operation = fu.operation(j);
                if (operation->isBound(*port)) {
                    int io = operation->io(*port);
                    // find the fu_data_port from DB
                    RelationalDBQueryResult* result = dbConnection_->query(
                        std::string(
                            "SELECT fu_data_port.id FROM fu_data_port,"
                            "io_binding,operation WHERE "
                            "fu_data_port.fu_arch=" +
                            Conversion::toString(archID) +
                            " AND io_binding.port=fu_data_port.id AND "
                            "io_binding.io_number=" +
                            Conversion::toString(io) +
                            " AND lower(operation.name)=\"" + 
                            operation->name() + 
                            "\" AND io_binding.operation=operation.id;"));
                    if (!result->hasNext()) {
                        delete result;
                        throw InvalidData(__FILE__, __LINE__, __func__);
                    }
                    result->next();
                    string portID = result->data(0).stringValue();
                    delete result;
                    
                    // update fu_port_map table
                    dbConnection_->updateQuery(
                        std::string(
                            "INSERT INTO fu_port_map(id,name,width_formula,"
                            "load_port,guard_port,fu_impl,arch_port) VALUES"
                            "(NULL,\"" + name + "\",\"" + widthFormula +
                            "\",\"" + loadPort + "\",\"" + guardPort +
                            "\"," + Conversion::toString(implID) + "," +
                            portID + ");"));
                    portAdded = true;
                    break;
                }
            }
            
            if (!portAdded) {
                throw InvalidData(__FILE__, __LINE__, __func__);
            }
        }
#ifdef ALLOW_OPCODE_EDITING
        // insert into opcode_map table
        for (int i = 0; i < fu.operationCount(); i++) {
            HWOperation* operation = fu.operation(i);
            if (!containsOperation(operation->name())) {
                format errorMsg(
                    "FU implementation uses unknown operation %1%."); 
                errorMsg % operation->name();
                throw InvalidData(
                    __FILE__, __LINE__, __func__, errorMsg.str());
            }
            if (fu.operationCount() > 1 && 
                !impl.hasOpcode(operation->name())) {
                format errorMsg("Opcode not defined for operation %1%.");
                errorMsg % operation->name();
                throw InvalidData(
                    __FILE__, __LINE__, __func__, errorMsg.str());
            }
            if (fu.operationCount() > 1) {
                int opcode = impl.opcode(operation->name());
                dbConnection_->updateQuery(
                    std::string(
                        "INSERT INTO opcode_map(id,opcode,operation,fu_impl)"
                        " VALUES(NULL," + Conversion::toString(opcode) +
                        ",(SELECT id FROM operation WHERE lower(name)=\"" +
                        operation->name() + "\")," +
                        Conversion::toString(implID) + ");"));
            }
        }
#endif
        // insert into fu_external_port table
        for (int i = 0; i < impl.externalPortCount(); i++) {
            FUExternalPort& port = impl.externalPort(i);
            string direction = directionString(port.direction());
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO fu_external_port(id,name,direction,"
                    "width_formula,description,fu_impl) VALUES(NULL,\"" +
                    port.name() + "\",\"" + direction + "\",\"" +
                    port.widthFormula() + "\",\"" + port.description() +
                    "\"," + Conversion::toString(implID) + ");"));
        }

        // insert into fu_implementation_parameter table
        for (int i = 0; i < impl.parameterCount(); i++) {
            FUImplementation::Parameter param = impl.parameter(i);
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO fu_implementation_parameter(id,name,type,"
                    "value,fu_impl) VALUES(NULL,\"" + param.name +
                    "\",\"" + param.type + "\",\"" + param.value +
                    "\"," + Conversion::toString(implID) + ");"));
        }

        // insert into fu_ext_port_parameter_dependency table
        for (int i = 0; i < impl.externalPortCount(); i++) {
            FUExternalPort& port = impl.externalPort(i);
            for (int i = 0; i < port.parameterDependencyCount(); i++) {
                string param = port.parameterDependency(i);
                dbConnection_->updateQuery(
                    std::string(
                        "INSERT INTO fu_ext_port_parameter_dependency(id,"
                        "port,parameter) VALUES(NULL,(SELECT id FROM "
                        "fu_external_port WHERE fu_impl=" +
                        Conversion::toString(implID) + " AND name=\"" +
                        port.name() + "\"),(SELECT id FROM "
                        "fu_implementation_parameter WHERE fu_impl=" +
                        Conversion::toString(implID) + " AND name=\"" +
                        param + "\"));"));
            }
        }

        // insert into block_source_file table
        for (int i = 0; i < impl.implementationFileCount(); i++) {
            BlockImplementationFile& file = impl.file(i);
            addBlockImplementationFileToHDB(file);
        }

        // insert into fu_source_file table
        for (int i = 0; i < impl.implementationFileCount(); i++) {
            BlockImplementationFile& file = impl.file(i);
            string path = file.pathToFile();
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO fu_source_file(id,fu_impl,file) "
                    "VALUES(NULL," + Conversion::toString(implID) +
                    ",(SELECT id FROM block_source_file WHERE file=\"" +
                    path + "\"));"));
        }

        dbConnection_->commit();

    } catch (const RelationalDBException& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    } catch (const InvalidData& e) {
        dbConnection_->rollback();
        throw;
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }

    return implID;
}

/**
 * Removes the given FU implementation from the database.
 *
 * @param implID ID of the FU implementation.
 */
void
HDBManager::removeFUImplementation(RowID implID) const {
    
    try {
        dbConnection_->beginTransaction();

        // remove from fu_ext_port_parameter_dependency table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_ext_port_parameter_dependency "
                "WHERE parameter in (SELECT ALL id FROM "
                "fu_implementation_parameter WHERE fu_impl="
                + Conversion::toString(implID) + ");"));

        // remove from fu_external_port table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_external_port "
                "WHERE fu_impl=" + Conversion::toString(implID) + ";"));
        
        // remove from fu_port_map table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_port_map "
                "WHERE fu_impl=" + Conversion::toString(implID) + ";"));
        
        // remove from opcode_map table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM opcode_map "
                "WHERE fu_impl=" + Conversion::toString(implID) + ";"));
                
        // remove from fu_implementation_parameter
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_implementation_parameter "
                "WHERE fu_impl=" + Conversion::toString(implID) + ";"));
        
        // remove from fu_source_file
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_source_file "
                "WHERE fu_impl=" + Conversion::toString(implID) + ";"));
        
        // remove from block_source_file
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM block_source_file "
                "WHERE id NOT IN "
                "(SELECT file FROM fu_source_file UNION "
                "SELECT file FROM rf_source_file);"));
        
        // remove from fu_implementation
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM fu_implementation "
                "WHERE id=" + Conversion::toString(implID) + ";"));
        
        dbConnection_->commit();
        
    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Sets the given architecture for the given FU entry.
 *
 * @param fuID ID of the FU entry.
 * @param archID ID of the FU architecture.
 * @exception InvalidData If the FU entry has an implementation already or
 *            if the HDB does not have FU or architecture by the given ID.
 */
void
HDBManager::setArchitectureForFU(RowID fuID, RowID archID) const {
    if (!hasFUEntry(fuID) || !containsFUArchitecture(archID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    FUEntry* entry = fuByEntryID(fuID);
    if (entry->hasImplementation()) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    // set the architecture
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE fu SET architecture=" +
                Conversion::toString(archID) + " WHERE id=" +
                Conversion::toString(fuID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Unsets the architecture of the given FU entry.
 *
 * @param fuID ID of the FU entry.
 * @exception InvalidData If the HDB does not contain the given FU entry
 *                          or if the FU entry has an implementation.
 */
void
HDBManager::unsetArchitectureForFU(RowID fuID) const {
    if (!hasFUEntry(fuID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    FUEntry* entry = fuByEntryID(fuID);
    if (entry->hasImplementation()) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    // unset the architecture
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE fu SET architecture=NULL WHERE id=" +
                Conversion::toString(fuID)));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Adds the given RF architecture to the HDB.
 *
 * @param architecture The architecture to add.
 * @return ID of the architecture added.
 */
RowID
HDBManager::addRFArchitecture(const RFArchitecture& architecture) const {

    try {
        string query = 
            "INSERT INTO rf_architecture(id,size,width,read_ports,"
            "write_ports,bidir_ports,latency,max_reads,max_writes,"
            "guard_support,guard_latency,zero_register) VALUES(NULL,";
        if (architecture.hasParameterizedSize()) {
            query += "NULL,";
        } else {
            query += (Conversion::toString(architecture.size()) + ",");
        }
        if (architecture.hasParameterizedWidth()) {
            query += "NULL,";
        } else {
            query += (Conversion::toString(architecture.width()) + ",");
        }
        query += (Conversion::toString(architecture.readPortCount()) + ",");
        query += (Conversion::toString(architecture.writePortCount()) + ",");
        query += (Conversion::toString(architecture.bidirPortCount()) + ",");
        query += (Conversion::toString(architecture.latency()) + ",");
        query += (Conversion::toString(architecture.maxReads()) + ",");
        query += (Conversion::toString(architecture.maxWrites()) + ",");
        query += 
            (Conversion::toString(architecture.hasGuardSupport()) + ",");
        query += (Conversion::toString(architecture.guardLatency()) + ",");
        query += (Conversion::toString(architecture.zeroRegister()));
        query += ");";
        
        dbConnection_->updateQuery(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    return dbConnection_->lastInsertRowID();
}


/**
 * Tells whether the given RF architecture can be removed.
 *
 * The architecture can be removed if it is not used by any RF entry.
 *
 * @param archID ID of the RF architecture.
 */
bool
HDBManager::canRemoveRFArchitecture(RowID archID) const {
    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM rf WHERE architecture=" + 
                Conversion::toString(archID) + ";"));
        bool returnValue = !result->hasNext();
        delete result;
        return returnValue;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Removes the RF architecture that has the given ID.
 *
 * @param archID ID of the RF architecture.
 * @exception InvalidData If the RF architecture cannot be removed.
 */
void
HDBManager::removeRFArchitecture(RowID archID) const {
    if (!canRemoveRFArchitecture(archID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM rf_architecture WHERE id=" +
                Conversion::toString(archID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Adds an empty RF entry to the database.
 *
 * @return ID of the entry added.
 */
RowID
HDBManager::addRFEntry() const {
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO rf(id,architecture,cost_function) "
                "VALUES(NULL,NULL,NULL);"));
        return dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}


/**
 * Removes the RF entry that has the given ID.
 *
 * Cost estimation data and implementation of the entry are removed too.
 *
 * @param id ID of the RF entry.
 */
void
HDBManager::removeRFEntry(RowID id) const {

    if (!hasRFEntry(id)) {
        return;
    }
    
    // remove from cost_estimation_data
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM cost_estimation_data WHERE rf_reference=" +
                Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove implementation
    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM rf_implementation WHERE rf=" +
                Conversion::toString(id) + ";"));
        if (result->hasNext()) {
            result->next();
            RowID implID = result->data(0).integerValue();
            removeRFImplementation(implID);
        }
        delete result;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove the entry
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM rf WHERE id=" + Conversion::toString(id)
                + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }       
}


/**
 * Adds an implementation for the the given RF entry.
 *
 * @param implementation The implementation to add.
 * @param rfEntryID ID of the RF entry.
 * @exception InvalidData If the RF entry has an implementation already or
 *                          if the database does not contain an RF entry
 *                          with the given ID.
 */
RowID
HDBManager::addRFImplementation(
    const RFImplementation& implementation, RowID rfEntryID) {
    if (!hasRFEntry(rfEntryID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }
    
    RFEntry* entry = rfByEntryID(rfEntryID);
    if (entry->hasImplementation()) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }
    delete entry;
    entry = NULL;
    
    if(!hasColumn("rf_implementation", "sac_param")) {
        addBooleanColumn("rf_implementation", "sac_param");
    }

    // Create tables for external ports, parameters and parameter dependecies
    // if needed.
    if (!dbConnection_->tableExistsInDB("rf_implementation_parameter")) {
        dbConnection_->DDLQuery(CQ_RF_IMPLEMENTATION_PARAMETER);
    }
    if (!dbConnection_->tableExistsInDB("rf_external_port")) {
        dbConnection_->DDLQuery(CQ_RF_EXTERNAL_PORT);
    }
    if (!dbConnection_->tableExistsInDB("rf_ext_port_parameter_dependency")) {
        dbConnection_->DDLQuery(CQ_RF_EXT_PORT_PARAMETER_DEPENDENCY);

    }

    try {
        dbConnection_->beginTransaction();

        int sacFlagAsInt = implementation.separateAddressCycleParameter();

        // insert into rf_implementation table
        std::string insert_query(
            "INSERT INTO rf_implementation(id,name,size_param,"
            "width_param,clk_port,rst_port,glock_port,guard_port,sac_param,rf) "
            "VALUES(NULL,\"" + implementation.moduleName() + "\",\"" +
            implementation.sizeParameter() + "\",\"" +
            implementation.widthParameter() + "\",\"" +
            implementation.clkPort() + "\",\"" +
            implementation.rstPort() + "\",\"" +
            implementation.glockPort() + "\",\"" +
            implementation.guardPort() + "\"," +
            Conversion::toString(sacFlagAsInt) +  "," +
            Conversion::toString(rfEntryID) + ");");

        dbConnection_->updateQuery(insert_query);
        RowID implID = dbConnection_->lastInsertRowID();

        // insert into rf_data_port table
        for (int i = 0; i < implementation.portCount(); i++) {
            RFPortImplementation& port = implementation.port(i);
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_data_port(id,name,direction,load_port,"
                    "opcode_port,opcode_port_width_formula,rf_impl) "
                    "VALUES(NULL,\"" + port.name() + "\",\"" +
                    directionString(port.direction()) + "\",\"" +
                    port.loadPort() + "\",\"" + port.opcodePort() + "\",\""
                    + port.opcodePortWidthFormula() + "\"," +
                    Conversion::toString(implID) + ");"));
        }

        // insert into block_source_file table
        for (int i = 0; i < implementation.implementationFileCount(); i++) {
            BlockImplementationFile& file = implementation.file(i);
            addBlockImplementationFileToHDB(file);
        }

        // insert into rf_source_file table
        for (int i = 0; i < implementation.implementationFileCount(); i++) {
            BlockImplementationFile& file = implementation.file(i);
            string path = file.pathToFile();
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_source_file values(NULL, " + 
                    Conversion::toString(implID) +
                    ", (SELECT id FROM block_source_file WHERE file=\"" +
                    path + "\"));"));
        }
        
        // insert into rf_external_port table
        for (int i = 0; i < implementation.externalPortCount(); i++) {
            RFExternalPort& port = implementation.externalPort(i);
            string direction = directionString(port.direction());
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_external_port(id,name,direction,"
                    "width_formula,description,rf_impl) VALUES(NULL,\"" +
                    port.name() + "\",\"" + direction + "\",\"" +
                    port.widthFormula() + "\",\"" + port.description() +
                    "\"," + Conversion::toString(implID) + ");"));
        }

        // insert into rf_implementation_parameter table
        for (int i = 0; i < implementation.parameterCount(); i++) {
            RFImplementation::Parameter param = implementation.parameter(i);
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_implementation_parameter(id,name,type,"
                    "value,rf_impl) VALUES(NULL,\"" + param.name +
                    "\",\"" + param.type + "\",\"" + param.value +
                    "\"," + Conversion::toString(implID) + ");"));
        }

        // Insert implicit parameters to rf_implementation_parameter table
        // (size and width parameter references if not empty and parameters
        // for them do not exists).
        string widthParam = implementation.widthParameter();
        if (!widthParam.empty() && !implementation.hasParameter(widthParam)) {
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_implementation_parameter(id,name,type,"
                    "value,rf_impl) VALUES(NULL,\"" + widthParam +
                    "\", \"integer\", \"\"," +
                    Conversion::toString(implID) + ");"));
        }
        string sizeParam = implementation.sizeParameter();
        if (!sizeParam.empty() && !implementation.hasParameter(sizeParam)) {
            dbConnection_->updateQuery(
                std::string(
                    "INSERT INTO rf_implementation_parameter(id,name,type,"
                    "value,rf_impl) VALUES(NULL,\"" + sizeParam +
                    "\", \"integer\", \"\"," +
                    Conversion::toString(implID) + ");"));
        }

        // insert into rf_ext_port_parameter_dependency table
        for (int i = 0; i < implementation.externalPortCount(); i++) {
            RFExternalPort& port = implementation.externalPort(i);
            for (int i = 0; i < port.parameterDependencyCount(); i++) {
                string param = port.parameterDependency(i);
                dbConnection_->updateQuery(
                    std::string(
                        "INSERT INTO rf_ext_port_parameter_dependency(id,"
                        "port,parameter) VALUES(NULL,(SELECT id FROM "
                        "rf_external_port WHERE rf_impl=" +
                        Conversion::toString(implID) + " AND name=\"" +
                        port.name() + "\"),(SELECT id FROM "
                        "rf_implementation_parameter WHERE rf_impl=" +
                        Conversion::toString(implID) + " AND name=\"" +
                        param + "\"));"));
            }
        }

        dbConnection_->commit();
        return implID;

    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}

/**
 * Removes the RF implementation that has the given ID.
 *
 * @param implID ID of the RF implementation.
 */
void
HDBManager::removeRFImplementation(RowID implID) const {

    bool dependencyTableExists =
        dbConnection_->tableExistsInDB("rf_ext_port_parameter_dependency");
    bool parameterTableExists =
        dbConnection_->tableExistsInDB("rf_implementation_parameter");
    bool externalPortTableExists =
        dbConnection_->tableExistsInDB("rf_external_port");

    try {
        dbConnection_->beginTransaction();
        
        // remove from rf_ext_port_parameter_dependency table if it exists
        // (backward compatibility for old HDBs).
        if (dependencyTableExists) {
            assert(parameterTableExists && externalPortTableExists);
            dbConnection_->updateQuery(
                std::string(
                    "DELETE FROM rf_ext_port_parameter_dependency "
                    "WHERE parameter in (SELECT ALL id "
                    "FROM rf_implementation_parameter WHERE rf_impl = " +
                    Conversion::toString(implID) + ");"));
        }

        // remove from rf_external_port table if it exists
        // (backward compatibility for old HDBs).
        if (externalPortTableExists) {
            dbConnection_->updateQuery(
                std::string(
                    "DELETE FROM rf_external_port "
                    "WHERE rf_impl=" +
                    Conversion::toString(implID) + ";"));
        }

        // remove from rf_implementation_parameter table if it exists
        // (backward compatibility for old HDBs).
        if (parameterTableExists) {
            dbConnection_->updateQuery(
                std::string(
                    "DELETE FROM rf_implementation_parameter "
                    "WHERE rf_impl=" + Conversion::toString(implID) + ";"));
        }

        // remove from rf_source_file table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM rf_source_file WHERE rf_impl=" +
                Conversion::toString(implID) + ";"));
        
        // remove from block_source_file table
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM block_source_file WHERE id NOT IN "
                "(SELECT file FROM fu_source_file UNION "
                "SELECT file FROM rf_source_file);"));
        
        // remove from rf_data_port
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM rf_data_port WHERE rf_impl=" +
                Conversion::toString(implID) + ";"));

        // remove from rf_implementation
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM rf_implementation WHERE id=" +
                Conversion::toString(implID) + ";"));

        dbConnection_->commit();

    } catch (const Exception& e) {
        dbConnection_->rollback();
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Sets architecture for an RF entry.
 *
 * @param rfID ID of the RF entry.
 * @param archID ID of the RF architecture to set.
 * @exception InvalidData If the database does not contain the given IDs or
 *                          if the RF entry has an architecture already.
 */
void
HDBManager::setArchitectureForRF(RowID rfID, RowID archID) const {
    if (!hasRFEntry(rfID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    RFEntry* entry = rfByEntryID(rfID);
    if (entry->hasArchitecture()) {
        delete entry;
        throw InvalidData(__FILE__, __LINE__, __func__);
    }
    delete entry;
    entry = NULL;

    if (!containsRFArchitecture(archID)) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE rf SET architecture=" +
                Conversion::toString(archID) + " WHERE id=" +
                Conversion::toString(rfID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Unsets architecture of the given RF entry.
 *
 * @param rfID ID of the RF entry.
 */
void
HDBManager::unsetArchitectureForRF(RowID rfID) const {
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE rf SET architecture=NULL WHERE id=" +
                Conversion::toString(rfID)));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Sets the given cost function plugin for the given FU entry.
 *
 * @param fuID ID of the FU entry.
 * @param pluginID ID of the cost function plugin.
 */
void
HDBManager::setCostFunctionPluginForFU(RowID fuID, RowID pluginID) const {

    // set the cost function plugin for fu
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE fu SET cost_function=" +
                Conversion::toString(pluginID) + " WHERE id=" +
                Conversion::toString(fuID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Unsets cost function plugin of the given FU entry.
 *
 * @param fuID ID of the FU entry.
 */
void
HDBManager::unsetCostFunctionPluginForFU(RowID fuID) const {

    // unset the cost function plugin
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE fu SET cost_function=NULL WHERE id=" +
                Conversion::toString(fuID)));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Sets the given cost function plugin for the given RF entry.
 *
 * @param rfID ID of the RF entry.
 * @param pluginID ID of the cost function plugin.
 */
void
HDBManager::setCostFunctionPluginForRF(RowID rfID, RowID pluginID) const {

    // set the cost function plugin for rf
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE rf SET cost_function=" +
                Conversion::toString(pluginID) + " WHERE id=" +
                Conversion::toString(rfID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Unsets cost function plugin of the given RF entry.
 *
 * @param rfID ID of the RF entry.
 */
void
HDBManager::unsetCostFunctionPluginForRF(RowID rfID) const {

    // unset the cost function plugin
    try {
        dbConnection_->updateQuery(
            std::string(
                "UPDATE rf SET cost_function=NULL WHERE id=" +
                Conversion::toString(rfID)));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Returns a set of FU entry IDs in the database.
 *
 * @return A set containing all the FU entry IDs in the database.
 */
std::set<RowID>
HDBManager::fuEntryIDs() const {

    string query = "SELECT id AS 'fu.id' FROM fu;";

    // make the SQL query to obtain all the IDs
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(query);
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    std::set<RowID> idSet;
    while (queryResult->hasNext()) {
        queryResult->next();
        const DataObject& idData = queryResult->data("fu.id");
        idSet.insert(idData.integerValue());
    }

    delete queryResult;
    return idSet;
}     


/**
 * Returns a set of RF entry IDs in the database.
 *
 * @return A set containing all the RF entry IDs in the database.
 */
std::set<RowID>
HDBManager::rfEntryIDs() const {

    string query = "SELECT id AS 'rf.id' FROM rf;";

    // make the SQL query to obtain all the IDs
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(query);
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    std::set<RowID> idSet;
    while (queryResult->hasNext()) {
        queryResult->next();
        const DataObject& idData = queryResult->data("rf.id");
        idSet.insert(idData.integerValue());
    }

    delete queryResult;
    return idSet;
}     


/**
 * Returns a set of Bus entry IDs in the database.
 *
 * @return A set containing all the Bus entry IDs in the database.
 */
std::set<RowID>
HDBManager::busEntryIDs() const {

    string query = "SELECT id AS 'bus.id' FROM bus;";

    // make the SQL query to obtain all the IDs
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(query);
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    std::set<RowID> idSet;
    while (queryResult->hasNext()) {
        queryResult->next();
        const DataObject& idData = queryResult->data("bus.id");
        idSet.insert(idData.integerValue());
    }

    delete queryResult;
    return idSet;
}   


/**
 * Returns a set of Socket entry IDs in the database.
 *
 * @return A set containing all the Socket entry IDs in the database.
 */
std::set<RowID>
HDBManager::socketEntryIDs() const {

    string query = "SELECT id AS 'socket.id' FROM socket;";

    // make the SQL query to obtain all the IDs
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(query);
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    std::set<RowID> idSet;
    while (queryResult->hasNext()) {
        queryResult->next();
        const DataObject& idData = queryResult->data("socket.id");
        idSet.insert(idData.integerValue());
    }

    delete queryResult;
    return idSet;
}   


/**
 * Returns a set of FU architecture IDs in the database.
 *
 * @return A set containing all the FU architecture IDs in the database.
 */
std::set<RowID>
HDBManager::fuArchitectureIDs() const {

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            std::string("SELECT id FROM fu_architecture;"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    std::set<RowID> idSet;
    while (result->hasNext()) {
        result->next();
        const DataObject& idData = result->data(0);
        idSet.insert(idData.integerValue());
    }

    delete result;
    return idSet;
}


/**
 * Returns a set of Operation Implementation Resource IDs in the database.
 *
 * @return A set containing all the Operation Implementation Resource
 * IDs in the database.
 */
std::set<RowID>
HDBManager::OperationImplementationIDs() const {
    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            std::string("SELECT id FROM operation_implementation;"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    std::set<RowID> idSet;
    while (result->hasNext()) {
        result->next();
        const DataObject& idData = result->data(0);
        idSet.insert(idData.integerValue());
    }

    delete result;
    return idSet;
}

/**
 * Returns a set of Operation Implementation IDs in the database.
 *
 * @return A set containing all the Operation Implemnetation
 * IDs in the database.
 */
std::set<RowID>
HDBManager::OperationImplementationResourceIDs() const {
    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            std::string("SELECT id FROM operation_implementation_resource;"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    std::set<RowID> idSet;
    while (result->hasNext()) {
        result->next();
        const DataObject& idData = result->data(0);
        idSet.insert(idData.integerValue());
    }

    delete result;
    return idSet;
}

/**
 * Returns OperationImplementation.
 *
 * @return OperationImplementation.
 */
HDB::OperationImplementation
HDBManager::OperationImplementationByID(RowID id) const {
    RelationalDBQueryResult* result = nullptr;
    try {
        result = dbConnection_->query(
            std::string("SELECT * FROM operation_implementation WHERE id = ") +
            std::to_string(id) +
            std::string(";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    OperationImplementation retval;
    if (result->hasNext()) {
        result->next();

        retval.id = id;
        retval.latency = result->data("latency").integerValue();
        retval.name = result->data("name").stringValue();
        retval.postOpImplFileVhdl =
            result->data("post_op_vhdl").stringValue();
        retval.postOpImplFileVerilog =
            result->data("post_op_verilog").stringValue();
        retval.absBusDefFile =
            result->data("bus_definition").stringValue();
        retval.initialImplFileVerilog =
            result->data("initial_verilog").stringValue();
        retval.initialImplFileVhdl =
            result->data("initial_vhdl").stringValue();
    }
    delete result;
    result = nullptr;

    try {
        result = dbConnection_->query(
            std::string("SELECT block_source_file.file "
                        "FROM operation_implementation_source_file, "
                        "block_source_file "
                        "WHERE operation_implementation_source_"
                            "file.operation = ") +
            std::to_string(id) +
            std::string(" AND operation_implementation_source_file.file = "
                        " block_source_file.id"
                        " AND block_source_file.format = "
                        + std::to_string(
                            BlockImplementationFile::VHDL) +
                        ";"));

    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    if (result->hasNext()) {
        result->next();
        retval.implFileVhdl = result->data("file").stringValue();
    }
    delete result;
    result = nullptr;

    try {
        result = dbConnection_->query(
            std::string("SELECT block_source_file.file "
                        "FROM operation_implementation_source_file, "
                        "block_source_file "
                        "WHERE operation_implementation_"
                            "source_file.operation = ") +
            std::to_string(id) +
            std::string(" AND operation_implementation_source_file.file = "
                        " block_source_file.id"
                        " AND block_source_file.format = "
                        + std::to_string(
                            BlockImplementationFile::Verilog) +
                        ";"));

    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    if (result->hasNext()) {
        result->next();
        retval.implFileVerilog = result->data("file").stringValue();
    }
    delete result;
    result = nullptr;

    std::string q1 = "SELECT resource, count "
        "FROM operation_implementation_resources "
        "WHERE operation_implementation_resources.operation = "
        + std::to_string(id) + ";";
    result = dbConnection_->query(q1);
    while (result->hasNext()) {
        result->next();
        int resource = result->data("resource").integerValue();
        OperationImplementationResource r =
            OperationImplementationResourceByID(resource);
        r.count = result->data("count").integerValue();
        retval.resources.emplace_back(r);
    }

    std::string q2 = "SELECT name, width, type, language "
        "FROM operation_implementation_variable "
        "WHERE CAST(operation as TEXT) = \"" + std::to_string(id) + "\";";
    result = dbConnection_->query(q2);
    while (result->hasNext()) {
        result->next();
        std::string name = result->data("name").stringValue();
        std::string width = result->data("width").stringValue();
        std::string type = result->data("type").stringValue();
        std::string lang = result->data("language").stringValue();
        // TODO not all HDBs have a rename column yet, so variable renaming is
        // hardcoded
        bool rename = true;
        // std::string renamestr = result->data("rename").stringValue();
        // bool rename = renamestr != "0";
        if (lang == "VHDL") {
            Variable var = {name, width, type, rename};
            retval.vhdlVariables.emplace_back(var);
        } else if (lang == "Verilog") {
            retval.verilogVariables.emplace_back(
                Variable{name, width, type, rename});
        } else {
            throw std::runtime_error("Unknown language");
        }
    }

    if (dbConnection_->tableExistsInDB(
            "operation_implementation_globalsignal")) {
        std::string q3 =
            "SELECT name, width, type, language, rename "
            "FROM operation_implementation_globalsignal "
            "WHERE CAST(operation as TEXT) = \"" +
            std::to_string(id) + "\";";
        result = dbConnection_->query(q3);
        while (result->hasNext()) {
            result->next();
            std::string name = result->data("name").stringValue();
            std::string width = result->data("width").stringValue();
            std::string type = result->data("type").stringValue();
            std::string lang = result->data("language").stringValue();
            std::string renamestr = result->data("rename").stringValue();
            bool rename = renamestr != "0";
            if (lang == "VHDL") {
                Variable var = {name, width, type, rename};
                retval.vhdlGlobalSignals.emplace_back(var);
            } else if (lang == "Verilog") {
                retval.verilogGlobalSignals.emplace_back(
                    Variable{name, width, type, rename});
            } else {
                throw std::runtime_error("Unknown language");
            }
        }
    }

    return retval;
}

/**
 * Returns OperationImplementationResource.
 *
 * @return OperationImplementationResource.
 */
HDB::OperationImplementationResource
HDBManager::OperationImplementationResourceByID(RowID id) const {
    RelationalDBQueryResult* result = nullptr;
    try {
        result = dbConnection_->query(
            std::string("SELECT * FROM "
                "operation_implementation_resource WHERE id = ") +
            std::to_string(id) +
            std::string(";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    OperationImplementationResource retval;
    if (result->hasNext()) {
        result->next();

        retval.id = id;
        retval.name = result->data("name").stringValue();
        retval.ipxact = result->data("ipxact").stringValue();
    }
    delete result;
    result = nullptr;

    try {
        result = dbConnection_->query(
            std::string("SELECT block_source_file.file, "
                        "format.format "
                        "FROM operation_implementation_resource_"
                        "source_file, block_source_file, "
                        "format "
                        "WHERE operation_implementation_resource_"
                            "source_file.resource = ") +
            std::to_string(id) +
            std::string(" AND operation_implementation_resource_"
                        "source_file.file = "
                        " block_source_file.id "
                        "AND block_source_file.format = "
                        "format.id;"
                        ));

    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    while (result->hasNext()) {
        result->next();
        std::string format = result->data("format").stringValue();
        if (format == VHDL_FORMAT) {
            retval.synFiles.push_back(result->data("file").stringValue());
            retval.synFormats.push_back(format);
        } else if (format == VHDL_SIM_FORMAT) {
            retval.simFiles.push_back(result->data("file").stringValue());
            retval.simFormats.push_back(format);
        } else if (format == VERILOG_FORMAT) {
            retval.synFiles.push_back(result->data("file").stringValue());
            retval.synFormats.push_back(format);
        } else if (format == VERILOG_SIM_FORMAT) {
            retval.simFiles.push_back(result->data("file").stringValue());
            retval.simFormats.push_back(format);
        }
    }
    delete result;
    result = nullptr;

    return retval;
}

/**
 * Add addOperationImplementationResource to the DB.
 */
void
HDBManager::addOperationImplementationResource(
    const OperationImplementationResource& resource) {

    dbConnection_->updateQuery(
        std::string(
            "INSERT INTO operation_implementation_resource(id,name,ipxact) "
            "VALUES (NULL, \"" +
            resource.name +"\", \"" + resource.ipxact+ "\");"));
    RowID resourceID = dbConnection_->lastInsertRowID();

    auto t = resource.simFormats.begin();
    auto f = resource.simFiles.begin();
    for (; f != resource.simFiles.end(); ++f, ++t) {
        int type = fileFormat(*t) + 1;

        dbConnection_->updateQuery(
        std::string(
            "INSERT INTO block_source_file(id,file,format) "
            "VALUES (NULL, \"" + *f + "\","
            + std::to_string(type) +
            ");"));
        RowID fileID = dbConnection_->lastInsertRowID();

        dbConnection_->updateQuery(
        std::string(
            "INSERT INTO operation_implementation_resource_source_file"
            "(id,resource,file) "
            "VALUES (NULL, " + std::to_string(resourceID) + ", "
            + std::to_string(fileID) +
            ");"));
    }

    auto st = resource.synFormats.begin();
    auto sf = resource.synFiles.begin();
    for (; sf != resource.synFiles.end(); ++sf, ++st) {
        int type = fileFormat(*st) + 1;

        dbConnection_->updateQuery(
        std::string(
            "INSERT INTO block_source_file(id,file,format) "
            "VALUES (NULL, \"" + *sf + "\","
            + std::to_string(type) +
            ");"));
        RowID fileID = dbConnection_->lastInsertRowID();

        dbConnection_->updateQuery(
        std::string(
            "INSERT INTO operation_implementation_resource_source_file"
            "(id,resource,file) "
            "VALUES (NULL, " + std::to_string(resourceID) + ", "
            + std::to_string(fileID) +
            ");"));
    }
}

/**
 * Add addOperationImplementation to the DB.
 */
void
HDBManager::addOperationImplementation(
        const OperationImplementation& operation) {


    std::string i1 = "INSERT INTO operation_implementation(id,name,latency,"
        "post_op_vhdl,post_op_verilog,bus_definition,"
        "initial_vhdl,initial_verilog) "
        "VALUES (NULL,\"" + operation.name + "\","
             + std::to_string(operation.latency) + ","
        "\"" + operation.postOpImplFileVhdl + "\","
        "\"" + operation.postOpImplFileVerilog + "\","
        "\"" + operation.absBusDefFile + "\","
        "\"" + operation.initialImplFileVhdl + "\","
        "\"" + operation.initialImplFileVerilog +
        "\");";
    dbConnection_->updateQuery(i1);
    RowID newid = dbConnection_->lastInsertRowID();

    for (const auto r : operation.resources) {
        std::string i2 = "INSERT INTO operation_implementation_resources("
            "id, operation, resource, count) "
            "VALUES (NULL, " + std::to_string(newid)
            + ", " + std::to_string(r.id)
            + ", " + std::to_string(r.count)
            + ");";
        dbConnection_->updateQuery(i2);
    }

    for (const auto r : operation.vhdlVariables) {
        std::string i2 = "INSERT INTO operation_implementation_variable("
            "id, operation, name, width, type, language) "
            "VALUES (NULL, " + std::to_string(newid)
            + ", \"" + r.name + "\""
            + ", \"" + r.width + "\""
            + ", \"" + r.type + "\""
            + ", \"VHDL\");";
        dbConnection_->updateQuery(i2);
    }

    for (const auto r : operation.verilogVariables) {
        std::string i2 = "INSERT INTO operation_implementation_variable("
            "id, operation, name, width, type, language) "
            "VALUES (NULL, " + std::to_string(newid)
            + ", \"" + r.name + "\""
            + ", \"" + r.width + "\""
            + ", \"" + r.type + "\""
            + ", \"Verilog\");";
        dbConnection_->updateQuery(i2);
    }

    std::string i3 = "INSERT INTO block_source_file(id,file,format) "
        "VALUES (NULL,\"" + operation.implFileVhdl +
        "\", " + std::to_string(BlockImplementationFile::VHDL) + ");";
    dbConnection_->updateQuery(i3);
    RowID vhdl = dbConnection_->lastInsertRowID();

    std::string i4 = "INSERT INTO "
        "operation_implementation_source_file(id, operation, file) "
        "VALUES (NULL, " + std::to_string(newid) +
        ", " + std::to_string(vhdl) +
        ");";
    dbConnection_->updateQuery(i4);

    std::string i5 = "INSERT INTO block_source_file(id,file,format) "
        "VALUES (NULL,\"" + operation.implFileVerilog +
        "\", " + std::to_string(BlockImplementationFile::Verilog) + ");";
    dbConnection_->updateQuery(i5);
    RowID verilog = dbConnection_->lastInsertRowID();

    std::string i6 = "INSERT INTO "
        "operation_implementation_source_file(id, operation, file) "
        "VALUES (NULL, " + std::to_string(newid) +
        ", " + std::to_string(verilog) +
        ");";
    dbConnection_->updateQuery(i6);
}

/**
 * Remove Operation Implemententation from DB.
 */
void
HDBManager::removeOperationImplementation(RowID id) {
    std::string d1 = "DELETE FROM operation_implementation "
        "WHERE id = " + std::to_string(id) + ";";
    std::string d2 =
        "DELETE FROM operation_implementation_source_file "
        "WHERE operation = " + std::to_string(id) + ";";
    std::string d3 =
        "DELETE FROM operation_implementation_resources "
        "WHERE operation = " + std::to_string(id) + ";";
    std::string d4 =
        "DELETE FROM operation_implementation_variable "
        "WHERE operation = " + std::to_string(id) + ";";
    std::string s1 = "SELECT file FROM "
        "operation_implementation_source_file "
        "WHERE operation = " + std::to_string(id) + ";";

    RelationalDBQueryResult* result = dbConnection_->query(s1);
    while (result->hasNext()) {
        result->next();
        int file_id = result->data(0).integerValue();
        std::string d3lete = "DELETE FROM block_source_file "
            "WHERE id = " + std::to_string(file_id) + ";";
        dbConnection_->updateQuery(d3lete);
    }

    dbConnection_->updateQuery(d4);
    dbConnection_->updateQuery(d3);
    dbConnection_->updateQuery(d2);
    dbConnection_->updateQuery(d1);
    delete result;
}

/**
 * Remove Operation Implemententation Resource from DB.
 */
void
HDBManager::removeOperationImplementationResource(RowID id) {
    std::string d1 = "DELETE FROM operation_implementation_resource "
        "WHERE id = " + std::to_string(id) + ";";
    std::string d2 =
        "DELETE FROM operation_implementation_resource_source_file "
        "WHERE resource = " + std::to_string(id) + ";";
    std::string s1 = "SELECT file FROM "
        "operation_implementation_resource_source_file "
        "WHERE resource = " + std::to_string(id) + ";";

    RelationalDBQueryResult* result = dbConnection_->query(s1);
    while (result->hasNext()) {
        result->next();
        int file_id = result->data(0).integerValue();
        std::string d3 = "DELETE FROM block_source_file "
            "WHERE id = " + std::to_string(file_id) + ";";
            dbConnection_->updateQuery(d3);
    }

    dbConnection_->updateQuery(d2);
    dbConnection_->updateQuery(d1);
    delete result;
}


std::set<RowID>
HDBManager::fuArchitectureIDsByOperationSet(
    const std::set<std::string>& operationNames) const {

    RelationalDBQueryResult* result = NULL;
    try {
        std::string operationQuery = "(";
        std::set<string>::const_iterator iter = operationNames.begin();
        while (iter != operationNames.end()) {
            // LIKE makes case-insensitive match to operation names
            operationQuery +=
                "operation.name LIKE '" + *iter + "'";
            iter++;
            if (iter != operationNames.end()) {
                operationQuery += " OR ";
            }
        }
        operationQuery += ")";
        if (operationQuery == "()") {
            return std::set<RowID>();
        }
        result = dbConnection_->query(
            std::string("SELECT fu_architecture.id FROM operation_pipeline,"
                        "operation, fu_architecture WHERE "
                        "operation.id=operation_pipeline.operation AND "
                        "operation_pipeline.fu_arch=fu_architecture.id AND "
                        + operationQuery +
                        "GROUP BY fu_architecture.id ORDER BY "
                        "fu_architecture.id;"));
    } catch (const Exception& e) {
        std::string eMsg = ", HDB file where error occurred was: " + hdbFile_;
        debugLog(e.errorMessage() + eMsg);
        assert(false);
    }

    std::set<RowID> idSet;
    while (result->hasNext()) {
        result->next();
        const DataObject& idData = result->data(0);
        idSet.insert(idData.integerValue());
    }

    delete result;
    return idSet;
}
/**
 * Returns a set of RF architecture IDs in the database.
 *
 * @return A set containing all the RF architecture IDs in the database.
 */
std::set<RowID>
HDBManager::rfArchitectureIDs() const {

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            std::string("SELECT id FROM rf_architecture;"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    std::set<RowID> idSet;
    while (result->hasNext()) {
        result->next();
        const DataObject& idData = result->data(0);
        idSet.insert(idData.integerValue());
    }

    delete result;
    return idSet;
}


/**
 * Returns the ID of the FU entry that has the given implementation ID.
 *
 * @param implID The implementation ID.
 * @return The FU entry ID.
 * @exception KeyNotFound If there is no implementation by the given ID.
 */
RowID
HDBManager::fuEntryIDOfImplementation(RowID implID) const {
    RelationalDBQueryResult* result = NULL;
    try {
        result =  dbConnection_->query(
            std::string(
                "SELECT fu from fu_implementation WHERE id=" + 
                Conversion::toString(implID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        RowID id = result->data(0).integerValue();
        delete result;
        return id;
    } else {
        delete result;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the ID of the RF entry that has the given implementation ID.
 *
 * @param implID The implementation ID.
 * @return The RF entry ID.
 * @exception KeyNotFound If there is no implementation by the given ID.
 */
RowID
HDBManager::rfEntryIDOfImplementation(RowID implID) const {
    RelationalDBQueryResult* result = NULL;
    try {
        result =  dbConnection_->query(
            std::string(
                "SELECT rf from rf_implementation WHERE id=" + 
                Conversion::toString(implID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        RowID id = result->data(0).integerValue();
        delete result;
        return id;
    } else {
        delete result;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the FU entry that has the given ID.
 *
 * @param id The ID of the FU entry.
 * @return The FU entry.
 * @excpetion KeyNotFound If the HDB does not contain an FU entry with the
 *                        given ID.
 */
FUEntry*
HDBManager::fuByEntryID(RowID id) const {
    std::string query = "SELECT architecture FROM fu WHERE id=";
    query += Conversion::toString(id) + ";";

    RelationalDBQueryResult* result = NULL;
    try {
        result =  dbConnection_->query(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    bool hasArch = false;
    RowID archID = -1;
    if (result->hasNext()) {        
        result->next();
        DataObject data = result->data(0);
        if (!data.isNull()) {
            hasArch = true;
            archID = data.integerValue();
        }
        delete result;
        result = NULL;
    } else {
        delete result;
        std::ostringstream stream;
        stream << "FU entry with id " << id << " not found from hdb "
               << hdbFile_;
        throw KeyNotFound(__FILE__, __LINE__, __func__, stream.str());
    }

    FUEntry* entry = new FUEntry();
    entry->setID(id);
    if (hasArch) {
        FUArchitecture* architecture = fuArchitectureByID(archID);
        entry->setArchitecture(architecture);
        FUImplementation* implementation = createImplementationOfFU(
            *architecture, id);
        entry->setImplementation(implementation);
    }

    CostFunctionPlugin* costFunction = createCostFunctionOfFU(id);
    entry->setCostFunction(costFunction);
    entry->setHDBFile(hdbFile_);
    delete result;
    return entry;
}

/**
 * Returns the RF entry that has the given ID.
 *
 * @param id The ID of the RF entry.
 * @return The RF entry.
 * @exception KeyNotFound If the HDB does not contain an RF entry with the
 *                        given ID.
 */
RFEntry*
HDBManager::rfByEntryID(RowID id) const {
    std::string query = "SELECT architecture FROM rf WHERE id=";
    query += Conversion::toString(id) + ";";

    RelationalDBQueryResult* result = NULL;
    try {
        result =  dbConnection_->query(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());        
        assert(false);
    }

    bool hasArch = false;
    RowID archID = -1;
    if (result->hasNext()) {        
        result->next();
        DataObject data = result->data(0);
        if (!data.isNull()) {
            hasArch = true;
            archID = data.integerValue();
        }
        delete result;
        result = NULL;
    } else {
        delete result;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    RFEntry* entry = new RFEntry();
    entry->setID(id);
    if (hasArch) {
        RFArchitecture* architecture = rfArchitectureByID(archID);
        entry->setArchitecture(architecture);
    }

    RFImplementation* implementation = createImplementationOfRF(id);
    CostFunctionPlugin* costFunction = createCostFunctionOfRF(id);
    entry->setImplementation(implementation);
    entry->setCostFunction(costFunction);
    entry->setHDBFile(hdbFile_);
    delete result;

    return entry;
}

/**
 * Creates an FUArchitecture instance of the FU architecture that has the
 * given ID.
 *
 * @param ID The ID of the FU architecture.
 * @return The newly created FUArchitecture instance.
 * @exception KeyNotFound If the HDB does not have a FU architecture with the 
 *                        given ID.
 */
FUArchitecture*
HDBManager::fuArchitectureByID(RowID id) const {
    if (!containsFUArchitecture(id)) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    FunctionUnit* fu = new FunctionUnit("name");
    FUArchitecture* architecture = new FUArchitecture(fu);
    architecture->setID(id);
    addPortsAndBindingsToFUArchitecture(*architecture, id);
    addOperationPipelinesToFUArchitecture(*architecture, id);
    return architecture;
}

/**
 * Creates an RFArchitecture instance of the RF architecture that has the
 * given ID.
 *
 * @param id The ID of the RF architecture.
 * @return The newly created RFArchitecture instance.
 * @exception KeyNotFound If the HDB does not contain RF architecture with the
 *                        given ID.
 */
RFArchitecture*
HDBManager::rfArchitectureByID(RowID id) const {
    if (!containsRFArchitecture(id)) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    RelationalDBQueryResult* architectureData;
    try {
        architectureData = dbConnection_->query(rfArchitectureByIDQuery(id));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    int sizeColumn = architectureData->column("size");
    int widthColumn = architectureData->column("width");
    int readPortsColumn = architectureData->column("read_ports");
    int writePortsColumn = architectureData->column("write_ports");
    int bidirPortsColumn = architectureData->column("bidir_ports");
    int latencyColumn = architectureData->column("latency");
    int maxReadsColumn = architectureData->column("max_reads");
    int maxWritesColumn = architectureData->column("max_writes");
    int guardSupportColumn = architectureData->column("guard_support");
    int guardLatencyColumn = architectureData->column("guard_latency");
    int zeroRegisterColumn = architectureData->column("zero_register");

    assert(architectureData->hasNext());
    architectureData->next();
    assert(!architectureData->hasNext());
    const DataObject& sizeData = architectureData->data(sizeColumn);
    const DataObject& widthData = architectureData->data(widthColumn);
    const DataObject& readPortsData = architectureData->data(
        readPortsColumn);
    const DataObject& writePortsData = architectureData->data(
        writePortsColumn);
    const DataObject& bidirPortsData = architectureData->data(
        bidirPortsColumn);
    const DataObject& latencyData = architectureData->data(
        latencyColumn);
    const DataObject& maxReadsData = architectureData->data(
        maxReadsColumn);
    const DataObject& maxWritesData = architectureData->data(
        maxWritesColumn);
    const DataObject& guardSupportData = architectureData->data(
        guardSupportColumn);
    const DataObject& guardLatencyData = architectureData->data(
        guardLatencyColumn);
    const DataObject& zeroRegisterData = architectureData->data(
        zeroRegisterColumn);
    RFArchitecture* architecture = new RFArchitecture(
        readPortsData.integerValue(), writePortsData.integerValue(),
        bidirPortsData.integerValue(), maxReadsData.integerValue(),
        maxWritesData.integerValue(), latencyData.integerValue(), 
        guardSupportData.boolValue(), guardLatencyData.integerValue(),
        zeroRegisterData.boolValue());
    std::cout.flush();
    architecture->setID(id);
    if (!sizeData.isNull()) {
        architecture->setSize(sizeData.integerValue());
    }
    if (!widthData.isNull()) {
        architecture->setWidth(widthData.integerValue());
    }
    delete architectureData;
    return architecture;
}

/**
 * Returns a set of FU entry IDs that have a corresponding architecture
 * with the given one.
 *
 * The set may contain FU entry IDs that have ports with parametrized
 * width while the given one has fixed width.
 *
 * @param fu The FU architecture.
 * @return Set of FU entry IDs.
 */
std::set<RowID>
HDBManager::fuEntriesByArchitecture(
    const TTAMachine::FunctionUnit& fu) const {

    std::set<RowID> architectureIDs;
    std::set<RowID> entryIDs;

    try {
        // get FU architectures with required operation set
        string query = "";
        for (int i = 0; i < fu.operationCount(); i++) {
            query += 
                "SELECT fu_arch FROM operation_pipeline,operation "
                "WHERE operation.name=\"" + fu.operation(i)->name() +
                "\" AND operation_pipeline.operation=operation.id";
            if (i+1 < fu.operationCount()) {
                query += " INTERSECT ";
            } else {
                query += ";";
            }
        }
            
        RelationalDBQueryResult* queryResult = dbConnection_->query(query);

        // check the architectures are compeletely similar
        while (queryResult->hasNext()) {
            queryResult->next();
            const DataObject& archID = queryResult->data(0);
            FUArchitecture* arch = fuArchitectureByID(archID.integerValue());
            if (isMatchingArchitecture(fu, *arch)) {
                architectureIDs.insert(arch->id());
            }
            delete arch;
        }

        delete queryResult;

        if (!architectureIDs.empty()) {
            // find the FU entry IDs
            string fuEntryQuery = "SELECT id FROM fu WHERE ";
            for (std::set<RowID>::const_iterator iter = 
                     architectureIDs.begin();
                 iter != architectureIDs.end(); iter++) {
                fuEntryQuery += "architecture=" + 
                    Conversion::toString(*iter);
                std::set<RowID>::const_iterator nextIter = iter;
                nextIter++;
                if (nextIter == architectureIDs.end()) {
                    fuEntryQuery += ";";
                } else {
                    fuEntryQuery += " OR ";
                }
            }

            RelationalDBQueryResult* fuEntryResult = dbConnection_->query(
                fuEntryQuery);
            while (fuEntryResult->hasNext()) {
                fuEntryResult->next();
                const DataObject& idData = fuEntryResult->data(0);
                entryIDs.insert(idData.integerValue());
            }

            delete fuEntryResult;
        }

    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        ;
    }

    return entryIDs;
}


/**
 * Returns a set of RF entry IDs that have the described architecture.
 *
 * In case that size or width is given as a parameter are also the RF entries
 * with a parameterized width or size returned as are the entries with matched
 * width and/or size.
 * 
 * @param readPorts The number of read ports.
 * @param writePorts The number of write ports.
 * @param bidirPorts The number of bidirectional ports.
 * @param maxRead The (minimum) max reads value.
 * @param latency The exact latency.
 * @param guardSupport Guard support.
 * @param guardLatency The guard latency.
 * @param width The bit withd of the register file.
 * @param size The number of registers in the register file.
 * @param zeroRegister zero register of the register file
 * @return Set of RF entry IDs.
 */
std::set<RowID>
HDBManager::rfEntriesByArchitecture(
    int readPorts,
    int writePorts,
    int bidirPorts,
    int maxReads,
    int maxWrites,
    int latency,
    bool guardSupport,
    int guardLatency,
    int width,
    int size,
    bool zeroRegister) const {

    RelationalDBQueryResult* result = NULL;
    try {
        string query = "SELECT rf.id FROM rf,rf_architecture "
            "WHERE rf_architecture.read_ports=" +
            Conversion::toString(readPorts) +
            " AND rf_architecture.write_ports=" +
            Conversion::toString(writePorts) +
            " AND rf_architecture.bidir_ports=" +
            Conversion::toString(bidirPorts) +
            " AND rf_architecture.max_reads>=" +
            Conversion::toString(maxReads) +
            " AND rf_architecture.max_writes>=" +
            Conversion::toString(maxWrites) +
            " AND rf_architecture.latency=" +
            Conversion::toString(latency);
        if (guardSupport) {
            query += " AND rf_architecture.guard_support=" + 
                Conversion::toString(guardSupport) + 
                " AND rf_architecture.guard_latency=" + 
                Conversion::toString(guardLatency);
        }
        if (size != 0) {
            query += " AND (rf_architecture.size=" +
                Conversion::toString(size) + 
                " OR rf_architecture.size is NULL)";
        }
        if (width != 0) {
            query += " AND (rf_architecture.width=" + 
                Conversion::toString(width) +
                " OR rf_architecture.width is NULL)";
        }
        query += " AND (rf_architecture.zero_register=" +
            Conversion::toString(zeroRegister);
        if (!zeroRegister) {
            query += " OR rf_architecture.zero_register is NULL";
        }
        query += ")";
        query += " AND rf.architecture=rf_architecture.id;";
        result = dbConnection_->query(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        ;
    }

    std::set<RowID> entryIDs;
    while (result->hasNext()) {
        result->next();
        entryIDs.insert(result->data(0).integerValue());
    }
    delete result;

    return entryIDs;
}


/**
 * Adds the given cost estimation data values to given FU entry.
 *
 * @param fuID The ID of the FU entry the cost data will be added.
 * @valueName The name of the cost value.
 * @value The cost value.
 * @pluginID The ID of the cost function plugin that owns this data.
 */
RowID
HDBManager::addFUCostEstimationData(
    RowID fuID,
    const std::string& valueName,
    const std::string& value,
    RowID pluginID) const {


    RowID dataID;

    // add the data
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO cost_estimation_data (id,plugin_reference,"
                "fu_reference,name,value) VALUES (NULL," +
                Conversion::toString(pluginID) + "," +
                Conversion::toString(fuID) + ",\"" + valueName + "\",\"" +
                value + "\");"));
        dataID = dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    return dataID;
}


/**
 * Adds the given cost estimation data values to given RF entry.
 *
 * @param rfID The ID of the RF entry the cost data will be added.
 * @valueName The name of the cost value.
 * @value The cost value.
 * @pluginID The ID of the cost function plugin that owns this data.
 */
RowID
HDBManager::addRFCostEstimationData(
    RowID rfID,
    const std::string& valueName,
    const std::string& value,
    RowID pluginID) const {

    RowID dataID;
    
    // add the data
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO cost_estimation_data (id,plugin_reference,"
                "rf_reference,name,value) VALUES (NULL," + 
                Conversion::toString(pluginID) + "," + 
                Conversion::toString(rfID) + ",\"" + valueName + "\",\"" +
                value + "\");"));
        dataID = dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    return dataID;
}


/**
 * Returns FU cost estimation data.
 *
 * This version assumes that there's only one entry with given parameters.
 * 
 * @todo Another version for fetching lists of data.
 * @todo Refactor most of the code in *costEstimationData() functions to
 *       a helper function
 *
 * @param valueName Name of the value to fetch.
 * @param implementationId The ID of the FU entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain FU cost estimation data
 *                        with the given arguments.
 */
DataObject
HDBManager::fuCostEstimationData(
    const std::string& valueName, RowID implementationId,
    const std::string& pluginName) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND bus_reference IS NULL " +
            " AND socket_reference IS NULL AND " +
            "     fu_reference = " + Conversion::toString(implementationId) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');");
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns RF cost estimation data.
 *
 * This version assumes that there's only one entry with given parameters.
 * 
 * @todo Another version for fetching lists of data.
 *
 * @param valueName Name of the value to fetch.
 * @param implementationId The ID of the RF entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain RF cost estimation data
 *                        with the given arguments.
 */
DataObject
HDBManager::rfCostEstimationData(
    const std::string& valueName, RowID implementationId,
    const std::string& pluginName) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND fu_reference IS NULL " +
            " AND bus_reference IS NULL " +
            " AND socket_reference IS NULL AND " +
            "     rf_reference = " + Conversion::toString(implementationId) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');");
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Adds an empty Bus entry to the database.
 *
 * @param entry The Bus entry.
 * @return ID of the added Bus entry.
 */
RowID
HDBManager::addBusEntry() const {
    try {
        dbConnection_->updateQuery(
            std::string("INSERT INTO bus(id) VALUES(NULL);"));
        return dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}    


/**
 * Removes the Bus entry that has the given ID from the database.
 *
 * The entry is removed entirely, meaning that also cost estimation data
 * of that entry are removed.
 *
 * @param id The ID of the Bus entry.
 */
void
HDBManager::removeBusEntry(RowID id) const {

    if (!hasBusEntry(id)) {
        return;
    }

    // remove cost estimation data
    try {
        // get the IDs of cost estimation datas
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM cost_estimation_data "
                "WHERE bus_reference=" + Conversion::toString(id) + ";"));
        while (result->hasNext()) {
            result->next();
            const DataObject& costIDData = result->data(0);
            int dataID = costIDData.integerValue();
            removeCostEstimationData(dataID);
        }
        delete result;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove from bus table
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM bus "
                "WHERE id=" + Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Adds the given cost estimation data values to given Bus entry.
 *
 * @param busID The ID of the Bus entry the cost data will be added.
 * @valueName The name of the cost value.
 * @value The cost value.
 * @pluginID The ID of the cost function plugin that owns this data.
 */
RowID
HDBManager::addBusCostEstimationData(
    RowID busID,
    const std::string& valueName,
    const std::string& value,
    RowID pluginID) const {


    RowID dataID;

    // add the data
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO cost_estimation_data (id,plugin_reference,"
                "bus_reference,name,value) VALUES (NULL," + 
                Conversion::toString(pluginID) + "," + 
                Conversion::toString(busID) + ",\"" + valueName + "\",\"" +
                value + "\");"));
        dataID = dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    return dataID;
}



/**
 * Returns bus cost estimation data.
 *
 * This version assumes that there's only one entry with given parameters.
 *
 * @param valueName Name of the value to fetch.
 * @param busID The ID of the bus entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain bus cost estimation data
 *                        with the given arguments.
 */
DataObject
HDBManager::busCostEstimationData(
    const std::string& valueName, RowID busID,
    const std::string& pluginName) const {
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(            
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND socket_reference IS NULL AND " +
            "     bus_reference = " + Conversion::toString(busID) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');");
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns a list of bus cost estimation data.
 *
 * @param valueName Name of the value to fetch.
 * @param busID The ID of the bus entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data. Becomes property of the caller.
 * @exception KeyNotFound If the HDB does not contain bus cost estimation data
 *                        with the given arguments.
 */
HDBManager::DataObjectList*
HDBManager::busCostEstimationDataList(
    const std::string& valueName, RowID busID,
    const std::string& pluginName) const {
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(            
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND socket_reference IS NULL AND " +
            "     bus_reference = " + Conversion::toString(busID) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');");
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }   

    if (queryResult->hasNext()) {
     
        DataObjectList* data = new DataObjectList;
   
        while (queryResult->hasNext()) {
            queryResult->next();
            DataObject value = queryResult->data("value");
            data->push_back(value);
        }

        delete queryResult;
        queryResult = NULL;
        return data;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Adds an empty Socket entry to the database.
 *
 * @param entry The Socket entry.
 * @return ID of the added Socket entry.
 */
RowID
HDBManager::addSocketEntry() const {
    try {
        dbConnection_->updateQuery(
            std::string("INSERT INTO socket(id) VALUES(NULL);"));
        return dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}    


/**
 * Removes the Socket entry that has the given ID from the database.
 *
 * The entry is removed entirely, meaning that also cost estimation data
 * of that entry are removed.
 *
 * @param id The ID of the Socket entry.
 */
void
HDBManager::removeSocketEntry(RowID id) const {

    if (!hasSocketEntry(id)) {
        return;
    }

    // remove cost estimation data
    try {
        // get the IDs of cost estimation datas
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM cost_estimation_data "
                "WHERE socket_reference=" + Conversion::toString(id) + ";"));
        while (result->hasNext()) {
            result->next();
            const DataObject& costIDData = result->data(0);
            int dataID = costIDData.integerValue();
            removeCostEstimationData(dataID);
        }
        delete result;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // remove from socket table
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM socket "
                "WHERE id=" + Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Adds the given cost estimation data values to given Socket entry.
 *
 * @param socketID The ID of the Socket entry the cost data will be added.
 * @valueName The name of the cost value.
 * @value The cost value.
 * @pluginID The ID of the cost function plugin that owns this data.
 */
RowID
HDBManager::addSocketCostEstimationData(
    RowID socketID,
    const std::string& valueName,
    const std::string& value,
    RowID pluginID) const {


    RowID dataID;

    // add the data
    try {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO cost_estimation_data (id,plugin_reference,"
                "socket_reference,name,value) VALUES (NULL," + 
                Conversion::toString(pluginID) + "," + 
                Conversion::toString(socketID) + ",\"" + valueName + "\",\"" +
                value + "\");"));
        dataID = dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    return dataID;
}


/**
 * Returns socket cost estimation data.
 *
 * This version assumes that there's only one entry with given parameters.
 * 
 * @param valueName Name of the value to fetch.
 * @param socketID The ID of the socket entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain socket cost estimation 
 *                        data with the given arguments.
 */
DataObject
HDBManager::socketCostEstimationData(
    const std::string& valueName, RowID socketID,
    const std::string& pluginName) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND bus_reference IS NULL AND " +
            "     socket_reference = " + 
            Conversion::toString(socketID) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');";
        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns socket cost estimation data.
 * 
 * @param valueName Name of the value to fetch.
 * @param socketID The ID of the socket entry.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data. Becomes property of the caller.
 * @exception KeyNotFound If the HDB does not contain socket cost estimation 
 *                        data with the given arguments.
 */
HDBManager::DataObjectList*
HDBManager::socketCostEstimationDataList(
    const std::string& valueName, RowID socketID,
    const std::string& pluginName) const {
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(            
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND bus_reference IS NULL AND " +
            "     socket_reference = " + 
            Conversion::toString(socketID) + 
            " AND cost_estimation_data.name LIKE('" + valueName + "');");
    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    }

    if (queryResult->hasNext()) {
     
        DataObjectList* data = new DataObjectList;
   
        while (queryResult->hasNext()) {
            queryResult->next();
            DataObject value = queryResult->data("value");
            data->push_back(value);
        }

        delete queryResult;
        queryResult = NULL;
        return data;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns cost estimation data which is not connected to any machine 
 * implementation id.
 *
 * This version assumes that there's only one entry with given parameters.
 * 
 * @todo Another version for fetching lists of data.
 *
 * @param valueName Name of the value to fetch.
 * @param pluginName Name of the cost estimation plugin that owns the data.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain cost estimation 
 *                        data with the given arguments.
 */
DataObject
HDBManager::costEstimationDataValue(
    const std::string& valueName, const std::string& pluginName) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT value "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE plugin_reference = cost_function_plugin.id AND " 
                "cost_function_plugin.name LIKE('") + 
            pluginName + "') " +
            " AND rf_reference IS NULL " +
            " AND fu_reference IS NULL " +
            " AND socket_reference IS NULL " +
            " AND bus_reference IS NULL " +       
            " AND cost_estimation_data.name LIKE('" + valueName + "');";
        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns cost estimation data value with the given id.
 *
 * @param entryId Id of the cost estimation data entry.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain cost estimation 
 *                        data with the given arguments.
 */
DataObject
HDBManager::costEstimationDataValue(RowID entryId) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT value "
                "FROM cost_estimation_data "
                "WHERE cost_estimation_data.id = ") +
            Conversion::toString(entryId);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    if (queryResult->hasNext()) {
        queryResult->next();
        
        DataObject value = queryResult->data("value");

        delete queryResult;
        queryResult = NULL;

        return value;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Tells whether the HDB has an FU entry that has the given ID.
 *
 * @return True if the HDB has the entry, otherwise false.
 */
bool
HDBManager::hasFUEntry(RowID id) const {

    RelationalDBQueryResult* result;

    try {
        result = dbConnection_->query(fuEntryByIDQuery(id));
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}


/**
 * Tells whether the HDB has an RF entry that has the given ID.
 *
 * @return True if the HDB has the entry, otherwise false.
 */
bool
HDBManager::hasRFEntry(RowID id) const {

    RelationalDBQueryResult* result;

    try {
        result = dbConnection_->query(rfEntryByIDQuery(id));
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}


/**
 * Tells whether the HDB has an Bus entry that has the given ID.
 *
 * @return True if the HDB has the entry, otherwise false.
 */
bool
HDBManager::hasBusEntry(RowID id) const {

    RelationalDBQueryResult* result;

    try {
        result = dbConnection_->query(busEntryByIDQuery(id));
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}


/**
 * Tells whether the HDB has an Socket entry that has the given ID.
 *
 * @return True if the HDB has the entry, otherwise false.
 */
bool
HDBManager::hasSocketEntry(RowID id) const {

    RelationalDBQueryResult* result;

    try {
        result = dbConnection_->query(socketEntryByIDQuery(id));
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Tells whether the HDB has cost estimation data that has the given ID.
 *
 * @return True if the HDB has the data, otherwise false.
 */
bool
HDBManager::hasCostEstimationDataByID(RowID id) const {

    RelationalDBQueryResult* result;
    std::string query = "SELECT id FROM cost_estimation_data WHERE id=";
    query += Conversion::toString(id) + ";";

    try {
        result = dbConnection_->query(query);
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}

/**
 * Tells whether the HDB has cost function plugin that has the given ID.
 *
 * @return True if the HDB has the plugin, otherwise false.
 */
bool
HDBManager::hasCostFunctionPluginByID(RowID id) const {

    RelationalDBQueryResult* result;
    std::string query = "SELECT id FROM cost_function_plugin WHERE id=";
    query += Conversion::toString(id) + ";";

    try {
        result = dbConnection_->query(query);
    } catch (const Exception&) {
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        assert(!result->hasNext());
        delete result;
        return true;
    } else {
        delete result;
        return false;
    }
}


/**
 * Tells whether the FU entry that has the given ID has an architecture.
 *
 * @param id ID of the FU entry.
 * @return True if it has an architecture, otherwise false.
 * @exception KeyNotFound If the HDB does not contain a FU entry with the given
 *                        ID.
 */
bool
HDBManager::fuEntryHasArchitecture(RowID id) const {
    RelationalDBQueryResult* result;
    try {
        result = dbConnection_->query(
            std::string(
                "SELECT architecture FROM fu WHERE id=" + 
                Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (result->hasNext()) {
        result->next();
        const DataObject& data = result->data(0);
        delete result;
        return !data.isNull();
    } else {
        delete result;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Tells whether the RF entry that has the given ID has an architecture.
 *
 * @param id ID of the RF entry.
 * @return True if it has an architecture, otherwise false.
 * @exception KeyNotFound If the HDB does not contain a RF entry with the
 *                        given ID.
 */
bool
HDBManager::rfEntryHasArchitecture(RowID id) const {
    RelationalDBQueryResult* result;
    try {
        result = dbConnection_->query(
            std::string(
                "SELECT architecture FROM rf WHERE id=" +
                Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    
    if (result->hasNext()) {
        result->next();
        const DataObject& data = result->data(0);
        delete result;
        return !data.isNull();
    } else {
        delete result;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Tells whether the HDB contains the given operation in operation table.
 *
 * @param opName Name of the operation.
 * @return True if HDB contains the operation, otherwise false.
 */
bool
HDBManager::containsOperation(const std::string& opName) const {
    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT * FROM operation WHERE lower(name)=lower(\"" + opName
                + "\");"));
        bool returnValue = result->hasNext();
        delete result;
        return returnValue;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Tells whether the HDB contains the given block implementation file.
 *
 * @param pathToFile Full path to the file.
 */
bool
HDBManager::containsImplementationFile(const std::string& pathToFile) const {
    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT * FROM block_source_file WHERE file=\"" +
                pathToFile + "\";"));
        bool returnValue = result->hasNext();
        delete result;
        return returnValue;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Tells whether the HDB contains a FU architecture that has the given ID.
 *
 * @param id The ID.
 * @return True if the HDB contains the architecture, otherwise false.
 */
bool
HDBManager::containsFUArchitecture(RowID id) const {

    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM fu_architecture WHERE id=" +
                Conversion::toString(id) + ";"));
        bool returnValue = result->hasNext();
        delete result;
        return returnValue;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Tells whether the HDB contains a RF architecture with the given ID.
 *
 * @param id The ID.
 * @return True if the HDB contains the architecture, otherwise false.
 */
bool
HDBManager::containsRFArchitecture(RowID id) const {
    try {
        RelationalDBQueryResult* result = dbConnection_->query(
            std::string(
                "SELECT id FROM rf_architecture WHERE id=" +
                Conversion::toString(id) + ";"));
        bool returnValue = result->hasNext();
        delete result;
        return returnValue;
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return false;
}


/**
 * Returns the ID of the architecture of the given FU entry.
 *
 * @param fuEntryID ID of the FU entry.
 * @return ID of the FU architecture.
 * @exception NotAvailable If the FU entry does not have an architecture.
 */
RowID
HDBManager::fuArchitectureID(RowID fuEntryID) const {
    RelationalDBQueryResult* result;
    try {
        result = dbConnection_->query(
            std::string(
                "SELECT architecture FROM fu WHERE id=" +
                Conversion::toString(fuEntryID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (!result->hasNext()) {
        delete result;
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    result->next();
    const DataObject& idData = result->data(0);
    if (idData.isNull()) {
        delete result;
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        RowID retValue = idData.integerValue();
        delete result;
        return retValue;
    }
}

/**
 * Returns the ID of the architecture of the given RF entry.
 *
 * @param rfEntryID ID of the RF entry.
 * @return ID of the RF architecture.
 * @exception NotAvailable If the RF entry does not have an architecture.
 */
RowID
HDBManager::rfArchitectureID(RowID rfEntryID) const {
    RelationalDBQueryResult* result;
    try {
        result = dbConnection_->query(
            std::string(
                "SELECT architecture FROM rf WHERE id=" +
                Conversion::toString(rfEntryID) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (!result->hasNext()) {
        delete result;
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    result->next();
    const DataObject& idData = result->data(0);
    if (idData.isNull()) {
        delete result;
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        RowID retValue = idData.integerValue();
        delete result;
        return retValue;
    }
}

/**
 * Returns true if a table by name has a column by given name.
 *
 * @param table The table by name to search the column from.
 * @param columnName The name of the column to be searched.
 * @return True if the table has the named column.
 */
bool
HDBManager::hasColumn(
    const std::string& table, const std::string& columnName) const {

    std::string table_info_query("PRAGMA table_info(");
    table_info_query += table;
    table_info_query += ");";

    RelationalDBQueryResult* result;
    try {
        result = dbConnection_->query(table_info_query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    while(result->hasNext()) {
        result->next();
        // Second column in result row is column name of the table.
        const DataObject& columnData = result->data(1);
        std::string columnNameFromTable = columnData.stringValue();

        assert(!columnNameFromTable.empty());
        if(columnNameFromTable == columnName) {
            return true;
        }
    }

    return false;
}

/**
 * Inserts a new boolean type column into existing table.
 *
 * @param table The name of the targeted table.
 * @param newColumn The name of the new column.
 * @return Number of rows affected by the change.
 */
int
HDBManager::addBooleanColumn(const std::string& table,
    const std::string& newcolumn) {
    std::string add_column_query("ALTER TABLE ");
    add_column_query += table + " ADD COLUMN " + newcolumn;
    add_column_query += " INTEGER DEFAULT 0;";

    int result = 0;

    try {
        result = dbConnection_->updateQuery(add_column_query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    return result;
}

/**
 * Obtains data from HDB and creates ports and operand bindings to the given
 * FU architecture that has the given ID in HDB.
 *
 * @param architecture The FU architecture to which the ports are added.
 * @param id ID of the FU architecture in HDB.
 */
void
HDBManager::addPortsAndBindingsToFUArchitecture(
    FUArchitecture& architecture,
    RowID id) const {

    FunctionUnit& fu = architecture.architecture();

    // make the SQL query to obtain the ports
    RelationalDBQueryResult* fuPorts = NULL;
    try {
        fuPorts = dbConnection_->query(fuPortsAndBindingsByIDQuery(id));
    } catch (const Exception& e) {
        abortWithError(e.errorMessage());
    }
    int portIDColumnIndex = fuPorts->column("fu_data_port.id");
    int triggersColumnIndex = fuPorts->column("fu_data_port.triggers");
    int setsOpcodeColumnIndex = fuPorts->column("fu_data_port.sets_opcode");
    int guardSupportColumnIndex = fuPorts->column(
        "fu_data_port.guard_support");
    int widthColumnIndex = fuPorts->column("fu_data_port.width");
    int operationColumnIndex = fuPorts->column("operation.name");
    int bindingColumnIndex = fuPorts->column("io_binding.io_number");

    // @fixme Do not assert() inside a library function in case of broken
    // user input data!!
    assert(portIDColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(triggersColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(setsOpcodeColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(
        guardSupportColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(widthColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(operationColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(bindingColumnIndex != RelationalDBQueryResult::UNKNOWN_INDEX);

    if (!fuPorts->hasNext()) {
        delete fuPorts;
        abortWithError("No row.");
    }

    std::map<int, std::string> portIDMap;

    // create ports, operations and bindings to the FU
    int name(1);
    while (fuPorts->hasNext()) {
        fuPorts->next();

        const DataObject& idData = fuPorts->data(portIDColumnIndex);
        const DataObject& triggersData = fuPorts->data(triggersColumnIndex);
        const DataObject& setsOpcodeData = fuPorts->data(
            setsOpcodeColumnIndex);
        const DataObject& guardData = fuPorts->data(guardSupportColumnIndex);
        const DataObject& widthData = fuPorts->data(widthColumnIndex);
        const DataObject& operationData = fuPorts->data(
            operationColumnIndex);
        const DataObject& bindingData = fuPorts->data(bindingColumnIndex);

        int portID = idData.integerValue();

        // create operation if it is not created yet
        string operationName = operationData.stringValue();
        if (!fu.hasOperation(operationName)) {
            new HWOperation(operationName, fu);
        }

        // create port if it is not created yet
        if (!MapTools::containsKey(portIDMap, portID)) {
            bool triggers = triggersData.boolValue();
            bool setsOpcode = setsOpcodeData.boolValue();            
            string portName = "p" + Conversion::toString(name);

            int width = DEFAULT_PORT_WIDTH;
            if (widthData.isNull()) {
                architecture.setParameterizedWidth(portName);
            } else {
                width = widthData.integerValue();
            }
            new FUPort(portName, width, fu, triggers, setsOpcode);
            portIDMap.insert(
                std::pair<int, string>(idData.integerValue(), portName));
            if (setsOpcode && !triggers) 
                debugLog(
                    std::string("Created a suspicious port ") + portName + 
                    " which sets opcode but does not trigger");

            // set guard support
            if (guardData.boolValue()) {
                architecture.setGuardSupport(portName);
            }

            name++;
        }
        
        // create binding
        FUPort* portToBind = fu.operationPort(
            MapTools::valueForKey<string>(portIDMap, portID));
        HWOperation* operation = fu.operation(operationName);
        operation->bindPort(bindingData.integerValue(), *portToBind);
    }

    delete fuPorts;
    fuPorts = NULL;
}


/**
 * Obtains data from HDB and creates the operation pipelines to the
 * given FU architecture.
 *
 * @param architecture The FU architecture to which the operations are added.
 * @param id ID the FU architecture in HDB.
 */
void
HDBManager::addOperationPipelinesToFUArchitecture(
    FUArchitecture& architecture,
    RowID id) const {

    FunctionUnit& fu = architecture.architecture();

    // make the SQL query to obtain IO usage data
    RelationalDBQueryResult* ioUsageData = NULL;
    try {
        ioUsageData = dbConnection_->query(ioUsageDataByIDQuery(id));
    } catch (const Exception& e) {
        assert(false);
    }
    int operationColumn = ioUsageData->column("operation.name");
    int cycleColumn = ioUsageData->column("io_usage.cycle");
    int ioColumn = ioUsageData->column("io_usage.io_number");
    int actionColumn = ioUsageData->column("io_usage.action");
    
    while (ioUsageData->hasNext()) {
        ioUsageData->next();
        const DataObject& operationData = ioUsageData->data(operationColumn);
        const DataObject& cycleData = ioUsageData->data(cycleColumn);
        const DataObject& ioData = ioUsageData->data(ioColumn);
        const DataObject& actionData = ioUsageData->data(actionColumn);

        string operationName = operationData.stringValue();
        int cycle = cycleData.integerValue();
        int ioNumber = ioData.integerValue();
        int action = actionData.boolValue();
        
        assert(fu.hasOperation(operationName));
        HWOperation* operation = fu.operation(operationName);
        ExecutionPipeline* pipeline = operation->pipeline();
        if (action == READ_ACTION) {
            pipeline->addPortRead(ioNumber, cycle, 1);
        } else if (action == WRITE_ACTION) {
            pipeline->addPortWrite(ioNumber, cycle, 1);
        }
    }

    delete ioUsageData;
    ioUsageData = NULL;

    // add resource usages
    RelationalDBQueryResult* resUsageData = NULL;
    try {
        resUsageData = dbConnection_->query(resourceUsageDataByIDQuery(id));
    } catch (const Exception&) {
        assert(false);
    }
    operationColumn = resUsageData->column("operation.name");
    cycleColumn = resUsageData->column("pipeline_resource_usage.cycle");
    int resourceColumn = resUsageData->column("pipeline_resource.id");

    int resourceName(0);
    std::map<int, string> resourceMap;

    while (resUsageData->hasNext()) {
        resUsageData->next();
        const DataObject& operationData = resUsageData->data(
            operationColumn);
        const DataObject& cycleData = resUsageData->data(cycleColumn);
        const DataObject& resourceData = resUsageData->data(resourceColumn);

        string operationName = operationData.stringValue();
        int cycle = cycleData.integerValue();
        int resourceID = resourceData.integerValue();
        
        assert(fu.hasOperation(operationName));
        HWOperation* operation = fu.operation(operationName);
        ExecutionPipeline* pipeline = operation->pipeline();
        if (!MapTools::containsKey(resourceMap, resourceID)) {
            resourceMap.insert(
                std::pair<int, string>(
                    resourceID, "res" + Conversion::toString(resourceName)));
            resourceName++;
        }
        pipeline->addResourceUse(
            MapTools::valueForKey<string>(
                resourceMap, resourceID), cycle, 1);
    }

    delete resUsageData;
    resUsageData = NULL;
}


/**
 * Obtains implementation data of the FU that has the given ID and creates an
 * FUImplementation instance of it.
 *
 * @param architecture Architecture of the FU (needed when matching the
 *                     ports in the implementation).
 * @param id ID of the FU entry.
 * @return The newly created FUImplementation instance or NULL if the FU
 *         has no implementation.
 */
FUImplementation*
HDBManager::createImplementationOfFU(
    FUArchitecture& architecture,
    RowID id) const {

    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* implData = NULL;
    const std::string queryString = fuImplementationByIDQuery(id);
    try {
        implData = dbConnection_->query(queryString);
    } catch (const Exception& e) {
        delete implData;
        debugLog(
            std::string("query ") + queryString + " threw something: " + 
            e.errorMessage());
        return NULL;
    }

    FUImplementation* implementation = NULL;

    if (implData->hasNext()) {
        implData->next();
        assert(!implData->hasNext());

        int idColumn = implData->column("fu_implementation.id");
        int nameColumn = implData->column("fu_implementation.name");
        int opcodePortColumn = implData->column(
            "fu_implementation.opcode_port");
        int clkPortColumn = implData->column("fu_implementation.clk_port");
        int rstPortColumn = implData->column("fu_implementation.rst_port");
        int glockPortColumn = implData->column(
            "fu_implementation.glock_port");
        int glockReqPortColumn = implData->column(
            "fu_implementation.glock_req_port");
        
        const DataObject& idData = implData->data(idColumn);
        const DataObject& nameData = implData->data(nameColumn);
        const DataObject& opcodePortData = implData->data(opcodePortColumn);
        const DataObject& clkPortData = implData->data(clkPortColumn);
        const DataObject& rstPortData = implData->data(rstPortColumn);
        const DataObject& glockPortData = implData->data(glockPortColumn);
        const DataObject& glockReqPortData = implData->data(
            glockReqPortColumn);

        RowID implID = idData.integerValue();
        string name = nameData.stringValue();
        string opcodePort = opcodePortData.stringValue();
        string clkPort = clkPortData.stringValue();
        string rstPort = rstPortData.stringValue();
        string glockPort = glockPortData.stringValue();
        string glockReqPort = glockReqPortData.stringValue();

        implementation = new FUImplementation(
            name, opcodePort, clkPort, rstPort, glockPort, glockReqPort);
        implementation->setID(implID);
        
        addFUParametersToImplementation(*implementation, id);
        addOpcodesToImplementation(*implementation, id);
        addDataPortsToImplementation(*implementation, architecture, id);
        addFUExternalPortsToImplementation(*implementation, id);
        addBlockImplementationFiles(*implementation, id);
    }
    
    delete implData;
    implData = NULL;
    return implementation;
}

/**
 * Obtains the cost function data of the FU that has the given ID and creates 
 * a CostFunctionPlugin instance of it.
 *
 * @param id ID of the FU entry.
 * @return The newly created FUImplementation instance or NULL if the FU
 *         has no cost function.
 */
CostFunctionPlugin*
HDBManager::createCostFunctionOfFU(RowID id) const {

    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(
            "SELECT cost_function_plugin.id AS id, "
            "       cost_function_plugin.description AS description,"
            "       cost_function_plugin.name AS name, "
            "       cost_function_plugin.plugin_file_path AS plugin_file_path "
            "FROM cost_function_plugin, fu "
            "WHERE fu.id = " + Conversion::toString(id) + 
            " AND cost_function_plugin.id = fu.cost_function;");
    } catch (const Exception& e) {
        delete queryResult;
        debugLog(e.errorMessage());
        return NULL;
    }

    CostFunctionPlugin* costFunction = NULL;

    if (queryResult->hasNext()) {
        queryResult->next();
        
        const DataObject& pluginIdData = 
            queryResult->data("id");
        const DataObject& descriptionData = 
            queryResult->data("description");
        const DataObject& nameData = 
            queryResult->data("name");
        const DataObject& pluginFilePathData = 
            queryResult->data("plugin_file_path");

        int pluginId = -1;
        string description = "";
        string name = "";
        string pluginFilePath = "";
        try {
            if (!pluginIdData.isNull()) {
                pluginId = pluginIdData.integerValue();
            } 
            description = descriptionData.stringValue();
            name = nameData.stringValue();
            pluginFilePath = pluginFilePathData.stringValue();
        } catch (const Exception& e) {
            debugLog(
                std::string("Something wrong with conversion: ") + 
                e.errorMessage());
            delete queryResult;
            queryResult = NULL;
            return NULL;
        }

        costFunction = new CostFunctionPlugin(
            pluginId, description, name, pluginFilePath, 
            CostFunctionPlugin::COST_FU);
    }
    
    delete queryResult;
    queryResult = NULL;
    return costFunction;
}

/**
 * Obtains the cost function data of the RF that has the given ID and creates 
 * a CostFunctionPlugin instance of it.
 *
 * @param id ID of the RF entry.
 * @return The newly created FUImplementation instance or NULL if the RF
 *         has no cost function.
 */
CostFunctionPlugin*
HDBManager::createCostFunctionOfRF(RowID id) const {

    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        queryResult = dbConnection_->query(
            "SELECT cost_function_plugin.id AS id, "
            "       cost_function_plugin.description AS description,"
            "       cost_function_plugin.name AS name, "
            "       cost_function_plugin.plugin_file_path AS plugin_file_path "
            "FROM cost_function_plugin, rf "
            "WHERE rf.id = " + Conversion::toString(id) + 
            " AND cost_function_plugin.id = rf.cost_function;");
    } catch (const Exception& e) {
        delete queryResult;
        debugLog(e.errorMessage());
        return NULL;
    }

    CostFunctionPlugin* costFunction = NULL;

    if (queryResult->hasNext()) {
        queryResult->next();
        
        const DataObject& pluginIdData = 
            queryResult->data("id");
        const DataObject& descriptionData = 
            queryResult->data("description");
        const DataObject& nameData = 
            queryResult->data("name");
        const DataObject& pluginFilePathData = 
            queryResult->data("plugin_file_path");

        int pluginId = -1;
        string description = "";
        string name = "";
        string pluginFilePath = "";
        try {
            if (!pluginIdData.isNull()) {
                pluginId = pluginIdData.integerValue();
            } 
            description = descriptionData.stringValue();
            name = nameData.stringValue();
            pluginFilePath = pluginFilePathData.stringValue();
        } catch (const Exception& e) {
            debugLog(
                std::string("Something wrong with conversion: ") + 
                e.errorMessage());
            delete queryResult;
            queryResult = NULL;
            return NULL;
        }

        costFunction = new CostFunctionPlugin(
            pluginId, description, name, pluginFilePath, 
            CostFunctionPlugin::COST_RF);
    }
    
    delete queryResult;
    queryResult = NULL;
    return costFunction;
}



/**
 * Obtains implementation data of the RF that has the given ID and creates
 * an RFImplementation instance of it.
 *
 * @param id ID of the RF entry.
 * @return The newly created RFImplementation instance or NULL if the RF
 *         has no implementation.
 */
RFImplementation*
HDBManager::createImplementationOfRF(RowID id) const {

    RelationalDBQueryResult* implementationData = NULL;
    try {
        if(hasColumn("rf_implementation", "sac_param")) {
            // Use new query.
            implementationData = dbConnection_->query(
                rfImplementationByIDQuery2(id));
        } else {
            // Use fallback query.
            implementationData = dbConnection_->query(
                rfImplementationByIDQuery(id));
        }
    } catch (const Exception& e) {
        assert(false);
    }

    RFImplementation* implementation = NULL;

    if (implementationData->hasNext()) {
        implementationData->next();
        assert(!implementationData->hasNext());
        int idColumn = implementationData->column("id");
        int nameColumn = implementationData->column("name");
        int sizeParamColumn = implementationData->column("size_param");
        int widthParamColumn = implementationData->column("width_param");
        int sacParamColumn = implementationData->column("sac_param");
        int clkPortColumn = implementationData->column("clk_port");
        int rstPortColumn = implementationData->column("rst_port");
        int glockPortColumn = implementationData->column("glock_port");
        int guardPortColumn = implementationData->column("guard_port");
        
        const DataObject& idData = implementationData->data(idColumn);
        const DataObject& nameData = implementationData->data(nameColumn);
        const DataObject& sizeParamData = implementationData->data(
            sizeParamColumn);
        const DataObject& widthParamData = implementationData->data(
            widthParamColumn);
        const DataObject& clkPortData = implementationData->data(
            clkPortColumn);
        const DataObject& rstPortData = implementationData->data(
            rstPortColumn);
        const DataObject& glockPortData = implementationData->data(
            glockPortColumn);
        const DataObject& guardPortData = implementationData->data(
            guardPortColumn);
        const DataObject& sacParamData = implementationData->data(
            sacParamColumn);

        string sizeParam = sizeParamData.stringValue();
        string widthParam = widthParamData.stringValue();
        string guardPort = guardPortData.stringValue();
        bool sacParam =
            (sacParamColumn != RelationalDBQueryResult::UNKNOWN_INDEX) ?
            sacParamData.boolValue() :
            false;

        implementation = new RFImplementation(
            nameData.stringValue(), clkPortData.stringValue(), 
            rstPortData.stringValue(), glockPortData.stringValue(), 
            sizeParam, widthParam, guardPort, sacParam);
        implementation->setID(idData.integerValue());

        addRFParametersToImplementation(*implementation, id);
        addDataPortsToImplementation(*implementation, id);
        addRFExternalPortsToImplementation(*implementation, id);
        addBlockImplementationFiles(*implementation, id);
    }
    
    delete implementationData;
    implementationData = NULL;
    return implementation;
}


/**
 * Resolves what is the architectural port corresponding to the given
 * implemented port.
 *
 * @param architecture The architecture of the FU.
 * @param id ID of the FU entry in the database.
 * @param implementedPort Name of the implemented port to resolve.
 * @return Name of the corresponding port in the FU architecture.
 */
std::string
HDBManager::resolveArchitecturePort(
    const FUArchitecture& architecture,
    RowID entryID,
    const std::string& implementedPort) const {

    RelationalDBQueryResult* bindingData = NULL;
    try {
        bindingData = dbConnection_->query(
            fuPortBindingByNameQuery(entryID, implementedPort));
    } catch (const Exception&) {
        assert(false);
    }

    int operationColumn = bindingData->column("operation.name");
    int ioColumn = bindingData->column("io_binding.io_number");
    assert(operationColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(ioColumn != RelationalDBQueryResult::UNKNOWN_INDEX);

    FunctionUnit& fu = architecture.architecture();
    string portName = "";

    while (bindingData->hasNext()) {
        bindingData->next();
        const DataObject& operationData = bindingData->data(operationColumn);
        const DataObject& ioData = bindingData->data(ioColumn);
        string operationName = operationData.stringValue();
        int io = ioData.integerValue();
        assert(fu.hasOperation(operationName));
        HWOperation* operation = fu.operation(operationName);
        FUPort* port = operation->port(io);
        assert(portName == "" || portName == port->name());
        portName = port->name();
    }

    delete bindingData;
    bindingData = NULL;

    return portName;
}


/**
 * Adds the operation codes to the given FU implementation which is the
 * implementation of the FU entry that has the given ID.
 *
 * @param implementation The FU implementation.
 * @param entryID ID of the FU entry.
 */
void
HDBManager::addOpcodesToImplementation(
    FUImplementation& implementation,
    RowID entryID) const {

    RelationalDBQueryResult* opcodeData = NULL;
    try {
        opcodeData = dbConnection_->query(opcodesByIDQuery(entryID));
    } catch (const Exception&) {
        assert(false);
    }
    int operationColumn = opcodeData->column("operation.name");
    int opcodeColumn = opcodeData->column("opcode_map.opcode");
    while (opcodeData->hasNext()) {
        opcodeData->next();
        const DataObject& operationData = opcodeData->data(
            operationColumn);
        const DataObject& opcodeDataObject = opcodeData->data(
            opcodeColumn);
        implementation.setOpcode(
            operationData.stringValue(), opcodeDataObject.integerValue());
    }
    delete opcodeData;
    opcodeData = NULL;
}


/**
 * Adds data ports to the given FU implementation which is the implementation
 * of the FU entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param architecture The corresponding architecture.
 * @param entryID ID of the FU entry.
 */
void
HDBManager::addDataPortsToImplementation(
    FUImplementation& implementation,
    FUArchitecture& architecture,
    RowID entryID) const {

    RelationalDBQueryResult* portData = NULL;
    try {
        portData = dbConnection_->query(
            fuImplementationDataPortsByIDQuery(entryID));
    } catch (const Exception& e) {
        assert(false);
    }
    int portNameColumn = portData->column("fu_port_map.name");
    int widthFormulaColumn = portData->column("fu_port_map.width_formula");
    int loadPortColumn = portData->column("fu_port_map.load_port");
    int guardPortColumn = portData->column("fu_port_map.guard_port");
    
    while (portData->hasNext()) {
        portData->next();
        const DataObject& portNameData = portData->data(portNameColumn);
        const DataObject& widthFormulaData = portData->data(
            widthFormulaColumn);
        const DataObject& loadPortData = portData->data(loadPortColumn);
        const DataObject& guardPortData = portData->data(guardPortColumn);
        
        string portName = portNameData.stringValue();
        string widthFormula = widthFormulaData.stringValue();
        string loadPort = loadPortData.stringValue();
        string guardPort = guardPortData.stringValue();
        string architecturePort = resolveArchitecturePort(
            architecture, entryID, portName);
        new FUPortImplementation(
            portName, architecturePort, widthFormula, loadPort, guardPort,
            implementation);
    }

    delete portData;
    portData = NULL;
}


/**
 * Adds external ports to the given FU implementation which is the
 * implementation of the FU entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the FU entry.
 */
void
HDBManager::addFUExternalPortsToImplementation(
    FUImplementation& implementation,
    RowID entryID) const {

    RelationalDBQueryResult* extPortData = NULL;
    try {
        extPortData = dbConnection_->query(fuExternalPortsByIDQuery(entryID));
    } catch (const Exception& e) {
        assert(false);
    }

    int extPortNameColumn = extPortData->column("fu_external_port.name");
    int directionColumn = extPortData->column(
        "fu_external_port.direction");
    int extPortWidthFormulaColumn = extPortData->column(
        "fu_external_port.width_formula");
    int descriptionColumn = extPortData->column(
        "fu_external_port.description");
    
    while (extPortData->hasNext()) {
        extPortData->next();
        const DataObject& nameData = extPortData->data(
            extPortNameColumn);
        const DataObject& directionData = extPortData->data(
            directionColumn);
        const DataObject& widthFormulaData = extPortData->data(
            extPortWidthFormulaColumn);
        const DataObject& descriptionData = extPortData->data(
            descriptionColumn);
        
        string name = nameData.stringValue();
        string widthFormula = widthFormulaData.stringValue();
        string description = descriptionData.stringValue();

        Direction direction;
        if (directionData.stringValue() == IN_DIRECTION) {
            direction = IN;
        } else if (directionData.stringValue() == OUT_DIRECTION) {
            direction = OUT;
        } else {
            assert(directionData.stringValue() == BIDIR_DIRECTION);
            direction = BIDIR;
        }

        new FUExternalPort(
            name, direction, widthFormula, description, implementation);
    }
    
    delete extPortData;
    extPortData = NULL;

    // add parameter dependencies
    for (int i = 0; i < implementation.externalPortCount(); i++) {
        FUExternalPort& port = implementation.externalPort(i);
        try {
            RelationalDBQueryResult* result = dbConnection_->query(
                std::string(
                    "SELECT fu_implementation_parameter.name FROM "
                    "fu_implementation_parameter, fu_external_port, "
                    "fu_ext_port_parameter_dependency, fu_implementation "
                    "WHERE fu_implementation.fu=" +
                    Conversion::toString(entryID) +
                    " AND fu_external_port.fu_impl=fu_implementation.id AND "
                    "fu_external_port.name=\"" + port.name() +
                    "\" AND fu_ext_port_parameter_dependency.port="
                    "fu_external_port.id AND fu_implementation_parameter.id="
                    "fu_ext_port_parameter_dependency.parameter;"));
            while (result->hasNext()) {
                result->next();
                const DataObject& paramData = result->data(0);
                port.setParameterDependency(paramData.stringValue());
            }
            delete result;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            assert(false);
        }
    }
}

/**
 * Adds external ports to the given RF implementation which is the
 * implementation of the RF entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the RF implementation entry.
 */
void
HDBManager::addRFExternalPortsToImplementation(
    RFImplementation& implementation,
    RowID entryID) const {

    if (!dbConnection_->tableExistsInDB("rf_external_port")) {
        return;
    }

    RelationalDBQueryResult* extPortData = NULL;
    try {
        extPortData = dbConnection_->query(rfExternalPortsByIDQuery(entryID));
    } catch (const Exception& e) {
        assert(false);
    }

    int extPortNameColumn = extPortData->column("rf_external_port.name");
    int directionColumn = extPortData->column(
        "rf_external_port.direction");
    int extPortWidthFormulaColumn = extPortData->column(
        "rf_external_port.width_formula");
    int descriptionColumn = extPortData->column(
        "rf_external_port.description");

    while (extPortData->hasNext()) {
        extPortData->next();
        const DataObject& nameData = extPortData->data(
            extPortNameColumn);
        const DataObject& directionData = extPortData->data(
            directionColumn);
        const DataObject& widthFormulaData = extPortData->data(
            extPortWidthFormulaColumn);
        const DataObject& descriptionData = extPortData->data(
            descriptionColumn);

        string name = nameData.stringValue();
        string widthFormula = widthFormulaData.stringValue();
        string description = descriptionData.stringValue();

        Direction direction;
        if (directionData.stringValue() == IN_DIRECTION) {
            direction = IN;
        } else if (directionData.stringValue() == OUT_DIRECTION) {
            direction = OUT;
        } else {
            assert(directionData.stringValue() == BIDIR_DIRECTION);
            direction = BIDIR;
        }

        new RFExternalPort(
            name, direction, widthFormula, description, implementation);
    }

    delete extPortData;
    extPortData = NULL;

    // add parameter dependencies
    if (!dbConnection_->tableExistsInDB("rf_ext_port_parameter_dependency")) {
        return;
    }

    for (int i = 0; i < implementation.externalPortCount(); i++) {
        RFExternalPort& port = implementation.externalPort(i);
        try {
            RelationalDBQueryResult* result = dbConnection_->query(
                std::string(
                    "SELECT rf_implementation_parameter.name FROM "
                    "rf_implementation_parameter, rf_external_port, "
                    "rf_ext_port_parameter_dependency, rf_implementation "
                    "WHERE rf_implementation.rf=" +
                    Conversion::toString(entryID) +
                    " AND rf_external_port.rf_impl=rf_implementation.id AND "
                    "rf_external_port.name=\"" + port.name() +
                    "\" AND rf_ext_port_parameter_dependency.port="
                    "rf_external_port.id AND rf_implementation_parameter.id="
                    "rf_ext_port_parameter_dependency.parameter;"));
            while (result->hasNext()) {
                result->next();
                const DataObject& paramData = result->data(0);
                port.setParameterDependency(paramData.stringValue());
            }
            delete result;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            assert(false);
        }
    }
}


/**
 * Adds parameters to the given FU implementation which is the implementation
 * of the FU entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the FU entry.
 */
void
HDBManager::addFUParametersToImplementation(
    FUImplementation& implementation,
    RowID entryID) const {

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            fuImplementationParametersByIDQuery(entryID));
    } catch (const Exception&) {
        assert(false);
    }

    while (result->hasNext()) {
        result->next();
        const DataObject& nameData = result->data("name");
        const DataObject& typeData = result->data("type");
        const DataObject& valueData = result->data("value");
        string name = nameData.stringValue();
        string type = typeData.stringValue();
        string value = valueData.stringValue();
        implementation.addParameter(name, type, value);
    }
    delete result;
}


/**
 * Adds parameters to the given RF implementation which is the implementation
 * of the RF entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the RF entry.
 */
void
HDBManager::addRFParametersToImplementation(
    RFImplementation& implementation,
    RowID entryID) const {

    if (!dbConnection_->tableExistsInDB("rf_implementation_parameter")) {
        // Add implicit parameters: size and width parameters if older
        // hdb is opened.
        if (implementation.widthParameter() != "") {
            implementation.addParameter(implementation.widthParameter(),
                "integer", "");
        }
        if (implementation.sizeParameter() != "") {
            implementation.addParameter(implementation.sizeParameter(),
                "integer", "");
        }
        return;
    }

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(
            rfImplementationParametersByIDQuery(entryID));
    } catch (const Exception&) {
        assert(false);
    }

    while (result->hasNext()) {
        result->next();
        const DataObject& nameData = result->data("name");
        const DataObject& typeData = result->data("type");
        const DataObject& valueData = result->data("value");
        string name = nameData.stringValue();
        string type = typeData.stringValue();
        string value = valueData.stringValue();
        implementation.addParameter(name, type, value);
    }

    // If RF implementation's size and width parameter dependencies do not have
    // parameter defined, add default parameters for them.
    if (implementation.widthParameter() != "" &&
        !implementation.hasParameter(implementation.widthParameter())) {
        implementation.addParameter(implementation.widthParameter(),
            "integer", "");
        implementation.addParameter(implementation.sizeParameter(),
            "integer", "");
    }
    if (implementation.sizeParameter() != "" &&
        !implementation.hasParameter(implementation.sizeParameter())) {
        implementation.addParameter(implementation.sizeParameter(),
            "integer", "");
    }
    delete result;
}


/**
 * Adds the block implementation files to the given FU implementation which
 * is the implementation of the FU entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the FU entry.
 */
void
HDBManager::addBlockImplementationFiles(
    FUImplementation& implementation,
    RowID entryID) const {

    RelationalDBQueryResult* sourceFileData = NULL;
    try {
        sourceFileData = dbConnection_->query(
            fuSourceFilesByIDQuery(entryID));
    } catch (const Exception&) {
        assert(false);
    }
    
    int fileColumn = sourceFileData->column("block_source_file.file");
    int formatColumn = sourceFileData->column("format.format");
    
    while (sourceFileData->hasNext()) {
        sourceFileData->next();
        const DataObject& fileData = sourceFileData->data(fileColumn);
        const DataObject& formatData = sourceFileData->data(
            formatColumn);
        BlockImplementationFile::Format format = fileFormat(
            formatData.stringValue());
        BlockImplementationFile* file = new BlockImplementationFile(
            fileData.stringValue(), format);
        implementation.addImplementationFile(file);
    }
    
    delete sourceFileData;
    sourceFileData = NULL;
}


/**
 * Adds data ports to the given RF implementation which is the implementation
 * of the RF entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the RF entry.
 */
void
HDBManager::addDataPortsToImplementation(
    RFImplementation& implementation,
    RowID entryID) const {

    // obtain port data from HDB and add ports to RF implementation
    RelationalDBQueryResult* portData = NULL;
    try {
        portData = dbConnection_->query(
            rfImplementationDataPortsByIDQuery(entryID));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
    
    int portNameColumn = portData->column("name");
    int directionColumn = portData->column("direction");
    int loadPortColumn = portData->column("load_port");
    int opcodePortColumn = portData->column("opcode_port");
    int opcodePortFormulaColumn = portData->column(
        "opcode_port_width_formula");
    assert(portNameColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(directionColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(loadPortColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(opcodePortColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
    assert(
        opcodePortFormulaColumn != RelationalDBQueryResult::UNKNOWN_INDEX);
        
    while (portData->hasNext()) {
        portData->next();
        const DataObject& portNameData = portData->data(portNameColumn);
        const DataObject& directionData = portData->data(
            directionColumn);
        const DataObject& loadPortData = portData->data(loadPortColumn);
        const DataObject& opcodePortData = portData->data(
            opcodePortColumn);
        const DataObject& opcodePortFormulaData = portData->data(
            opcodePortFormulaColumn);
        Direction direction;
        if (directionData.stringValue() == IN_DIRECTION) {
            direction = IN;
        } else if (directionData.stringValue() == OUT_DIRECTION) {
            direction = OUT;
        } else if (directionData.stringValue() == BIDIR_DIRECTION) {
            direction = BIDIR;
        } else {
            assert(false);
        }
        new RFPortImplementation(
            portNameData.stringValue(), direction, 
            loadPortData.stringValue(), opcodePortData.stringValue(),
            opcodePortFormulaData.stringValue(), implementation);
    }
    
    delete portData;
    portData = NULL;
}


/**
 * Adds the block implementation files to the given RF implementation which
 * is the implementation of the RF entry that has the given ID.
 *
 * @param implementation The implementation.
 * @param entryID ID of the RF entry.
 */
void
HDBManager::addBlockImplementationFiles(
    RFImplementation& implementation,
    RowID entryID) const {

    RelationalDBQueryResult* result = NULL;
    try {
        result = dbConnection_->query(rfSourceFilesByIDQuery(entryID));
    } catch (const Exception&) {
        assert(false);
    }

    int fileColumn = result->column("block_source_file.file");
    int formatColumn = result->column("format.format");

    while (result->hasNext()) {
        result->next();
        const DataObject& fileData = result->data(fileColumn);
        const DataObject& formatData = result->data(formatColumn);
        BlockImplementationFile::Format format = fileFormat(
            formatData.stringValue());
        BlockImplementationFile* file = new BlockImplementationFile(
            fileData.stringValue(), format);
        implementation.addImplementationFile(file);
    }

    delete result;
    result = NULL;
}


/**
 * Removes the cost estimation data that has the given ID.
 *
 * @param id ID of the cost estimation data.
 */
void
HDBManager::removeCostEstimationData(RowID id) const {   
    try {
        dbConnection_->updateQuery(
            std::string(
                "DELETE FROM cost_estimation_data "
                "WHERE id=" + Conversion::toString(id) + ";"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Adds the given block implementation file to the HDB.
 *
 * @param file The file to add.
 */
void
HDBManager::addBlockImplementationFileToHDB(
    const BlockImplementationFile& file) const {
    if (!containsImplementationFile(file.pathToFile())) {
        dbConnection_->updateQuery(
            std::string(
                "INSERT INTO block_source_file(id,file,format) "
                "VALUES(NULL,\"" + file.pathToFile() +
                "\",(SELECT id FROM format WHERE format=\"" +
                formatString(file.format()) + "\"));"));
    }
}

/**
 * Checks whether the given FU has a mathing architecture with the given FU 
 * architecture instance.
 *
 * @param fu The function unit.
 * @param arch The FU architecture.
 * @return True if the architectures match, otherwise false.
 */
bool
HDBManager::isMatchingArchitecture(
    const TTAMachine::FunctionUnit& fu,
    const FUArchitecture& arch) {

    if (fu.operationCount() != arch.architecture().operationCount()) {
        return false;
    }

    std::map<const FUPort*, const FUPort*> portMap;
    for (int i = 0; i < fu.operationPortCount(); i++) {
        portMap.insert(
            std::pair<const FUPort*, const FUPort*>(
                fu.operationPort(i), NULL));
    }
    
    PipelineElementUsageTable plineElementUsages;

    for (int i = 0; i < fu.operationCount(); i++) {
        HWOperation* operation = fu.operation(i);
        if (!arch.architecture().hasOperation(operation->name())) {
            return false;
        }
        HWOperation* archOp = arch.architecture().operation(
            operation->name());
        if (operation->latency() != archOp->latency()) {
            return false;
        }

        // check operand bindings
        for (int i = 0; i < fu.operationPortCount(); i++) {
            FUPort* port = fu.operationPort(i);
            if (operation->isBound(*port)) {
                int io = operation->io(*port);
                FUPort* samePort = archOp->port(io);
                if (samePort == NULL) {
                    return false;
                }
                const FUPort* existingSamePort =
                    MapTools::valueForKey<const FUPort*>(portMap, port);
                if (existingSamePort != NULL &&
                    existingSamePort != samePort) {
                    return false;
                }
                
                // check the width of the ports
                if (!arch.hasParameterizedWidth(samePort->name()) && 
                    samePort->width() != port->width()) {
                    return false;
                }

                if (port->isOpcodeSetting() != samePort->isOpcodeSetting() ||
                    port->isTriggering() != samePort->isTriggering()) {
                    return false;
                }
                portMap.erase(port);
                portMap.insert(
                    std::pair<const FUPort*, const FUPort*>(port, samePort));
            }
        }

        // check operation pipeline
        ExecutionPipeline* opPipeline =  operation->pipeline();
        ExecutionPipeline* archOpPipeline = archOp->pipeline();
        for (int cycle = 0; cycle < operation->latency(); cycle++) {
            ExecutionPipeline::OperandSet written1 = 
                opPipeline->writtenOperands(cycle);
            ExecutionPipeline::OperandSet written2 = 
                archOpPipeline->writtenOperands(cycle);
            if (written1 != written2) {
                return false;
            }
            ExecutionPipeline::OperandSet read1 = 
                opPipeline->readOperands(cycle);
            ExecutionPipeline::OperandSet read2 = 
                archOpPipeline->readOperands(cycle);
            if (read1 != read2) {
                return false;
            }

            PipelineElementUsage usage;
            for (int i = 0; i < fu.pipelineElementCount(); i++) {
                const PipelineElement* elem = fu.pipelineElement(i);
                if (opPipeline->isResourceUsed(elem->name(),cycle)) {
                    usage.usage1.insert(elem);
                }
            }

            for (int i = 0; i < arch.architecture().pipelineElementCount();
                 i++) {
                const PipelineElement* elem =
                    arch.architecture().pipelineElement(i);
                if (archOpPipeline->isResourceUsed(elem->name(), cycle)) {
                    usage.usage2.insert(elem);
                }
            }

            plineElementUsages.push_back(usage);
        }
    }

    return areCompatiblePipelines(plineElementUsages);
}


/**
 * Checks whether the pipeline element usages of the given table are
 * compatible.
 *
 * They are compatible if the first pipeline is more restrictive or
 * equal to the second pipeline.
 *
 * @param table The table that describes the pipeline usages.
 * @return True if the pipelines are compatible, otherwise false.
 */
bool
HDBManager::areCompatiblePipelines(const PipelineElementUsageTable& table) {
    
    for (size_t i = 0; i < table.size(); i++) {
        std::set<const PipelineElement*> usedResources1 = table[i].usage1;
        // create a set of vector indices which mean what stages cannot be 
        // executed at the same time
        std::set<size_t> illegalStages1;
        for (size_t usageIndex = 0; usageIndex < table.size();
             usageIndex++) {
            if (usageIndex == i) {
                continue;
            }
            std::set<const PipelineElement*> resources = 
                table[usageIndex].usage1;
            std::set<const PipelineElement*> intersect;
            SetTools::intersection(usedResources1, resources, intersect);
            if (!intersect.empty()) {
                illegalStages1.insert(usageIndex);
            }
        }

        // create a similar vector of the other pipeline
        std::set<const PipelineElement*> usedResources2 = table[i].usage2;
        std::set<size_t> illegalStages2;
        for (size_t usageIndex = 0; usageIndex < table.size();
             usageIndex++) {
            if (usageIndex == i) {
                continue;
            }
            std::set<const PipelineElement*> resources = 
                table[usageIndex].usage2;
            std::set<const PipelineElement*> intersect;
            SetTools::intersection(usedResources2, resources, intersect);
            if (!intersect.empty()) {
                illegalStages2.insert(usageIndex);
            }
        }

        std::set<size_t> difference;
        AssocTools::difference(illegalStages2, illegalStages1, difference);
        if (!difference.empty()) {
            return false;
        }
    }

    return true;
}


/**
 * Inserts the supported formats to the format table.
 *
 * @param connection The connection used when inserting the formats.
 */
void
HDBManager::insertFileFormats(RelationalDBConnection& connection) {
    try {
        connection.updateQuery(
            std::string(
                "INSERT INTO format(id,format) VALUES(1,\"" +
                VHDL_FORMAT + "\");"));
        connection.updateQuery(
            std::string(
                "INSERT INTO format(id,format) VALUES(2,\"" +
                VERILOG_FORMAT + "\");"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}


/**
 * Returns the format corresponding to the given string which is stored in
 * the database.
 *
 * @param formatString The format string stored in the database.
 * @return The format.
 */
BlockImplementationFile::Format
HDBManager::fileFormat(const std::string& formatString) {
    if (formatString == VHDL_FORMAT) {
        return BlockImplementationFile::VHDL;
    } else if (formatString == VERILOG_FORMAT) {
         return BlockImplementationFile::Verilog;
    } else if (formatString == VHDL_SIM_FORMAT) {
         return BlockImplementationFile::VHDLsim;
    } else if (formatString == VERILOG_SIM_FORMAT) {
         return BlockImplementationFile::Verilogsim;
    }
    assert(false);
    // dummy return to avoid whining with some compilers
    return BlockImplementationFile::VHDL;
}


/**
 * Returns the string used to represent the given file format in HDB.
 *
 * @param format The format.
 * @return The string.
 */
std::string
HDBManager::formatString(BlockImplementationFile::Format format) {
    if (format == BlockImplementationFile::VHDL) {
        return VHDL_FORMAT;
    } else if (format == BlockImplementationFile::Verilog) {
       return VERILOG_FORMAT;
    } else if (format == BlockImplementationFile::VHDLsim) {
       return VHDL_SIM_FORMAT;
    } else if (format == BlockImplementationFile::Verilogsim) {
       return VERILOG_SIM_FORMAT;
    }

    // dummy return to avoid compiler whining
    assert(false);
    return "";
}


/**
 * Returns the string used to represent the given direction in HDB.
 *
 * @param direction The direction.
 * @return The string.
 */
std::string
HDBManager::directionString(HDB::Direction direction) {
    if (direction == HDB::IN) {
        return IN_DIRECTION;
    } else if (direction == HDB::OUT) {
        return OUT_DIRECTION;
    } else {
        return BIDIR_DIRECTION;
    }
}


/**
 * Creates an SQL query for getting the FU entry that has the given ID.
 *
 * The result set has fields {id, architecture, cost_function}.
 *
 * @param id ID of the entry.
 * @return The SQL query.
 */
std::string
HDBManager::fuEntryByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM fu "
        "WHERE fu.id=" + idString + ";";
    return query;
}


/**
 * Creates an SQL query for getting the RF entry that has the given ID.
 *
 * The result set has fields {id, architecture, cost_function}.
 *
 * @param id ID of the entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfEntryByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM rf "
        "WHERE rf.id=" + idString + ";";
    return query;
}


/**
 * Creates an SQL query for getting the Bus entry that has the given ID.
 *
 * The result set has fields {id}.
 *
 * @param id ID of the entry.
 * @return The SQL query.
 */
std::string
HDBManager::busEntryByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM bus "
        "WHERE bus.id=" + idString + ";";
    return query;
}


/**
 * Creates an SQL query for getting the Socket entry that has the given ID.
 *
 * The result set has fields {id}.
 *
 * @param id ID of the entry.
 * @return The SQL query.
 */
std::string
HDBManager::socketEntryByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM socket "
        "WHERE socket.id=" + idString + ";";
    return query;
}


/**
 * Creates an SQL query for getting the architecture of the FU entry that
 * has the given ID.
 *
 * The result set has all the fields of fu_architecture.
 *
 * @param id ID of the FU entry.
 */
std::string
HDBManager::fuArchitectureByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM fu, fu_architecture "
        "WHERE fu.id=" + idString + " AND"
        "      fu_architecture.id = fu.architecture;";
    return query;
}


/**
 * Creates an SQL query for getting the architectural ports and their 
 * operand bindings of the FU architecture that has the given ID.
 *
 * The result table has fields {fu_data_port.id, fu_data_port.triggers, 
 * fu_data_port.sets-opcode, fu_data_port.guard_support,
 * fu_data_port.width, operation.name, 
 * io_binding.io_number}
 *
 * @param id The ID of the FU architecture.
 */
std::string
HDBManager::fuPortsAndBindingsByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query = 
        "SELECT fu_data_port.id AS 'fu_data_port.id',"
        "       fu_data_port.triggers AS 'fu_data_port.triggers',"
        "       fu_data_port.sets_opcode AS 'fu_data_port.sets_opcode',"
        "       fu_data_port.guard_support AS 'fu_data_port.guard_support',"
        "       fu_data_port.width AS 'fu_data_port.width',"
        "       operation.name AS 'operation.name',"
        "       io_binding.io_number AS 'io_binding.io_number' "
        "FROM fu_data_port, io_binding, operation "
        "WHERE fu_data_port.fu_arch=" + idString + " AND"
        "      io_binding.port=fu_data_port.id AND"
        "      io_binding.operation=operation.id;";
    return query;
}


/**
 * Creates an SQL query for getting the IO usage data of pipelines of
 * operations contained in the FU architecture that has the given ID.
 *
 * The result table has fields {operation.name, io_usage.cycle, 
 * io_usage.io_number, io_usage.action}.
 *
 * @param id ID of the FU architecture in HDB.
 */
std::string
HDBManager::ioUsageDataByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT operation.name AS 'operation.name',"
        "       io_usage.cycle AS 'io_usage.cycle',"
        "       io_usage.io_number AS 'io_usage.io_number',"
        "       io_usage.action AS 'io_usage.action' "
        "FROM operation_pipeline, io_usage, operation "
        "WHERE operation_pipeline.fu_arch=" + idString + " AND"
        "      io_usage.pipeline=operation_pipeline.id AND"
        "      operation.id=operation_pipeline.operation;";
    return query;
}


/**
 * Creates an SQL query for getting resource usage data of pipelines
 * of the FU architecture that has the given ID.
 *
 * The result table has fields {operation.name,
 * pipeline_resource_usage.cycle, pipeline_resource.id}
 *
 * @param id ID of the FU architecture in HDB.
 */
std::string
HDBManager::resourceUsageDataByIDQuery(RowID id) {
    string idString  = Conversion::toString(id);
    string query =
        "SELECT operation.name AS 'operation.name',"
        "       pipeline_resource_usage.cycle AS "
        "          'pipeline_resource_usage.cycle',"
        "       pipeline_resource.id AS 'pipeline_resource.id' "
        "FROM pipeline_resource_usage, pipeline_resource, operation,"
        "     operation_pipeline "
        "WHERE operation_pipeline.fu_arch=" + idString + " AND"
        "      pipeline_resource_usage.pipeline=operation_pipeline.id AND"
        "      pipeline_resource.id = pipeline_resource_usage.resource AND"
        "      operation.id=operation_pipeline.operation;";
    return query;
}


/**
 * Creates an SQL query for getting FU implementation data of the FU that has
 * the given ID.
 *
 * The result table has fields {fu_implementation.id, fu_implementation.name, 
 * fu_implementation.opcode_port, fu_implementation.clk_port,
 * fu_implementation.rst_port, fu_implementation.glock_port,
 * fu_implementation.glock_req_port}.
 *
 * @param id ID of the FU entry in HDB.
 */
std::string
HDBManager::fuImplementationByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT fu_implementation.id AS 'fu_implementation.id',"
        "       fu_implementation.name AS 'fu_implementation.name',"
        "       fu_implementation.opcode_port AS "
        "'fu_implementation.opcode_port',"
        "       fu_implementation.clk_port AS 'fu_implementation.clk_port',"
        "       fu_implementation.rst_port AS 'fu_implementation.rst_port',"
        "       fu_implementation.glock_port AS "
        "           'fu_implementation.glock_port',"
        "       fu_implementation.glock_req_port AS "
        "           'fu_implementation.glock_req_port' "
        "FROM fu, fu_implementation "
        "WHERE fu.id=" + idString + " AND"
        "      fu_implementation.fu=fu.id;";
    return query;
}


/**
 * Creates an SQL query for getting operation code data of the FU that has the
 * given ID.
 *
 * The result table has fields {operation.name, opcode_map.opcode}.
 *
 * @param id ID of the FU entry in HDB.
 */
std::string
HDBManager::opcodesByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT operation.name AS 'operation.name',"
        "       opcode_map.opcode AS 'opcode_map.opcode' "
        "FROM fu, fu_implementation, operation, opcode_map "
        "WHERE fu.id=" + idString + " AND"
        "      fu_implementation.fu=fu.id AND"
        "      opcode_map.fu_impl=fu_implementation.id AND"
        "      operation.id=opcode_map.operation;";
    return query;
}


/**
 * Creates an SQL qury for getting data port data of implementation of
 * the FU that has the given ID.
 *
 * The result table has fields {fu_port_map.name,
 * fu_port_map.width_formula, fu_port_map.load_port,
 * fu_port_map.guard_port}
 *
 * @param id ID of the FU entry in HDB.
 */
std::string
HDBManager::fuImplementationDataPortsByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT fu_port_map.name AS 'fu_port_map.name',"
        "       fu_port_map.width_formula AS 'fu_port_map.width_formula',"
        "       fu_port_map.load_port AS 'fu_port_map.load_port',"
        "       fu_port_map.guard_port AS 'fu_port_map.guard_port' "
        "FROM fu, fu_port_map, fu_implementation "
        "WHERE fu.id=" + idString + " AND"
        "      fu_implementation.fu=fu.id AND"
        "      fu_port_map.fu_impl=fu_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting external port data of implementation of
 * the FU that has the given ID.
 *
 * The result table has fields {fu_external_port.name,
 * fu_external_port.direction, fu_external_port.width_formula,
 * fu_external_port.description}.
 *
 * @param id ID of the FU entry in HDB.
 * @return The SQL query.
 */
std::string
HDBManager::fuExternalPortsByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT fu_external_port.name AS 'fu_external_port.name',"
        "       fu_external_port.direction AS 'fu_external_port.direction',"
        "       fu_external_port.width_formula AS "
        "           'fu_external_port.width_formula',"
        "       fu_external_port.description AS "
        "           'fu_external_port.description' "
        "FROM fu, fu_implementation, fu_external_port "
        "WHERE fu.id=" + idString + " AND"
        "      fu_implementation.fu=fu.id AND"
        "      fu_external_port.fu_impl=fu_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting external port data of implementation of
 * the RF that has the given ID.
 *
 * The result table has fields {rf_external_port.name,
 * rf_external_port.direction, rf_external_port.width_formula,
 * rf_external_port.description}.
 *
 * @param id ID of the RF entry in HDB.
 * @return The SQL query.
 */
std::string
HDBManager::rfExternalPortsByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT rf_external_port.name AS 'rf_external_port.name',"
        "       rf_external_port.direction AS 'rf_external_port.direction',"
        "       rf_external_port.width_formula AS "
        "           'rf_external_port.width_formula',"
        "       rf_external_port.description AS "
        "           'rf_external_port.description' "
        "FROM rf, rf_implementation, rf_external_port "
        "WHERE rf.id=" + idString + " AND"
        "      rf_implementation.rf=rf.id AND"
        "      rf_external_port.rf_impl=rf_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting the parameters of the implementation
 * of the FU that has the given ID.
 *
 * The result table has fields {name, type, value}.
 *
 * @param id ID of the FU implementation entry.
 * @return The SQL query.
 */
std::string
HDBManager::fuImplementationParametersByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT fu_implementation_parameter.name AS 'name',"
        "       fu_implementation_parameter.type AS 'type',"
        "       fu_implementation_parameter.value AS 'value' "
        "FROM fu_implementation, fu_implementation_parameter "
        "WHERE fu_implementation.fu=" + idString + " AND"
        "      fu_implementation_parameter.fu_impl=fu_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting the parameters of the implementation
 * of the RF that has the given ID.
 *
 * The result table has fields {name, type, value}.
 *
 * @param id ID of the RF entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfImplementationParametersByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT rf_implementation_parameter.name AS 'name',"
        "       rf_implementation_parameter.type AS 'type',"
        "       rf_implementation_parameter.value AS 'value' "
        "FROM rf_implementation, rf_implementation_parameter "
        "WHERE rf_implementation.rf=" + idString + " AND"
        "      rf_implementation_parameter.rf_impl=rf_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting io binding data of port that has
 * the given name.
 *
 * The result table has fields {operation.name, io_binding.io_number}.
 *
 * @param fuID ID of the FU entry.
 * @param portName Name of the implemented port.
 * @return The SQL query.
 */
std::string
HDBManager::fuPortBindingByNameQuery(
    RowID fuID,
    const std::string& portName) {

    string idString = Conversion::toString(fuID);
    string query =
        "SELECT operation.name AS 'operation.name',"
        "       io_binding.io_number AS 'io_binding.io_number' "
        "FROM operation, io_binding, fu_port_map, fu_implementation "
        "WHERE fu_implementation.fu=" + idString + " AND"
        "      fu_port_map.fu_impl=fu_implementation.id AND"
        "      fu_port_map.name='" + portName + "' AND"
        "      io_binding.port=fu_port_map.arch_port AND"
        "      operation.id=io_binding.operation;";
    return query;
}


/**
 * Creates an SQL query for getting the block source files of the FU
 * entry that has the given ID.
 *
 * The result table has fields {block_source_file.file, format.format}.
 *
 * @param id ID of the FU entry.
 * @return The SQL query.
 */
std::string
HDBManager::fuSourceFilesByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT block_source_file.file AS 'block_source_file.file',"
        "       format.format AS 'format.format' "
        "FROM block_source_file, fu_source_file, fu_implementation, format "
        "WHERE fu_implementation.fu=" + idString + " AND"
        "      fu_source_file.fu_impl=fu_implementation.id AND"
        "      block_source_file.id=fu_source_file.file AND"
        "      format.id=block_source_file.format;";
    return query;
}


/**
 * Creates an SQL query for getting the architecture data of the RF
 * architecture that has the given ID.
 *
 * The result table has all the fields of rf_architecture table.
 *
 * @param id ID of the RF architecture.
 * @return The SQL query.
 */
std::string
HDBManager::rfArchitectureByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT * "
        "FROM rf_architecture "
        "WHERE id=" + idString + ";";
    return query;
}


/**
 * Creates an SQL query for getting the implementation of the RF entry that
 * has the given ID.
 *
 * The result table has fields {id, name, size_param, width_param,
 * clk_port, rst_port, glock_port, guard_port}.
 *
 * @param id The ID of the RF entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfImplementationByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
            "SELECT id,"
            "       name,"
            "       size_param,"
            "       width_param,"
            "       clk_port,"
            "       rst_port,"
            "       glock_port,"
            "       guard_port "
            "FROM rf_implementation "
            "WHERE rf_implementation.rf=" + idString + ";";
    return query;
}

/**
 * Same as rfImplementationByIDQuery() bus has additional field for separate
 * address cycle.
 *
 * The result table has fields {id, name, size_param, width_param,
 * clk_port, rst_port, glock_port, guard_port, sac_param}.
 *
 * @param id The ID of the RF entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfImplementationByIDQuery2(RowID id) {
    string idString = Conversion::toString(id);
    string query =
            "SELECT id,"
            "       name,"
            "       size_param,"
            "       width_param,"
            "       clk_port,"
            "       rst_port,"
            "       glock_port,"
            "       guard_port, "
            "       sac_param "
            "FROM rf_implementation "
            "WHERE rf_implementation.rf=" + idString + ";";
    return query;
}

/**
 * Creates an SQL query for getting the data ports of the implementation of
 * the RF entry that has the given ID.
 *
 * The result table has fields {name, direction, load_port, opcode_port, 
 * opcode_port_width_formula}.
 *
 * @param id ID of the RF entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfImplementationDataPortsByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT rf_data_port.name AS 'name',"
        "       rf_data_port.direction AS 'direction',"
        "       rf_data_port.load_port AS 'load_port',"
        "       rf_data_port.opcode_port AS 'opcode_port',"
        "       rf_data_port.opcode_port_width_formula AS "
        "           'opcode_port_width_formula' "
        "FROM rf_data_port, rf_implementation "
        "WHERE rf_implementation.rf=" + idString + " AND"
        "      rf_data_port.rf_impl=rf_implementation.id;";
    return query;
}


/**
 * Creates an SQL query for getting the block implementation files of the
 * RF entry that has the given ID.
 *
 * The result table has fields {block_source_file.file, format.format}.
 *
 * @param id ID of the RF entry.
 * @return The SQL query.
 */
std::string
HDBManager::rfSourceFilesByIDQuery(RowID id) {
    string idString = Conversion::toString(id);
    string query =
        "SELECT block_source_file.file AS 'block_source_file.file',"
        "       format.format AS 'format.format' "
        "FROM block_source_file, format, rf_implementation, rf_source_file "
        "WHERE rf_implementation.rf=" + idString + " AND"
        "      rf_source_file.rf_impl=rf_implementation.id AND"
        "      block_source_file.id=rf_source_file.file AND"
        "      format.id=block_source_file.format;";
    return query;
}


/**
 * Returns cost estimation data with the given id.
 *
 * @param entryId Id of the cost estimation data entry.
 * @return The data.
 * @exception KeyNotFound If the HDB does not contain cost estimation 
 *                        data with the given arguments.
 */
CostEstimationData
HDBManager::costEstimationData(RowID entryId) const {
    // make the SQL query to obtain implementation data
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT value, cost_estimation_data.name AS data_name, "
                "       cost_function_plugin.id AS plugin_id, "
                "       fu_reference, rf_reference, bus_reference, "
                "       socket_reference "
                "FROM cost_estimation_data, cost_function_plugin "
                "WHERE cost_estimation_data.plugin_reference="
                "      cost_function_plugin.id AND "
                "      cost_estimation_data.id = ") +
            Conversion::toString(entryId);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);
    } 

    if (queryResult->hasNext()) {
        queryResult->next();

        std::string name = queryResult->data("data_name").stringValue();

        CostEstimationData data;
        data.setName(name);
        data.setValue(queryResult->data("value"));
        data.setPluginID(queryResult->data("plugin_id").integerValue());

        if (!queryResult->data("fu_reference").isNull()) {
            data.setFUReference(queryResult->data("fu_reference").integerValue());
        }
        if (!queryResult->data("rf_reference").isNull()) {
            data.setRFReference(queryResult->data("rf_reference").integerValue());
        }
        if (!queryResult->data("bus_reference").isNull()) {
            data.setBusReference(queryResult->data("bus_reference").integerValue());
        }
        if (!queryResult->data("socket_reference").isNull()) {
            data.setSocketReference(
                queryResult->data("socket_reference").integerValue());
        }

        delete queryResult;
        queryResult = NULL;

        return data;
    } else {
        delete queryResult;
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }    
    // silence compiler warning
    throw 1;
}

/**
 * Returns a set of cost estimation data IDs which reference the give FU
 * implementation.
 *
 * @param fuImplID ID of the FU implementation.
 * @return Set of cost estimation data IDs.
 */
std::set<RowID>
HDBManager::fuCostEstimationDataIDs(RowID fuImplID) const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT id "
                "FROM cost_estimation_data "
                "WHERE fu_reference = ") +
            Conversion::toString(fuImplID);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns a set of cost estimation data IDs which reference the give RF
 * implementation.
 *
 * @param rfImplID ID of the RF implementation.
 * @return Set of cost estimation data IDs.
 */
std::set<RowID>
HDBManager::rfCostEstimationDataIDs(RowID rfImplID) const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT id "
                "FROM cost_estimation_data "
                "WHERE rf_reference = ") +
            Conversion::toString(rfImplID);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns a set of cost estimation data IDs which reference the given
 * socket entry.
 *
 * @param socketID ID of the socket entry.
 * @return Set of cost estimation data IDs.
 */
std::set<RowID>
HDBManager::socketCostEstimationDataIDs(RowID socketID) const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT id "
                "FROM cost_estimation_data "
                "WHERE socket_reference = ") +
            Conversion::toString(socketID);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns a set of cost estimation data IDs which reference the given
 * bus entry.
 *
 * @param busID ID of the bus entry.
 * @return Set of cost estimation data IDs.
 */
std::set<RowID>
HDBManager::busCostEstimationDataIDs(RowID busID) const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT id "
                "FROM cost_estimation_data "
                "WHERE bus_reference = ") +
            Conversion::toString(busID);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns RowIDs of cost fucntion plugins in the HDB.
 *
 * @return All cost function plugin IDs.
 */
std::set<RowID>
HDBManager::costFunctionPluginIDs() const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            "SELECT id FROM cost_function_plugin";

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns cost estimation data IDs related to the given cost function plugin.
 *
 * @param pluginID ID of the cost function plugin.
 * @return IDs of the cost function plugin estimation data.
 */
std::set<RowID>
HDBManager::costFunctionPluginDataIDs(RowID pluginID) const {

    // make the SQL query to obtain IDs.
    RelationalDBQueryResult* queryResult = NULL;
    try {
        std::string theQuery =
            std::string(
                "SELECT id "
                "FROM cost_estimation_data "
                "WHERE plugin_reference = ") +
            Conversion::toString(pluginID);

        queryResult = dbConnection_->query(theQuery);

    } catch (const Exception& e) {
        // should not throw in any case
        debugLog(e.errorMessage());
        assert(false);        
    } 

    std::set<RowID> ids;

    while (queryResult->hasNext()) {
        queryResult->next();

        ids.insert(queryResult->data("id").integerValue());
    }

    delete queryResult;
    queryResult = NULL;
    return ids;
}

/**
 * Returns cost function plugin with the given ID.
 *
 * @param pluginID ID of the cost function plugin.
 * @return Cost function plugin with the given ID.
 * @exception Exception Throws if pluginID not found (KeyNotFound) or
 * illegal cost_function_plugin row found.
 */
CostFunctionPlugin*
HDBManager::costFunctionPluginByID(RowID pluginID) const {
    RelationalDBQueryResult* pluginData;

    std::string pluginDataQuery =
        "SELECT id, description, name, plugin_file_path, type "
        "       FROM cost_function_plugin WHERE id = ";

    pluginDataQuery += Conversion::toString(pluginID);

    try {
        pluginData = dbConnection_->query(pluginDataQuery);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    if (pluginData->hasNext()) {
        pluginData->next();
        
        int id = pluginData->data("id").integerValue();
        std::string name = pluginData->data("name").stringValue();
        std::string desc = pluginData->data("description").stringValue();
        std::string path = pluginData->data("plugin_file_path").stringValue();
        std::string typeStr = pluginData->data("type").stringValue();
        
        CostFunctionPlugin::CostFunctionPluginType type = 
            CostFunctionPlugin::COST_FU;
        if (typeStr == COST_PLUGIN_TYPE_FU) {
            type = CostFunctionPlugin::COST_FU;
        } else if (typeStr == COST_PLUGIN_TYPE_RF) {
            type = CostFunctionPlugin::COST_RF;
        } else if (typeStr == COST_PLUGIN_TYPE_DECOMP) {
            type = CostFunctionPlugin::COST_DECOMP;
        } else if (typeStr == COST_PLUGIN_TYPE_ICDEC) {
            type = CostFunctionPlugin::COST_ICDEC;
        } else {
            delete pluginData;
            InvalidData ex(
                __FILE__, __LINE__, __func__, 
                (boost::format("Illegal cost_function_plugin type %d.") %
                 type).str());
            throw ex;
        }

        delete pluginData;
        return new CostFunctionPlugin(id, desc, name, path, type);

    } else {
        delete pluginData;
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, 
            (boost::format("Cost function plugin with id %d not found.") %
             pluginID).str());
    }
}

/**
 * Adds cost estimation data to the HDB.
 *
 * @param data Cost estimation data to add.
 * @return Row ID of the added cost data.
 */
RowID
HDBManager::addCostEstimationData(const CostEstimationData& data) const {
    if (!data.hasName() || !data.hasValue() || !data.hasPluginID()) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    std::string query =
        std::string("INSERT INTO cost_estimation_data"
                    " (id, name, value, plugin_reference, "
                    "  fu_reference, rf_reference, "
                    "  bus_reference, socket_reference) VALUES (") +
        "NULL, '" + data.name()  + "', '" +
        data.value().stringValue() + "', " +
        Conversion::toString(data.pluginID()) + ", ";

    // FU Reference
    if (data.hasFUReference()) {
        if (!hasFUEntry(data.fuReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += Conversion::toString(data.fuReference());
        query += ", ";
    } else {
        query += "NULL, ";
    }

    // RF Reference
    if (data.hasRFReference()) {
        if (!hasRFEntry(data.rfReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += Conversion::toString(data.rfReference());
        query += ", ";
    } else {
        query += "NULL, ";
    }

    // Bus Reference
    if (data.hasBusReference()) {
        if (!hasBusEntry(data.busReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += Conversion::toString(data.busReference());
        query += ", ";
    } else {
        query += "NULL, ";
    }

    // Socket Reference
    if (data.hasSocketReference()) {
        if (!hasSocketEntry(data.socketReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += Conversion::toString(data.socketReference());
        query += ");";
    } else {
        query += "NULL);";
    }

    try {
        dbConnection_->updateQuery(query);
        return dbConnection_->lastInsertRowID();
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    // dummy return to avoid compiler whining
    assert(false);
    return 0;
}

/**
 * Function for querying cost estimation data from the HDB.
 *
 * Returns set of cost estiamtion data Row IDs that match the given
 * datas attributes that are set.
 *
 * @param match CostEstimationData which is matched to the HDB data.
 * @param useCompiledQueries if true use a compiled query instead of making a
 *        new one.
 * @param compiledQuery Pointer to a prepared query to be used.
 * @return Set of cost estimation data row IDs that match the query.
 */
std::set<RowID>
HDBManager::costEstimationDataIDs(
    const CostEstimationData& match, 
    bool /*useCompiledQueries*/,
    RelationalDBQueryResult* compiledQuery) const {

    std::string query = "";
    if (!compiledQuery) {
        createCostEstimatioDataIdsQuery(match, &query);
    } else {
        // only bind query variables
        createCostEstimatioDataIdsQuery(match, NULL, compiledQuery, NULL);
    }

    RelationalDBQueryResult* result = NULL;
    
    if (compiledQuery) {
        result = compiledQuery;
    } else {
        try {
            result = dbConnection_->query(query);
        } catch (const Exception& e) {
            debugLog(query);
            debugLog(e.errorMessage());
            assert(false);
        }
    }

    std::set<RowID> dataIDs;
    while (result->hasNext()) {
        result->next();
        dataIDs.insert(result->data(0).integerValue());
    }

    if (!compiledQuery) {
        delete result;
    } else {
        compiledQuery->reset();
    }

    return dataIDs;
}


/**
 * Creates or prepares the query for cost estimation data ids.
 *
 * @param match CostEstimationData which is matched to the HDB data.
 * @param query String variable where query is stored, null if no query is to
 *         be created.
 * @param compiledQuery Pointer to a prepared query to be used, null if not to
 *        be used.
 * @param queryHash Pointer to unique id variable to be created for the
 *        query, null if not to be created.
 * @param createBindableQuery If true query of a kind where variables can be
 *        binded is created, if false normal query with values is created.
 */
void
HDBManager::createCostEstimatioDataIdsQuery(
    const CostEstimationData& match, 
    std::string* query,
    RelationalDBQueryResult* compiledQuery,
    short int* queryHash,
    bool createBindableQuery) const {

    if (queryHash) {
        *queryHash = 0;
    }
    bool firstMatch = true;
    unsigned int count = 0;

    if (query) {
        *query = "SELECT id FROM cost_estimation_data WHERE ";
    }

    if (match.hasName()) {
        if (queryHash) {
            *queryHash |= 1;
        }

        if (compiledQuery) {
            compiledQuery->bindString(++count, match.name());
        }

        if (query) {
            firstMatch = false;
            *query += "name='";
            *query += createBindableQuery ? "?" : match.name();
            *query += "'";
        }
    }

    if (match.hasValue()) {
        if (queryHash) {
            *queryHash |= 2;
        }

        if (compiledQuery) {
            compiledQuery->bindString(++count, match.name());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "value='";
            *query += createBindableQuery ? "?" : match.name();
            *query += "'";
        }
    }

    if (match.hasPluginID()) {
        if (queryHash) {
            *queryHash |= 4;
        }

        if (compiledQuery) {
            compiledQuery->bindInt(++count, match.pluginID());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "plugin_reference = ";
            *query += createBindableQuery ? "?" : 
                Conversion::toString(match.pluginID());
        }
    }

    if (match.hasFUReference()) {
        if (queryHash) {
            *queryHash |= 8;
        }

        if (compiledQuery) {
            compiledQuery->bindInt(++count, match.fuReference());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "fu_reference = ";
            *query += createBindableQuery ? "?" : 
                Conversion::toString(match.fuReference());
        }
    }

    if (match.hasRFReference()) {
        if (queryHash) {
            *queryHash |= 16;
        }

        if (compiledQuery) {
            compiledQuery->bindInt(++count, match.rfReference());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "rf_reference = ";
            *query += createBindableQuery ? "?" : 
                Conversion::toString(match.rfReference());
        }
    }

    if (match.hasBusReference()) {
        if (queryHash) {
            *queryHash |= 32;
        }

        if (compiledQuery) {
            compiledQuery->bindInt(++count, match.rfReference());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "bus_reference = ";
            *query += createBindableQuery ? "?" : 
                Conversion::toString(match.busReference());
        }
    }

    if (match.hasSocketReference()) {
        if (queryHash) {
            *queryHash |= 64;
        }

        if (compiledQuery) {
            compiledQuery->bindInt(++count, match.rfReference());
        }

        if (query) {
            if (!firstMatch) *query += " AND ";
            firstMatch = false;
            *query += "socket_reference = ";
            *query += createBindableQuery ? "?" : 
                Conversion::toString(match.socketReference());
        }
    }

    if (query) {
        *query += ";";
    }
}


/**
 * Returns used database connection.
 */
RelationalDBConnection* 
HDBManager::getDBConnection() const {
    return dbConnection_;
}


/**
 * Updates cost estimation data in the HDB.
 *
 * @param id Row ID of the data to update.
 * @param data Updated data.
 */
void
HDBManager::modifyCostEstimationData(RowID id, const CostEstimationData& data) {
    if (!data.hasName() || !data.hasValue() || !data.hasPluginID()) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    if (!hasCostEstimationDataByID(id)) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    std::string query =
        std::string("UPDATE cost_estimation_data SET ") +
        " name='" + data.name() +
        "', value='" + data.value().stringValue() +
        "', plugin_reference=" + Conversion::toString(data.pluginID());

    // FU entry reference.
    if (data.hasFUReference()) {
        if (!hasFUEntry(data.fuReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += ", fu_reference=";
        query += Conversion::toString(data.fuReference());
    } else {
        query += ", fu_reference=NULL";
    }

    // RF entry reference.
    if (data.hasRFReference()) {
        if (!hasRFEntry(data.rfReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += ", rf_reference=";
        query += Conversion::toString(data.rfReference());
    } else {
        query += ", rf_reference=NULL";
    }

    // Bus entry reference
    if (data.hasBusReference()) {
        if (!hasBusEntry(data.busReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += ", bus_reference=";
        query += Conversion::toString(data.busReference());
    } else {
        query += ", bus_reference=NULL";
    }

    // Socket entry reference.
    if (data.hasSocketReference()) {
        if (!hasSocketEntry(data.socketReference())) {
            throw KeyNotFound(__FILE__, __LINE__, __func__);
        }
        query += ", socket_reference=";
        query += Conversion::toString(data.socketReference());
    } else {
        query += ", socket_reference=NULL";
    }

    query += " WHERE id=";
    query += Conversion::toString(id);
    query += ";";

    try {
        dbConnection_->updateQuery(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/**
 * Modifies cost estimation function plugin attributes.
 *
 * @param id RowID of the plugin to modify.
 * @param plugin Modified plugin (ID is ignored).
 * @exception InvalidData Throws if the given plugin was invalid.
 * @exception KeyNotFound Throws if no cost function plugin was found with
 * given RowID.
 */
void
HDBManager::modifyCostFunctionPlugin(
    RowID id, const CostFunctionPlugin& plugin) {
    if (plugin.name() == "") {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }
    
    if (!hasCostFunctionPluginByID(id)) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    string type = "";
    switch (plugin.type()) {
    case CostFunctionPlugin::COST_FU:
        type = COST_PLUGIN_TYPE_FU;
        break;
    case CostFunctionPlugin::COST_RF:
        type = COST_PLUGIN_TYPE_RF;
        break;
    case CostFunctionPlugin::COST_DECOMP:            
        type = COST_PLUGIN_TYPE_DECOMP;
        break;
    case CostFunctionPlugin::COST_ICDEC:
        type = COST_PLUGIN_TYPE_ICDEC;
        break;
    default:
        InvalidData ex(
            __FILE__, __LINE__, __func__, 
            (boost::format("Illegal cost_function_plugin type %d.") %
             type).str());
        throw ex;
        break;
    }
    
    std::string query =
        std::string("UPDATE cost_function_plugin SET ") +
        " name='" + plugin.name() +
        "', description='" + plugin.description() +
        "', plugin_file_path='" + plugin.pluginFilePath() +
        "', type='" + type + "'";
    
    query += " WHERE id=";
    query += Conversion::toString(id);
    query += ";";
    
    try {
        dbConnection_->updateQuery(query);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }
}

/** 
 * Returns block source file names 
 * 
 * @return List of block source file names.
 */
std::list<std::string>
HDBManager::blockSourceFile() {
    
    RelationalDBQueryResult* queryResult;
    try {
        queryResult = dbConnection_->query(
            std::string("SELECT * FROM block_source_file"));
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        assert(false);
    }

    std::list<std::string> files;
    while (queryResult->hasNext()) {
        queryResult->next(); 
        files.push_back(queryResult->data(1).stringValue());
    }

    delete queryResult;
    return files;
}

} // namespace HDB
