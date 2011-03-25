/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file Environment.cc
 *
 * Definition of Environment class.
 *
 * *
 * @todo DISTRIBUTED_VERSION compile time variable does not make much sense.
 *
 * Currently the paths include the src dir first and then from the 
 * installation path which might cause strange behavior when running
 * a differing (usually old) installed version than the src dir has.
 *
 * In case DISTRIBUTED_VERSION is 'true' then the src dirs are excluded
 * from the search path for efficiency etc. One way to fix this is
 * to make DISTRIBUTED_VERSION a function call which determines somehow
 * if the binary is executed from the build path or from the installation
 * prefix. Anyways, it works fine as long as the developers know about
 * this issue.
 *
 *
 * @author Atte Oksman 2003 (oksman-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007,2010 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 *
 * @note reviewed 19 May 2004 by ao, jn, ml, am
 * @note rating: red
 */

#include <vector>
#include <string>

#include "Environment.hh"
#include "tce_config.h"
#include "FileSystem.hh"
#include "StringTools.hh"
#include "VectorTools.hh"
#include "MapTools.hh"

using std::vector;
using std::string;

// initializing the static class member variables
const string Environment::SCHEMA_DIR_NAME_ = "data";
const string Environment::DATA_DIR_NAME_ = "data";
const string Environment::BITMAPS_DIR_NAME_ = "bitmaps";
const string Environment::MAN_DIR_NAME_ = "man";
const string Environment::CONF_DIR_NAME_ = "conf";
const string Environment::ICON_DIR_NAME_ = "icons";
const string Environment::ERROR_LOG_FILE_NAME_ = "error_log.log";
bool Environment::initialized_ = false;
vector<string> Environment::schemaPaths_;
vector<string> Environment::dataPaths_;
vector<string> Environment::bitmapsPaths_;
vector<string> Environment::manPaths_;
vector<string> Environment::confPaths_;
vector<string> Environment::iconPaths_;
vector<string> Environment::errorPaths_;
vector<string> Environment::pathsToUse_;
string Environment::newSchemaFileDir_ = "";
string Environment::newDataFileDir_ = "";
string Environment::newBitmapsFileDir_ = "";
string Environment::newIconFileDir_ = "";
string Environment::newManFileDir_ = "";
string Environment::newConfFile_ = "";
string Environment::newErrorLogFileDir_ = "";

const string Environment::PDF_MANUAL_INSTALLED = "manual/TCE_manual.pdf";
const string Environment::PDF_MANUAL_SRC = "manual/TCE_manual.pdf";

const string Environment::MINIMAL_ADF_INSTALLED = "data/mach/minimal.adf";
const string Environment::MINIMAL_ADF_SRC = "data/mach/minimal.adf";

// macro definitions (maybe relocated later to config.h)
#define INSTALLATION_DIR "/share/tce/"


/**
 * Initializes the program environment.
 *
 * Initializes all of the source directories the program needs.
 */
void
Environment::initialize() {

    if (initialized_) {
        return;
    }

    errorPaths_.push_back(
        FileSystem::homeDirectory() +
        FileSystem::DIRECTORY_SEPARATOR + string(".tce"));

    if (!DISTRIBUTED_VERSION) {
        schemaPaths_.push_back(string(TCE_SRC_ROOT));
        iconPaths_.push_back(string(TCE_SRC_ROOT));
        dataPaths_.push_back(string(TCE_SRC_ROOT));
        bitmapsPaths_.push_back(string(TCE_SRC_ROOT));
        manPaths_.push_back(
            string(TCE_SRC_ROOT) +
            FileSystem::DIRECTORY_SEPARATOR + string("doc"));
        confPaths_.push_back(string(TCE_SRC_ROOT));
    }

    string instDir = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR);
    schemaPaths_.push_back(instDir);
    iconPaths_.push_back(instDir);
    dataPaths_.push_back(instDir);
    bitmapsPaths_.push_back(instDir);
    manPaths_.push_back(instDir);
    confPaths_.push_back(instDir);
    initialized_ = true;
}


/**
 * Tries to locate the directory where the XML Schema file is stored.
 *
 * Returns the first absolute path of the directory where the file is found.
 * If directory for new data file is given, it is created and returned.
 * If the directory can not be created, an empty string is returned.
 *
 * @param prog The program whose Schema is searched.
 * @return The path of the Schema directory.
 */
