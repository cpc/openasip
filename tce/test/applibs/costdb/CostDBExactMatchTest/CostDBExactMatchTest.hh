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
 * @file CostDBExactMatchTest.hh 
 *
 * A test suite for CostDatabase building.
 *
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 */    

#ifndef TTA_COSTDB_EXACT_MATCH_TEST_HH
#define TTA_COSTDB_EXACT_MATCH_TEST_HH

#include <string>
#include <TestSuite.h>
#include "CostDatabase.hh"
#include "FilterSearch.hh"
#include "EntryKeyProperty.hh"
#include "CostDBTypes.hh"
#include "CostDBEntry.hh"
#include "SearchStrategy.hh"
#include "MatchType.hh"
#include "EntryKeyData.hh"
#include "ExactMatch.hh"
#include "Interpolation.hh"
#include "Matcher.hh"
#include "CostDBEntryStats.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "Exception.hh"
#include "CostDatabaseRegistry.hh"

using std::string;


/**
 * Tests for making exact match searches in cost database.
 */
class CostDBExactMatchTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    CostDBEntry* createBusEntry(int data, int fanin, int fanout,
                                double area, double delay,
                                double energyActive, double energyIdle);
    CostDBEntry* createRFEntry(int size, int bitWidth, int reads,
                               int writes, double area, double delay,
                               double energyActive, double energyIdle);
    void testExactMatchTest();
    void testRFExactMatchTest();
    
private:
    CostDatabaseRegistry* costDatabaseRegistry_;
    CostDatabase* costDB_;
    SearchStrategy* strategy_;
    EntryKeyProperty* mbus_property;
    EntryKeyProperty* rfileProperty;
    std::vector<MatchType*> matchTypeList;
    std::vector<CostDBEntry*> entryList;
};


/**
 * Called before test.
 */
void
CostDBExactMatchTest::setUp() {

    // assign hdb to cost database
    HDB::HDBManager& hdb = 
        HDB::HDBRegistry::instance().hdb("data/asic.hdb");
    // Gen an CostDatabaseRegistry instance.
    costDatabaseRegistry_ = &CostDatabaseRegistry::instance();
    // create an instance of cost database
    costDB_ = &costDatabaseRegistry_->costDatabase(hdb);
    // Create and assign a search strategy to the cost database 
    strategy_ = new FilterSearch();
    costDB_->setSearchStrategy(strategy_);
    // create bus entry
    mbus_property =
        EntryKeyProperty::create(CostDBTypes::EK_MBUS);
    mbus_property->createFieldProperty(CostDBTypes::EKF_BIT_WIDTH);
    mbus_property->createFieldProperty(CostDBTypes::EKF_BUS_FANIN);
    mbus_property->createFieldProperty(CostDBTypes::EKF_BUS_FANOUT);

    // create rf entry and fields
    rfileProperty =
        EntryKeyProperty::create(CostDBTypes::EK_RFILE);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_NUM_REGISTERS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_READ_PORTS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_WRITE_PORTS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_BIT_WIDTH);
}


/**
 * Called after test.
 */
void CostDBExactMatchTest::tearDown() {
    mbus_property->destroy();
    delete costDatabaseRegistry_;
}


/**
 * Function that adds an entry to costDB with given parameters.
 */
CostDBEntry* 
CostDBExactMatchTest::createBusEntry(int data, int fanin, int fanout,
                                     double area, double delay,
                                     double energyActive,
                                     double energyIdle) {
    
    CostDBEntryKey* newKey = new CostDBEntryKey(mbus_property);
    
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(data),
            mbus_property->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(fanin),
            mbus_property->fieldProperty(CostDBTypes::EKF_BUS_FANIN)));
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(fanout),
            mbus_property->fieldProperty(CostDBTypes::EKF_BUS_FANOUT)));
    
    
    CostDBEntryStats* newStats = new CostDBEntryStats(area, delay);
    newStats->setEnergyActive(energyActive);
    newStats->setEnergyIdle(energyIdle);
    
    CostDBEntry* new_entry = new CostDBEntry(newKey);
    new_entry->addStatistics(newStats);
    
    costDB_->insertEntry(new_entry);
    
    return new_entry;
}


