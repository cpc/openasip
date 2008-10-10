/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file Environment.hh
 *
 * Declaration of Environment class.
 *
 * @author Atte Oksman 2003 (oksman-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note reviewed 19 May 2004 by ao, jn, ml, am
 * @note rating: red
 */

#ifndef TTA_ENVIRONMENT_HH
#define TTA_ENVIRONMENT_HH

#include <string>
#include <vector>

/**
 * A class to get the environmental variable values as well as to find files
 * or directories from the project's directory structure.
 */
class Environment {

public:
    static void initialize();

    static std::string schemaDirPath(const std::string& prog);
    static std::string dataDirPath(const std::string& prog);
    static std::string bitmapsDirPath(const std::string& prog);
    static std::string manDirPath(const std::string& prog);
    static std::string confDirPath(const std::string& fileName);
    static std::string errorLogFilePath();
    static std::string iconDirPath();
    static std::vector<std::string> includeDirPaths();
    static std::vector<std::string> opsetIncludeDir();

    static void setNewSchemaFileDir(const std::string& path);
    static void setNewDataFileDir(const std::string& path);
    static void setNewBitmapsFileDir(const std::string& path);
    static void setNewManFileDir(const std::string& path);
    static void setNewConfFile(const std::string& file);
    static void setNewErrorLogFileDir(const std::string& path);

    static std::string environmentVariable(const std::string& variable);
    static std::vector<std::string> osalPaths();
    static std::vector<std::string> codeCompressorPaths();
    static std::vector<std::string> schedulerPluginPaths();
    static std::vector<std::string> icDecoderPluginPaths();
    static std::vector<std::string> hwModulePaths();
    static std::vector<std::string> hdbPaths();
    static std::vector<std::string> vhdlPaths(const std::string& hdbPath);
    static std::vector<std::string> decompressorPaths();
    static std::vector<std::string> explorerPluginPaths();
    static std::vector<std::string> estimatorPluginPaths();

    static std::string pdfManual();
    static std::string minimalADF();
    static std::string tceCompiler();
    static std::string defaultSchedulerConf();
    static std::string oldGccSchedulerConf();
    static std::string defaultICDecoderPlugin();
    static std::string defaultTextEditorPath();

    static std::string llvmtceCachePath();

private:
    Environment();
    ~Environment();

    static std::string pathTo(const std::string& name);
    static inline void parsePathEnvVariable(std::vector<std::string>& paths);

    /// Name of the XML Schema directory.
    static const std::string SCHEMA_DIR_NAME_;
    /// Name of the data directory.
    static const std::string DATA_DIR_NAME_;
    /// Name of the bitmaps directory.
    static const std::string BITMAPS_DIR_NAME_;
    /// Name of the manual directory.
    static const std::string MAN_DIR_NAME_;
    /// Name of the configuration directory.
    static const std::string CONF_DIR_NAME_;
    /// Name of the toolbar icon directory.
    static const std::string ICON_DIR_NAME_;
    /// Name of the system error log file.
    static const std::string ERROR_LOG_FILE_NAME_;
    /// Flag indicating whether class has been initialized.
    static bool initialized_;

    /// Schema directory search paths.
    static std::vector<std::string> schemaPaths_;
    /// Data directory search paths.
    static std::vector<std::string> dataPaths_;
    /// Bitmaps directory search paths.
    static std::vector<std::string> bitmapsPaths_;
    /// Man directory search paths.
    static std::vector<std::string> manPaths_;
    /// Configure file search paths.
    static std::vector<std::string> confPaths_;
    /// Toolbar icon search paths.
    static std::vector<std::string> iconPaths_;
    /// Error file search paths.
    static std::vector<std::string> errorPaths_;
    /// Paths used in a particular search.
    static std::vector<std::string> pathsToUse_;

    /// Directory for new schema file.
    static std::string newSchemaFileDir_;
    /// Directory for new data file.
    static std::string newDataFileDir_;
    /// Directory for new bitmaps file.
    static std::string newBitmapsFileDir_;
    /// Directory for new icon file.
    static std::string newIconFileDir_;
    /// Directory for new manual file.
    static std::string newManFileDir_;
    /// Directory for new configure file.
    static std::string newConfFile_;
    /// Directory for new error log file.
    static std::string newErrorLogFileDir_;

    /// Relative path to the installed TCE .pdf manual.
    static const std::string PDF_MANUAL_INSTALLED;
    /// Relative path to the TCE .pdf manual in source tree.
    static const std::string PDF_MANUAL_SRC;
    /// Relative path to the installed minimal.adf.
    static const std::string MINIMAL_ADF_INSTALLED;
    /// Relative path to the minimal.adf in source tree.
    static const std::string MINIMAL_ADF_SRC;

};

#endif