string
Environment::schemaDirPath(const std::string& prog) {
    initialize();
    string path = SCHEMA_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR + prog;
    pathsToUse_.assign(schemaPaths_.begin(), schemaPaths_.end());
    string schemaPath = pathTo(path);
    if (schemaPath == "" && newSchemaFileDir_ != "") {
        if (FileSystem::createDirectory(newSchemaFileDir_)) {
            return newSchemaFileDir_;
        }
    }
    return schemaPath;
}


/**
 * Tries to locate the directory where the application data is stored.
 *
 * Returns the first absolute path of the directory where the data is found.
 * If directory is not found and directory for new data file is given, it
 * is created. If creation fails, an empty string is returned.
 *
 * @param prog The program whose data directory is searched.
 * @return The path of data directory or an empty string.
 */
string
Environment::dataDirPath(const std::string& prog) {
    initialize();
    string path = DATA_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR + prog;
    pathsToUse_.assign(dataPaths_.begin(), dataPaths_.end());
    string dataPath = pathTo(path);
    if (dataPath == "" && newDataFileDir_ != "") {
        if (FileSystem::createDirectory(newDataFileDir_)) {
            return newDataFileDir_;
        }
    }
    return dataPath;
}


/**
 * Tries to locate the directory where the application bitmaps are stored.
 *
 * Returns the first absolute path of the directory where the file is found.
 * If bitmaps directory is not found and a new directory is given, it is
 * created. If creation fails, an empty string is returned.
 *
 * @param prog The program whose bitmaps directory are searched.
 * @return The path of the bitmaps directory.
 */
string
Environment::bitmapsDirPath(const std::string& prog) {
    initialize();
    string path = DATA_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR +
        BITMAPS_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR + prog;
    pathsToUse_.assign(bitmapsPaths_.begin(), bitmapsPaths_.end());
    string bitmapsPath = pathTo(path);
    if (bitmapsPath == "" && newBitmapsFileDir_ != "") {
        if (FileSystem::createDirectory(newBitmapsFileDir_)) {
            return newBitmapsFileDir_;
        }
    }
    return bitmapsPath;
}

/**
 * Tries to locate the directory where common toolbar icon files are stored.
 *
 * Returns the first absolute path of the directory where the file is found.
 * If icons directory is not found and a new directory is given, it is
 * created. If creation fails, an empty string is returned.
 *
 * @return The path of the icons directory.
 */
string
Environment::iconDirPath() {
    initialize();
    string path = DATA_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR +
        ICON_DIR_NAME_;
    pathsToUse_.assign(bitmapsPaths_.begin(), bitmapsPaths_.end());
    string iconPath = pathTo(path);
    if (iconPath == "" && newIconFileDir_ != "") {
        if (FileSystem::createDirectory(newIconFileDir_)) {
            return newIconFileDir_;
        }
    }
    return iconPath;
}

/**
 * Tries to locate the directory where the system's manual is stored.
 *
 * Returns the first absolute path of the directory where the file is found.
 * If no directory is found and a new directory for manual is given, it is
 * created. If creation fails, an empty string is returned.
 *
 * @param prog The program whose manual directory is searched.
 * @return The path of the manual directory.
 */
string
Environment::manDirPath(const std::string& prog) {
    initialize();
    string path = MAN_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR + prog;
    pathsToUse_.assign(manPaths_.begin(), manPaths_.end());
    string manPath = pathTo(path);
    if (manPath == "" && newManFileDir_ != "") {
        if (FileSystem::createDirectory(newManFileDir_)) {
            return newManFileDir_;
        }
    }
    return manPath;
}


/**
 * Tries to locate the application configuration file's directory.
 *
 * Returns the absolute path of the first found directory. If the
 * directory is not found and new directory for configuration is given,
 * it is created. If creation fails, an empty string is returned.
 *
 * @param fileName The name of the configuration file.
 * @return The path of the configuration file.
 */