/**
 * Test that adds entries in the costDB and then searches exact matches.
 */
void CostDBExactMatchTest::testExactMatchTest() {
    
    int    data          = 2;
    int    fanin         = 4;
    int    fanout        = 6;
    double area          = 8.0;
    double delay         = 10.0;
    double energyActive  = 12.0;
    double energyIdle    = 14.0;
    

    // "Read the cost database."
    CostDBEntry* bus1 = createBusEntry(
        data+4, fanin, fanout, area, delay, energyActive, energyIdle);
    CostDBEntry* bus2 = createBusEntry(
        data+4, fanin+4, fanout+4, area+4, delay+4, energyActive+4,
        energyIdle+4);

    // Equal to bus1 to ensure that multiple entries can not have
    // same properties
    CostDBEntry* bus3 = createBusEntry(
        data+4, fanin, fanout, area, delay, energyActive, energyIdle);
    delete bus3;
    bus3 = 0;

    // Same entry cannot be added multiple times.
    TS_ASSERT_THROWS(costDB_->insertEntry(bus1), ObjectAlreadyExists);

    // assign new search strategy 
    // (stupid replace, should be tested better with different strategies)
    costDB_->setSearchStrategy(strategy_);
    
    EntryKeyProperty* search_key_property = 
        EntryKeyProperty::create(CostDBTypes::EK_MBUS);
    search_key_property->createFieldProperty(CostDBTypes::EKF_BUS_FANIN);
    
    // Creates the search key.
    CostDBEntryKey* search_key = new CostDBEntryKey(search_key_property);  
    search_key->addField(
        new EntryKeyField(
            new EntryKeyDataInt(fanin),
            search_key_property->fieldProperty(CostDBTypes::EKF_BUS_FANIN)));
    search_key->addField(
        new EntryKeyField(
            new EntryKeyDataInt(data+4),
            search_key_property->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    
    // Create the searh type of the querie (Exact match)
    CostDBTypes::MatchTypeTable match;    
    match.push_back(
        new MatchType(search_key_property->
                      fieldProperty(CostDBTypes::EKF_BIT_WIDTH), 
                      CostDBTypes::MATCH_EXACT));
    
    // Perform a database query.
    CostDBTypes::EntryTable results;
    results = costDB_->search(*search_key, match);
    
    TS_ASSERT(results.size() == 2);
    TS_ASSERT(results[0]->isEqualKey(*bus1));
    TS_ASSERT(results[1]->isEqualKey(*bus2));
    

    // make search again to use the cache
    results = costDB_->search(*search_key, match);
    delete search_key;
    search_key = 0;
    delete match.at(0);
    match.at(0) = 0;

    TS_ASSERT(results.size() == 2);
    TS_ASSERT(results[0]->isEqualKey(*bus1));
    TS_ASSERT(results[1]->isEqualKey(*bus2));

    // search something that is not in the cost database
    EntryKeyProperty* search_key_property2 = 
        EntryKeyProperty::create(CostDBTypes::EKF_BUS_FANIN);
    search_key_property2->createFieldProperty(CostDBTypes::EKF_BUS_FANIN);
    CostDBEntryKey* search_key2 = new CostDBEntryKey(search_key_property2);  
    search_key2->addField(
        new EntryKeyField(
            new EntryKeyDataInt(fanin),
            search_key_property2->fieldProperty(
                CostDBTypes::EKF_BUS_FANIN)));

    CostDBTypes::MatchTypeTable match2;
    
    match2.push_back(
        new MatchType(search_key_property2->
                      fieldProperty(CostDBTypes::EKF_BUS_FANIN), 
                      CostDBTypes::MATCH_EXACT));
  
    TS_ASSERT_THROWS(costDB_->search(*search_key2, match2), KeyNotFound);
    delete search_key2;
    search_key2 = 0;
    delete match2.at(0);
    match2.at(0) = 0;
}


/**
 * Function that adds an RF entry to costDB with given parameters.
 */
CostDBEntry* 
CostDBExactMatchTest::createRFEntry(int size, int bitWidth, int reads,
                                    int writes, double area, double delay,
                                    double energyActive, double energyIdle) {

    CostDBEntryKey* newKey = new CostDBEntryKey(rfileProperty);
    
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(size),
            rfileProperty->fieldProperty(CostDBTypes::EKF_NUM_REGISTERS)));
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(bitWidth),
            rfileProperty->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(reads),
            rfileProperty->fieldProperty(CostDBTypes::EKF_READ_PORTS)));
    newKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(writes),
            rfileProperty->fieldProperty(CostDBTypes::EKF_WRITE_PORTS)));
    
    
    CostDBEntryStats* newStats = new CostDBEntryStats(area, delay);
    newStats->setEnergyActive(energyActive);
    newStats->setEnergyIdle(energyIdle);
    
    CostDBEntry* newEntry =
        new CostDBEntry(newKey);
    newEntry->addStatistics(newStats);
    
    costDB_->insertEntry(newEntry);

    return newEntry;
}


