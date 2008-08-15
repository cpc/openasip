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
 * @file InterpolationTest.hh 
 *
 * A test suite for Interpolation class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 */    

#ifndef TTA_COSTDB_INTERPOLATION_TEST_HH
#define TTA_COSTDB_INTERPOLATION_TEST_HH

#include <string>
#include <TestSuite.h>
#include "CostDBEntryKey.hh"
#include "CostDBTypes.hh"
#include "Interpolation.hh"
#include "EntryKeyFieldProperty.hh"
#include "EntryKeyProperty.hh"
#include "EntryKeyField.hh"
#include "Exception.hh"

using std::string;


/**
 * Tests for verify interpolation in costdb.
 */
class InterpolationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testInterpolate();
    void testExactMatch();
private:

};


/**
 * Called before test.
 */
void
InterpolationTest::setUp() {

}


/**
 * Called after test.
 */
void InterpolationTest::tearDown() {

}


/**
 * Test that tests the Interpolation class with data that needs to be
 * interpolated.
 */
void InterpolationTest::testInterpolate() {
    
    string type = "test";
    EntryKeyProperty* entryKeyProperty = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty1 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty2 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty3 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty4 = EntryKeyProperty::create(type);
    
    EntryKeyFieldProperty* entryKeyFieldProperty = 
        entryKeyProperty->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty1 = 
        entryKeyProperty1->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty2 = 
        entryKeyProperty2->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty3 = 
        entryKeyProperty3->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty4 = 
        entryKeyProperty4->createFieldProperty(type);
    
    Interpolation interpolation(entryKeyFieldProperty4);

    CostDBEntryKey* entryKey1 = new CostDBEntryKey(entryKeyProperty1);
    CostDBEntryKey* entryKey2 = new CostDBEntryKey(entryKeyProperty2);
    CostDBEntryKey* entryKey3 = new CostDBEntryKey(entryKeyProperty3);
    
    EntryKeyData* fieldData1 = new EntryKeyDataInt(7);
    EntryKeyData* fieldData2 = new EntryKeyDataInt(9);
    EntryKeyData* fieldData3 = new EntryKeyDataInt(15);

    EntryKeyField* field1 = 
        new EntryKeyField(fieldData1, entryKeyFieldProperty1);
    EntryKeyField* field2 = 
        new EntryKeyField(fieldData2, entryKeyFieldProperty2);
    EntryKeyField* field3 = 
        new EntryKeyField(fieldData3, entryKeyFieldProperty3);
    entryKey1->addField(field1);
    entryKey2->addField(field2);

    CostDBTypes::EntryTable entryTable;
    CostDBEntry* entry1 = new CostDBEntry(entryKey1);
    CostDBEntry* entry2 = new CostDBEntry(entryKey2);
    CostDBEntry* entry3 = new CostDBEntry(entryKey3);

    entryTable.push_back(entry1);
    entryTable.push_back(entry2);
    entryTable.push_back(entry3);

    EntryKeyData* fieldData = new EntryKeyDataInt(10);
    EntryKeyField* searchField = 
        new EntryKeyField(fieldData, entryKeyFieldProperty);    
    CostDBEntryKey* searchKey = new CostDBEntryKey(entryKeyProperty);
    searchKey->addField(searchField);

    // should notice that entry3 has no proper EntryKeyField set
    TS_ASSERT_THROWS(interpolation.filter(*searchKey, entryTable), KeyNotFound);

    // set the EntryKeyField to entry3
    entryKey3->addField(field3);
    interpolation.filter(*searchKey, entryTable);

    unsigned int size = 1;
    TS_ASSERT_EQUALS(entryTable.size(), size);
    CostDBTypes::EntryTable::iterator i = entryTable.begin();
    for (; i < entryTable.end(); i++) {
        // should not be any of the original entries
        TS_ASSERT_DIFFERS((*i), entry1);
        TS_ASSERT_DIFFERS((*i), entry2);
        TS_ASSERT_DIFFERS((*i), entry3);
        for (int n = 0; n < (*i)->fieldCount(); n++) {
            // EntryKeyField should be equal to searched EntryKeyField
            TS_ASSERT((*i)->field(n).isEqual(*searchField));
        }
    }

    delete entry1;
    entry1 = NULL;
    delete entry2;
    entry2 = NULL;
    delete entry3;
    entry3 = NULL;
    delete searchKey;
    searchKey = NULL;
}