string
Environment::confDirPath(const std::string& fileName) {
    initialize();
    string path = CONF_DIR_NAME_ + FileSystem::DIRECTORY_SEPARATOR + fileName;
    pathsToUse_.assign(confPaths_.begin(), confPaths_.end());
    string confPath = pathTo(path);
    if (confPath == "" && newConfFile_ != "") {
        string toCreate = "";
        if (!DISTRIBUTED_VERSION) {
            toCreate = 
                string(TCE_SRC_ROOT) + 
                FileSystem::DIRECTORY_SEPARATOR + CONF_DIR_NAME_ +
                FileSystem::DIRECTORY_SEPARATOR + newConfFile_;
        } else {
            toCreate = 
                string(TCE_INSTALLATION_ROOT) +
                string(INSTALLATION_DIR) + FileSystem::DIRECTORY_SEPARATOR +
                newConfFile_;
        }
		if (FileSystem::createFile(toCreate)) {
            return toCreate;
        }
    }
    return confPath;
}


/**
 * Tries to locate the error log file on system.
 *
 * Returns the first absolute path where the file is found. If the file is
 * not found and directory for the error log file is given, the directory
 * and the error log file is created. If creation fails, an empty string is
 * returned.
 *
 * @return The path of the error log file.
 */
string
Environment::errorLogFilePath() {
    initialize();
    pathsToUse_.assign(errorPaths_.begin(), errorPaths_.end());
    string errorPath = pathTo(ERROR_LOG_FILE_NAME_);
    if (errorPath == "" && newErrorLogFileDir_ != "") {
        if (FileSystem::createDirectory(newErrorLogFileDir_)) {
            string filePath = newErrorLogFileDir_ +
                FileSystem::DIRECTORY_SEPARATOR + ERROR_LOG_FILE_NAME_;
            if (FileSystem::createFile(filePath)) {
                return filePath;
            }
        }
    }
    return errorPath;
}

/**
 * Returns path where installable tce include headers reside.
 *
 * The list returned contains both post-installation paths and the paths to
 * the original header files found in the source tree.
 *
 * @return Paths where tce installable headers files reside.
 */
std::vector<std::string>
Environment::includeDirPaths() {
    vector<string> includes;
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    const std::string ROOT = string(TCE_SRC_ROOT);
    const std::string BASE = ROOT + DS + "src" + DS + "base";
    const std::string APPLIBS = ROOT + DS + "src" + DS + "applibs";
    const std::string BLD = string(TCE_BLD_ROOT);
    
    if (!DISTRIBUTED_VERSION) {
        includes.push_back(ROOT);
        includes.push_back(ROOT + DS + "src" + DS + "tools");
        includes.push_back(BASE + DS + "osal");
        includes.push_back(BASE + DS + "memory");
        includes.push_back(BASE + DS + "bem");
        includes.push_back(BASE + DS + "Graph");
        includes.push_back(BASE + DS + "idf");
        includes.push_back(BASE + DS + "mach");
        includes.push_back(BASE + DS + "program");
        includes.push_back(BASE + DS + "tpef");
        includes.push_back(BASE + DS + "umach");
        includes.push_back(APPLIBS + DS + "Simulator");
        includes.push_back(APPLIBS + DS + "mach");
        includes.push_back(APPLIBS + DS + "FSA");
        includes.push_back(TCE_BLD_ROOT);
    } 
    includes.push_back(string(TCE_INSTALLATION_ROOT) + DS + "include");
    
    return includes;
}

/**
 * Returns the include dir needed to build base opset in compiling the
 * distributed version of TCE
 *
 */
vector<string>
Environment::opsetIncludeDir() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    string BASE = TCE_SRC_ROOT + DS + "src" + DS + "base";
    
    vector<string> includePaths;   
    includePaths.push_back(BASE + DS + "osal ");
    includePaths.push_back(BASE + DS + "tpef ");
    includePaths.push_back(BASE + DS + "memory ");
    includePaths.push_back(TCE_SRC_ROOT + DS + "src" + DS +  "tools");
    includePaths.push_back(TCE_SRC_ROOT);

    return includePaths;
}

/**
 * Returns the value of the given environment variable.
 *
 * An empty string is returned if the variable does not exist.
 *
 * @param variable The environment variable whose value is required.
 * @return The value of the environment variable.
 */
string
Environment::environmentVariable(const std::string& variable) {

    char* varValue = getenv(variable.c_str());
    if (varValue == NULL) {
        return "";
    }

    return varValue;
}


/**
 * Constructor.
 */
Environment::Environment() {
}


/**
 * Destructor.
 */
Environment::~Environment() {
}


