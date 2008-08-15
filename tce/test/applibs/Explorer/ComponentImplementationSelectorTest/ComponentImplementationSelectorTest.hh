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
 * @file ComponentImplementationSelectorTest.hh 
 *
 * A test suite for ComponentImplementationSelector class.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMPONENT_IMPLEMENTATION_SELECTOR_TEST_HH
#define TTA_COMPONENT_IMPLEMENTATION_SELECTOR_TEST_HH

#include <string>
#include <TestSuite.h>
#include <set>
#include <map>
#include <list>

#include "FileSystem.hh"
#include "ComponentImplementationSelector.hh"
#include "FunctionUnit.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "ContainerTools.hh"
#include "CostEstimates.hh"
#include "TCEString.hh"

using namespace TTAMachine;
using namespace HDB;
using std::set;
using std::map;
using std::list;
using std::string;

const string DS = FileSystem::DIRECTORY_SEPARATOR;
const string HDB_FILE = "data" + DS + "test.hdb";
const string HDB_FILE2 = "data" + DS + "fu.hdb";
const string HDB_FILE3 = "data" + DS + "rf.hdb";
const string ADF_FILE = "data" + DS + "test.adf";
const string ADF_FILE2 = "data" + DS + "fu.adf";
const string ADF_FILE3 = "data" + DS + "rf.adf";

/**
 * Class that tests ComponentImplementationSelector class.
 */
class ComponentImplementationSelectorTest : public CxxTest::TestSuite {
public:
    void testFuArchsByOpSetWithMinLatency();
    void testFuImplementations();
    void testRFImplementations();
};

/**
 * Test fuArchsByOpSetWithMinLatency() function and check that it returns
 * correct data.
 */
void
ComponentImplementationSelectorTest::testFuArchsByOpSetWithMinLatency() {

    HDBManager& hdb = HDBRegistry::instance().hdb(HDB_FILE);
    ComponentImplementationSelector selector;
    selector.addHDB(hdb);

    TTAMachine::Machine* mach = TTAMachine::Machine::loadFromADF(ADF_FILE);

    Machine::FunctionUnitNavigator navigator = mach->functionUnitNavigator();

    OperationPool operationPool;
    Operation* add = &operationPool.operation("add");
    Operation* div = &operationPool.operation("div");
    Operation* sub = &operationPool.operation("sub");
    Operation* mul = &operationPool.operation("mul");
    set<string> operations;
    operations.insert(div->name());
    operations.insert(add->name());

    // this should return only one FU where are both operations
    list<FunctionUnit*> fus1 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    unsigned int size = 1;
    TS_ASSERT_EQUALS(fus1.size(), size);

    FunctionUnit* fuAddDiv = navigator.item("add_div");
    list<FunctionUnit*>::const_iterator iter1 = fus1.begin();
    TS_ASSERT((*iter1)->isArchitectureEqual(fuAddDiv));
    
    operations.clear();
    operations.insert(sub->name());
    list<FunctionUnit*> fus2 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    size = 1;
    TS_ASSERT_EQUALS(fus2.size(), size);
    list<FunctionUnit*>::const_iterator iter2 = fus2.begin();
    FunctionUnit* fuSub = navigator.item("sub");
    TS_ASSERT((*iter2)->isArchitectureEqual(fuSub));

    // test with such operation set that has no matches
    operations.clear();
    operations.insert(mul->name());
    list<FunctionUnit*> fus3 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    size = 0;
    TS_ASSERT_EQUALS(fus3.size(), size);

    // test with operation set {mul,div} where only div operation exists
    operations.clear();
    operations.insert(mul->name());
    operations.insert(div->name());
    list<FunctionUnit*> fus4 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    size = 0;
    TS_ASSERT_EQUALS(fus4.size(), size);

    // test with operation div
    operations.clear();
    operations.insert(div->name());
    list<FunctionUnit*> fus5 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    size = 1;
    TS_ASSERT_EQUALS(fus5.size(), size);
    list<FunctionUnit*>::const_iterator iter5 = fus5.begin();
    FunctionUnit* fuDiv = navigator.item("div");
    TS_ASSERT((*iter5)->isArchitectureEqual(fuDiv));

    // test with multiple operations
    operations.clear();
    operations.insert(div->name());
    operations.insert(sub->name());
    operations.insert(add->name());
    list<FunctionUnit*> fus6 = 
        selector.fuArchsByOpSetWithMinLatency(operations);
    size = 2;
    TS_ASSERT_EQUALS(fus6.size(), size);
    list<FunctionUnit*> expectedResults;
    expectedResults.push_back(fuAddDiv);
    expectedResults.push_back(fuSub);
    
    list<FunctionUnit*>::const_iterator iter6 = fus6.begin();
    for (; iter6 != fus6.end(); iter6++) {
        list<FunctionUnit*>::const_iterator result = expectedResults.begin();
        bool found = false;
        for (; result != expectedResults.end(); result++) {
            if ((*iter6)->isArchitectureEqual(*result)) {
                found = true;
            }
        }
        TS_ASSERT(found);
    }

    delete mach;
    AssocTools::deleteAllItems(fus1);
    AssocTools::deleteAllItems(fus2);
    AssocTools::deleteAllItems(fus3);
    AssocTools::deleteAllItems(fus4);
    AssocTools::deleteAllItems(fus5);
    AssocTools::deleteAllItems(fus6);    
}