/**
 * Test that adds entries in the costDB and then searches exact matches with
 * RFs.
 */
void CostDBExactMatchTest::testRFExactMatchTest() {
    
    int    size          = 2;
    int    bitWidth      = 32;
    int    reads         = 3;
    int    writes        = 4;
    double area          = 8.0;
    double delay         = 10.0;
    double energyActive  = 12.0;
    double energyIdle    = 14.0;
    

    // "Read the cost database."
    CostDBEntry* rf1 = createRFEntry(
        size, bitWidth, reads, writes, area, delay, energyActive, energyIdle);
    CostDBEntry* rf2 = createRFEntry(
        size+4, bitWidth, reads+4, writes+4, area+4, delay+4, energyActive+4,
        energyIdle+4);
    
    // Equal to rf1 to ensure that multiple entries can not have
    // same properties
    CostDBEntry* rf3 = createRFEntry(
        size, bitWidth, reads, writes, area, delay, energyActive, energyIdle);
    delete rf3;
    rf3 = 0;

    // Same entry cannot be added multiple times.
    TS_ASSERT_THROWS(costDB_->insertEntry(rf1), ObjectAlreadyExists);

    // assign new search strategy 
    // (stupid replace, should be tested better with different strategies)
    costDB_->setSearchStrategy(strategy_);
    
    EntryKeyProperty* searchKeyProperty = 
        EntryKeyProperty::create(CostDBTypes::EK_RFILE);
    searchKeyProperty->createFieldProperty(CostDBTypes::EKF_NUM_REGISTERS);
    
    // Creates the search key.
    CostDBEntryKey* searchKey = new CostDBEntryKey(searchKeyProperty);  
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(reads),
            searchKeyProperty->fieldProperty(CostDBTypes::EKF_READ_PORTS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(writes),
            searchKeyProperty->fieldProperty(CostDBTypes::EKF_WRITE_PORTS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(bitWidth),
            searchKeyProperty->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    // Create the searh type of the querie (Exact match)
    CostDBTypes::MatchTypeTable match;    
    match.push_back(
        new MatchType(searchKeyProperty->
                      fieldProperty(CostDBTypes::EKF_BIT_WIDTH), 
                      CostDBTypes::MATCH_EXACT));
    
    // Perform a database query.
    CostDBTypes::EntryTable results;
    results = costDB_->search(*searchKey, match);
    
    TS_ASSERT(results.size() == 2);

    TS_ASSERT(results[0]->isEqualKey(*rf1));
    TS_ASSERT(results[1]->isEqualKey(*rf2));
}


#endif