/**
 * Tries to locate the given directory or file on system.
 *
 * Returns the first absolute path of the directory if the directory/file
 * is found. In case the directory/file is not found in any of the search
 * paths, returns an empty string.
 *
 * @param name The name of the searched directory/file.
 * @return The path of the directory/file.
 */
string
Environment::pathTo(const std::string& name) {

    // test the stored paths and try to open the file
    for (unsigned int i = 0; i < pathsToUse_.size(); ++i) {
        string path =
            pathsToUse_[i] + FileSystem::DIRECTORY_SEPARATOR + name;

        if (FileSystem::fileExists(path)) {
            return path;
        }
    }

    return "";
}

/**
 * Sets the value for new schema file directory.
 *
 * @param path The path of new schema file directory.
 */
void
Environment::setNewSchemaFileDir(const std::string& path) {
    newSchemaFileDir_ = path;
}

/**
 * Sets the value for new data file directory
 *
 * @param path The path of new data file directory.
 */
void
Environment::setNewDataFileDir(const std::string& path) {
    newDataFileDir_ = path;
}

/**
 * Sets the value for new bitmaps file directory.
 *
 * @param path The path for new bitmaps directory.
 */
void
Environment::setNewBitmapsFileDir(const std::string& path) {
    newBitmapsFileDir_ = path;
}

/**
 * Sets the path for new manual file directory.
 *
 * @param path The path of new manual file directory.
 */
void
Environment::setNewManFileDir(const std::string& path) {
    newManFileDir_ = path;
}

/**
 * Sets the new configure file.
 *
 * @param file The new configure file.
 */
void
Environment::setNewConfFile(const std::string& file) {
    newConfFile_ = file;
}

/**
 * Sets the path for new error log file directory.
 *
 * @param path The path of new error log file directory.
 */
void
Environment::setNewErrorLogFileDir(const std::string& path) {
    newErrorLogFileDir_ = path;
}

/**
 * Returns the paths in which operation set files are searched.
 *
 * @return The paths in which operations are searched.
 */
vector<string>
Environment::osalPaths() {

	vector<string> paths;

	string DS = FileSystem::DIRECTORY_SEPARATOR;

    // ./data
    string data = FileSystem::currentWorkingDir() + DS + "data";

    // tce/opset/base -- these are needed during newlib build,
    // specifically STDOUT is needed to build puts() correctly.
    string src = string(TCE_BLD_ROOT) + DS + "opset" + DS + "base";
    paths.push_back(data);
    paths.push_back(src);

	// default path for predefined and "standard" operations
	string basePath =
	    string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
	    "opset" + DS + "base";
	// path for user-local custom operations
	string userPath =
	    FileSystem::homeDirectory() + DS + ".tce" + DS + "opset" +
	    DS + "custom";
	// path for system-wide, shared custom operations
	string customPath =
	    string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
	    "opset" + DS + "custom";
	// as a last resort fall-back, operations are always searched in
	// in current working directory
    string cwd = FileSystem::currentWorkingDir();

    VectorTools::insertUnique(paths, basePath);
    VectorTools::insertUnique(paths, userPath);
    VectorTools::insertUnique(paths, customPath);
    VectorTools::insertUnique(paths, cwd);

	return paths;
}

/**
 * Returns the paths in which code compressor plugins are searched.
 *
 * @return The paths in which code compressor plugins are searched.
 */
vector<string>
Environment::codeCompressorPaths() {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    string base = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "codecompressors" + DS + "base";
    string custom = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR)+
        "codecompressors" + DS + "custom";
    string personal = FileSystem::homeDirectory() + DS + ".tce" + DS +
        "codecompressors" + DS + "custom";
    string cwd = FileSystem::currentWorkingDir();

    paths.push_back(base);
    paths.push_back(personal);
    paths.push_back(custom);

    if (!DISTRIBUTED_VERSION) {
        string path = string(TCE_SRC_ROOT) + DS + "compressors";
        paths.push_back(path);
    }

    paths.push_back(cwd);        
    return paths;
}

/**
 * Returns the paths in which scheduler plugins are searched.
 *
 * @return The paths in which scheduler plugins are searched.
 */
