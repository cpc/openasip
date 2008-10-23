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
