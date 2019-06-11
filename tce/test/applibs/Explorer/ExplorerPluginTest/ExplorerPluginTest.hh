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
 * @file ExplorerPluginTest.hh 
 *
 * A test suite for loading DesignSpaceExplorerPlugins.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXPLORER_PLUGIN_TEST_HH
#define TTA_EXPLORER_PLUGIN_TEST_HH

#include <string>
#include <TestSuite.h>
#include <map>

#include "Application.hh"
#include "DesignSpaceExplorer.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Exception.hh"
#include "HDBRegistry.hh"
#include "Conversion.hh"

using std::string;

/**
 * Class that tests explorer plugin loading.
 */
class ExplorerPluginTest : public CxxTest::TestSuite {
public:
    void testLoadPlugin();
    // initialmachine plugin is depracated
    //void testInitialMachinePlugin();
};

/**
 * Test explorer plugin loading.
 */
void
ExplorerPluginTest::testLoadPlugin() {

    const string dsdbFile = "data/test.dsdb";
    FileSystem::removeFileOrDirectory(dsdbFile);
    DSDBManager* dsdb = DSDBManager::createNew(dsdbFile);
    string pluginName = "TestExplorerPlugin";
    DesignSpaceExplorerPlugin* plugin = 
        DesignSpaceExplorer::loadExplorerPlugin(pluginName, dsdb);
    RowID conf = 1;
    std::vector<RowID> result = 
        plugin->explore(conf, 0);
    TS_ASSERT_EQUALS(Conversion::toInt(result.size()), 3);
}

/**
 * Test initial machine generator plugin.
 */
/*
void
ExplorerPluginTest::testInitialMachinePlugin() {

    const string dsdbFile = "data/test.dsdb";
    FileSystem::removeFileOrDirectory(dsdbFile);
    DSDBManager* dsdb = DSDBManager::createNew(dsdbFile);
    dsdb->addApplication("../DesignSpaceExplorerTest/data/TestApp");
    dsdb->addApplication("../DesignSpaceExplorerTest/data/TestApp2");
    dsdb->addApplication("data/TestAppTremor");
    string pluginName = "InitialMachineExplorer";
    DesignSpaceExplorerPlugin* plugin = 
        DesignSpaceExplorer::loadExplorerPlugin(pluginName, *dsdb);
    DesignSpaceExplorerPlugin::ParameterTable parameters;
    DesignSpaceExplorerPlugin::Parameter bus;
    bus.name = "bus_count";
    bus.value = "5";
    parameters.push_back(bus);
    DesignSpaceExplorerPlugin::Parameter imm32;
    imm32.name = "imm_32";
    imm32.value = "1";
    parameters.push_back(imm32);
    DesignSpaceExplorerPlugin::Parameter imm16;
    imm16.name = "imm_16";
    imm16.value = "0";
    parameters.push_back(imm16);
    DesignSpaceExplorerPlugin::Parameter idf;
    idf.name = "build_idf";
    idf.value = "true";
//    parameters.push_back(idf);
    //DesignSpaceExplorerPlugin::Parameter icHDB;
    //icHDB.name = "ic_hdb";
    //icHDB.value = "10_bus_full_connectivity.hdb";
    //parameters.push_back(icHDB);
    
    plugin->setParameters(parameters);
    // Add a HDB to HDBRegistry
    HDB::HDBRegistry& hdbRegistry = HDB::HDBRegistry::instance();
    hdbRegistry.loadFromSearchPaths();
    //plugin
    RowID conf = 1;
    std::vector<RowID> result = 
        plugin->explore(conf, 0);
    TS_ASSERT_EQUALS(Conversion::toInt(result.size()), 1);
}
*/

#endif