vector<string>
Environment::schedulerPluginPaths() {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;

    string base = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "scheduler" + DS + "passes";

    string personal = FileSystem::homeDirectory() + DS + ".tce" + DS +
        "scheduler" + DS + "passes";

    string srcBase = string(TCE_SRC_ROOT) + DS +
        "scheduler" + DS + "passes";

    string cwd = FileSystem::currentWorkingDir();

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree to make the system tests etc.
        // to test the plugins in source base, not the installed plugins
        paths.push_back(srcBase);
    } 

    paths.push_back(base);
    paths.push_back(personal);
    paths.push_back(cwd);

    return paths;
}

/**
 * Returns the paths in which IC/decoder plugins are searched.
 *
 * @return The paths in which IC/decoder plugins are searched.
 */
vector<string>
Environment::icDecoderPluginPaths() {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;

    string base = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "icdecoder_plugins" + DS + "base";
    string custom = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR)+
        "icdecoder_plugins" + DS + "custom";
    string personal = FileSystem::homeDirectory() + DS + ".tce" + DS + 
        "icdecoder_plugins";
    string cwd = FileSystem::currentWorkingDir();
    string data = cwd + DS + "data";

    if (!DISTRIBUTED_VERSION) {
        string path = string(TCE_SRC_ROOT) + DS + "icdecoder_plugins";
        paths.push_back(path);
    }

    paths.push_back(base);
    paths.push_back(personal);
    paths.push_back(custom);
    paths.push_back(cwd);
    paths.push_back(data);
    return paths;
}


/**
 * Returns the paths in which hardware modules are searched.
 *
 * @return The paths in which hardware modules are searched.
 */
std::vector<std::string>
Environment::hwModulePaths() {
    vector<string> paths;
    paths.push_back(FileSystem::currentWorkingDir());
    return paths;
}


/**
 * Returns the paths in which hardware databases are searched.
 *
 * @return The paths in which hardware databases are searched.
 */
std::vector<std::string>
Environment::hdbPaths() {

    vector<string> paths;
    string DS = FileSystem::DIRECTORY_SEPARATOR;
    if (!DISTRIBUTED_VERSION) {
        string srcBase = string(TCE_SRC_ROOT) + DS + "hdb";
        paths.push_back(srcBase);
    }

    string instBase = 
        string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) + "hdb";
    paths.push_back(instBase);
    string cwd = FileSystem::currentWorkingDir();
    paths.push_back(cwd);
    paths.push_back(cwd + DS + "data"); 
    return paths;
}


/**
 * Returns the paths in which VHDL files are searched for.
 *
 * @param hdbPath The path where the HDB file is, to be added to the
 * search paths.
 * @return The paths in which VHDL files are searched for.
 */
std::vector<std::string>
Environment::vhdlPaths(const std::string& hdbPath) {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;

    // Drop the name of hdb file from hdb path
    string hdbDir = FileSystem::directoryOfPath(hdbPath);
    
    
    paths.push_back(hdbDir);
    paths.push_back(hdbDir + DS + "vhdl");    
    
    if (!DISTRIBUTED_VERSION) {
        string srcBase = string(TCE_SRC_ROOT) + DS + "hdb";
        paths.push_back(srcBase);
        paths.push_back(srcBase + DS + "vhdl");
    }

    string srcBase = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) 
        + "hdb";
    paths.push_back(srcBase);
    paths.push_back(srcBase + DS + "vhdl");
    string cwd = FileSystem::currentWorkingDir();
    paths.push_back(cwd);
    paths.push_back(cwd + DS + "data");
    return paths;
}


/**
 * Returns the paths in which decompressor module definitions are searhed.
 *
 * @return The paths in which decompressor module definitions are searched.
 */
std::vector<std::string>
Environment::decompressorPaths() {
    vector<string> paths;
    paths.push_back(FileSystem::currentWorkingDir());
    return paths;
}


/**
 * Returns the paths in which explorer plugins are searched.
 *
 * @return The paths in which explorer plugins are searched.
 */
vector<string>
Environment::explorerPluginPaths() {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;

    if (!DISTRIBUTED_VERSION) {
        string srcBase = string(TCE_SRC_ROOT) + DS + "explorer";
        paths.push_back(srcBase);
    }

    string base = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "explorer" + DS + "base";
    paths.push_back(base);

    string personal = FileSystem::homeDirectory() + DS + ".tce" + DS +
        "explorer";
    paths.push_back(personal);

    if (DISTRIBUTED_VERSION) {
        string custom = string(TCE_SRC_ROOT) + DS +
            "explorer";
        paths.push_back(custom);
    }

    string cwd = FileSystem::currentWorkingDir();
    paths.push_back(cwd);

    string data = FileSystem::currentWorkingDir() + DS +
        "data";
    paths.push_back(data);

    return paths;
}