/**
 * Test for fuImplementations() method.
 */
void
ComponentImplementationSelectorTest::testFuImplementations() {

    HDBManager& hdb = HDBRegistry::instance().hdb(HDB_FILE2);
    ComponentImplementationSelector selector;
    selector.addHDB(hdb);

    TTAMachine::Machine* mach = TTAMachine::Machine::loadFromADF(ADF_FILE2);

    Machine::FunctionUnitNavigator navigator = mach->functionUnitNavigator();
    
    FunctionUnit* fuAdd = navigator.item("add");
    
    map<const IDF::FUImplementationLocation*, CostEstimates*> results =
        selector.fuImplementations(*fuAdd, 100, 1000);

    unsigned int resultsSize = 3; 
    TS_ASSERT_EQUALS(results.size(), resultsSize);

    map<const IDF::FUImplementationLocation*,CostEstimates*>::const_iterator i
        = results.begin();
    for(; i != results.end(); i++) {
        int id = (*i).first->id();
        if (id != 21 && id != 27 && id != 28) {
            TS_ASSERT(false);
        }
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::FUImplementationLocation*, CostEstimates*> results2 =
        selector.fuImplementations(*fuAdd, 200, 1000);

    unsigned int resultsSize2 = 1; 
    TS_ASSERT_EQUALS(results2.size(), resultsSize2);

    i = results2.begin();
    for(; i != results2.end(); i++) {
        int id = (*i).first->id();
        if (id != 28) {
            TS_ASSERT(false);
        }
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::FUImplementationLocation*, CostEstimates*> results3 =
        selector.fuImplementations(*fuAdd, 200);

    i = results3.begin();
    for(; i != results3.end(); i++) {
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::FUImplementationLocation*, CostEstimates*> results4 =
        selector.fuImplementations(*fuAdd, 0, 1000);

    i = results4.begin();
    for(; i != results4.end(); i++) {
        TS_ASSERT(i->second != NULL);
    }
}

/**
 * Test for rfImplementations() method.
 */
void
ComponentImplementationSelectorTest::testRFImplementations() {

    HDBManager& hdb = HDBRegistry::instance().hdb(HDB_FILE3);
    ComponentImplementationSelector selector;
    selector.addHDB(hdb);
    TTAMachine::Machine* mach = TTAMachine::Machine::loadFromADF(ADF_FILE3);

    Machine::RegisterFileNavigator navigator = mach->registerFileNavigator();
    RegisterFile* rf = navigator.item("RF");

    map<const IDF::RFImplementationLocation*, CostEstimates*> results =
        selector.rfImplementations(*rf, false, 300, 2500);

    unsigned int resultsSize = 4;
    TS_ASSERT_EQUALS(results.size(), resultsSize);

    map<const IDF::RFImplementationLocation*,CostEstimates*>::const_iterator i
        = results.begin();
    for(; i != results.end(); i++) {
        int id = (*i).first->id();
        if (id != 98 && id != 97 && id != 96 && id != 51) {
            TS_ASSERT(false);
        }
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::RFImplementationLocation*, CostEstimates*> results2 =
        selector.rfImplementations(*rf, false, 900, 2500);

    unsigned int resultsSize2 = 2;
    TS_ASSERT_EQUALS(results2.size(), resultsSize2);

    i = results2.begin();
    for(; i != results2.end(); i++) {
        int id = (*i).first->id();
        if (id != 98 && id != 51) {
            TS_ASSERT(false);
        }
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::RFImplementationLocation*, CostEstimates*> results3 =
        selector.rfImplementations(*rf, 300);

    i = results3.begin();
    for(; i != results3.end(); i++) {
        TS_ASSERT(i->second != NULL);
    }

    map<const IDF::RFImplementationLocation*, CostEstimates*> results4 =
        selector.rfImplementations(*rf, 0, 2500);

    i = results4.begin();
    for(; i != results4.end(); i++) {
        TS_ASSERT(i->second != NULL);
    }
}

#endif