/**
 * Test that tests the Interpolation class with exact match entry given to
 * filter.
 */
void InterpolationTest::testExactMatch() {
    
    string type = "test";
    EntryKeyProperty* entryKeyProperty = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty1 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty2 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty3 = EntryKeyProperty::create(type);
    EntryKeyProperty* entryKeyProperty4 = EntryKeyProperty::create(type);
    
    EntryKeyFieldProperty* entryKeyFieldProperty = 
        entryKeyProperty->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty1 = 
        entryKeyProperty1->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty2 = 
        entryKeyProperty2->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty3 = 
        entryKeyProperty3->createFieldProperty(type);
    EntryKeyFieldProperty* entryKeyFieldProperty4 = 
        entryKeyProperty4->createFieldProperty(type);
    
    Interpolation interpolation(entryKeyFieldProperty4);

    CostDBEntryKey* entryKey1 = new CostDBEntryKey(entryKeyProperty1);
    CostDBEntryKey* entryKey2 = new CostDBEntryKey(entryKeyProperty2);
    CostDBEntryKey* entryKey3 = new CostDBEntryKey(entryKeyProperty3);
    
    EntryKeyData* fieldData1 = new EntryKeyDataInt(7);
    EntryKeyData* fieldData2 = new EntryKeyDataInt(9);
    EntryKeyData* fieldData3 = new EntryKeyDataInt(10);

    EntryKeyField* field1 = 
        new EntryKeyField(fieldData1, entryKeyFieldProperty1);
    EntryKeyField* field2 = 
        new EntryKeyField(fieldData2, entryKeyFieldProperty2);
    EntryKeyField* field3 = 
        new EntryKeyField(fieldData3, entryKeyFieldProperty3);
    entryKey1->addField(field1);
    entryKey2->addField(field2);

    CostDBTypes::EntryTable entryTable;
    CostDBEntry* entry1 = new CostDBEntry(entryKey1);
    CostDBEntry* entry2 = new CostDBEntry(entryKey2);
    CostDBEntry* entry3 = new CostDBEntry(entryKey3);

    entryTable.push_back(entry1);
    entryTable.push_back(entry2);
    entryTable.push_back(entry3);

    EntryKeyData* fieldData = new EntryKeyDataInt(10);
    EntryKeyField* searchField = 
        new EntryKeyField(fieldData, entryKeyFieldProperty);    
    CostDBEntryKey* searchKey = new CostDBEntryKey(entryKeyProperty);
    searchKey->addField(searchField);

    // should notice that entry3 has no proper EntryKeyField set
    TS_ASSERT_THROWS(interpolation.filter(*searchKey, entryTable), KeyNotFound);

    // set the EntryKeyField to entry3
    entryKey3->addField(field3);
    interpolation.filter(*searchKey, entryTable);

    unsigned int size = 1;
    TS_ASSERT_EQUALS(entryTable.size(), size);
    CostDBTypes::EntryTable::iterator i = entryTable.begin();
    for (; i < entryTable.end(); i++) {
        // should match with the entry3
        TS_ASSERT_DIFFERS((*i), entry1);
        TS_ASSERT_DIFFERS((*i), entry2);
        TS_ASSERT_EQUALS((*i), entry3);
        for (int n = 0; n < (*i)->fieldCount(); n++) {
            // EntryKeyField should be equal to searched EntryKeyField
            TS_ASSERT((*i)->field(n).isEqual(*searchField));
        }
    }

    delete entry1;
    entry1 = NULL;
    delete entry2;
    entry2 = NULL;
    delete entry3;
    entry3 = NULL;
    delete searchKey;
    searchKey = NULL;
}

#endif