/**
 * Returns the paths in which estimator plugins are searched.
 *
 * @return The paths in which estimator plugins are searched.
 */
vector<string>
Environment::estimatorPluginPaths() {

    vector<string> paths;
    const string DS = FileSystem::DIRECTORY_SEPARATOR;
    string base = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "cost_estimator_plugins";
    string baseRF = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "cost_estimator_plugins/rf";
    string baseFU = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "cost_estimator_plugins/fu";
    string personal = FileSystem::homeDirectory() + DS + ".tce" + DS +
        "cost_estimator_plugins";
    string custom = string(TCE_SRC_ROOT) + DS +
        "cost_estimator_plugins";
    string cwd = FileSystem::currentWorkingDir();
    string data = FileSystem::currentWorkingDir() + DS +
        "data";

    paths.push_back(base);
    paths.push_back(baseRF);
    paths.push_back(baseFU);
    paths.push_back(personal);
    paths.push_back(custom);
    paths.push_back(cwd);
    paths.push_back(data);
    return paths;
}


/**
 * Returns full path to the TCE .pdf manual.
 *
 * @return Full path to the TCE manual pdf.
 */
string
Environment::pdfManual() {

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree
        std::string srcPath =
            string(TCE_SRC_ROOT) + FileSystem::DIRECTORY_SEPARATOR +
            PDF_MANUAL_SRC;

        if (FileSystem::fileExists(srcPath))
            return srcPath;
    } 
    std::string path =
        string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        PDF_MANUAL_INSTALLED;

    if (!FileSystem::fileExists(path))
        debugLog("Installation broken, manual not found");
    return path;
}

/**
 * Returns the full path to the minimal.adf.
 */
string
Environment::minimalADF() {

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree
        std::string srcPath =
            string(TCE_SRC_ROOT) + FileSystem::DIRECTORY_SEPARATOR +
            MINIMAL_ADF_SRC;

        if (FileSystem::fileExists(srcPath))
            return srcPath;
    } 
    std::string path =
        string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        MINIMAL_ADF_INSTALLED;

    assert(
        FileSystem::fileExists(path) && 
        "Installation broken, minimal.adf not found");
    return path;
}


/**
 * Returns full path to the tce compiler tcecc.
 *
 * @return Full path to the tce compiler tcecc.
 */
string
Environment::tceCompiler() {

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree
        std::string srcPath =
            string(TCE_SRC_ROOT) + "/src/bintools/Compiler/tcecc";

        if (FileSystem::fileExists(srcPath))
            return srcPath;
    } 
    std::string path =
        string(TCE_INSTALLATION_ROOT) + "/bin/tcecc";
    assert(
        FileSystem::fileExists(path) &&
        "Installation broken, tcecc not found.");
    return path;
}


/**
 * Returns full path to the default scheduler pass configuration file.
 *
 * @return Full path to the default_scheduler.conf
 */
string
Environment::defaultSchedulerConf() {

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree
        std::string srcPath =
            string(TCE_SRC_ROOT) + "/scheduler/passes/default_scheduler.conf";

        if (FileSystem::fileExists(srcPath))
            return srcPath;
    } 
    std::string path =
        string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "/scheduler/passes/default_scheduler.conf";
    assert(
        FileSystem::fileExists(path) &&
        "Installation broken, default_scheduler.conf not found.");
    return path;
}


/**
 * Returns full path to the old gcc scheduler pass configuration file.
 *
 * This is the scheduler conf that is used to schedule code from the
 * old gcc compiler.
 *
 * @return Full path to the old_gcc.conf
 */
string
Environment::oldGccSchedulerConf() {

    if (!DISTRIBUTED_VERSION) {
        // first find from the src tree
        std::string srcPath =
            string(TCE_SRC_ROOT) + "/scheduler/passes/old_gcc.conf";

        if (FileSystem::fileExists(srcPath))
            return srcPath;
    } 
    std::string path =
        string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) +
        "/scheduler/passes/old_gcc.conf";
    assert(
        FileSystem::fileExists(path) &&
        "Installation broken, old_gcc.conf not found.");
    return path;
}

/**
 * Returns full path to the default IC/Decoder plugin file.
 *
 * @return Full path to the defaultICDecoderPlugin.so
 */
string
Environment::defaultICDecoderPlugin() {

    try {
        std::string file = "DefaultICDecoderPlugin.so";
        vector<std::string> paths = icDecoderPluginPaths();
        std::string path = FileSystem::findFileInSearchPaths(paths, file);

        return path;
    } catch(FileNotFound e) {
        abortWithError(
            "Installation broken, DefaultICDecoderPlugin.so not found.");
        return ""; // just to remove a warning
    }
}

/**
 * Returns default text editor to be used.
 *
 * @return Full path of default text editor or contents of EDITOR/VISUAL
 * environment variables. Empty string is also returned if no editor is found.
 */
string
Environment::defaultTextEditorPath() {

    std::string editor = environmentVariable("EDITOR");
    if (editor.empty()) {
        editor = environmentVariable("VISUAL");
    }
    if (editor.empty()) {
        // if couldn't get editor from env variables test if some usual
        // default editor is found
        vector<std::string> editors;
        editors.push_back("/usr/bin/emacs");
        editors.push_back("/usr/bin/xemacs");
        editors.push_back("/bin/nano");
        editors.push_back("/usr/bin/nano");
        editors.push_back("/usr/bin/vim");
        editors.push_back("/usr/bin/vi");
        for (unsigned int i = 0; i < editors.size(); ++i) {
            if (FileSystem::fileIsExecutable(editors.at(i))) {
                return editors.at(i);
            }
        }
        // if none of above editors were found try to find from PATH env
        // variable
        editors.clear();
        std::string DS = FileSystem::DIRECTORY_SEPARATOR;
        editors.push_back("nano");
        editors.push_back("emacs");
        editors.push_back("vim");
        editors.push_back("vi");
        vector<std::string> paths;
        parsePathEnvVariable(paths);
        for (unsigned int i = 0; i < paths.size(); ++i) {
            for (unsigned int j = 0; j < editors.size(); ++j) {
                editor = paths.at(i) + DS + editors.at(j);
                if (FileSystem::fileIsExecutable(editor)) {
                    return editor;
                }
            }
        }
         
        // no editor were found
        return "";
    } 

    // testi if env variable contained full path to text editor executable
    if (FileSystem::fileIsExecutable(editor)) {
        return editor;
    }

    // EDITOR and VISUAL doesn't have to contain full path
    // so let's search PATH enviroment variable to get the full path
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::string testEditor = "";
    vector<std::string> paths;
    parsePathEnvVariable(paths);
    for (unsigned int i = 0; i < paths.size(); ++i) {
        testEditor = paths.at(i) + DS + editor;
        if (FileSystem::fileIsExecutable(testEditor)) {
            return testEditor;
        }
    }

    // no editor were found
    return "";
}

/**
 * Returns Paths in PATH environment variable.
 *
 * @return Paths in PATH environment variable.
 */
inline void
Environment::parsePathEnvVariable(std::vector<std::string>& paths) {

    std::string pathsEnv = environmentVariable("PATH");
    StringTools::chopString(pathsEnv, ":", paths);
}

/**
 * Returns full path to llvmtce plugin cache directory.
 *
 * @return Full path to the llvmtce plugin cache directory.
 */
string
Environment::llvmtceCachePath() {

    std::string path =
        FileSystem::homeDirectory() +
        FileSystem::DIRECTORY_SEPARATOR + string(".tce") +
        FileSystem::DIRECTORY_SEPARATOR + string("tcecc") +
        FileSystem::DIRECTORY_SEPARATOR + string("cache");

    return path;
}

/**
 * Returns full paths to implementation tester vhdl testbench template 
 * directory
 *
 * @return Paths to testbench templates
 */
std::vector<std::string> Environment::implementationTesterTemplatePaths() {
    std::vector<std::string> paths;
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    if (!DISTRIBUTED_VERSION) {
        string path = string(TCE_SRC_ROOT) + DS + "data" + DS + "hdb";
        paths.push_back(path);
    }
    string path = string(TCE_INSTALLATION_ROOT) + string(INSTALLATION_DIR) 
        + "data" + DS + "hdb";
    paths.push_back(path);
    return paths;
}
