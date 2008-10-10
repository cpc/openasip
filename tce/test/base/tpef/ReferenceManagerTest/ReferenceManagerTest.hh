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
 * @file ReferenceManagerTest.hh 
 *
 * A test suite for ReferenceManager. Tests SafePointer and
 * reference manager co-operation.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 26 Sep 2003 by rm, jn
 *
 */

#ifndef TTA_REFERENCE_MANAGER_TEST_HH
#define TTA_REFERENCE_MANAGER_TEST_HH

#include <cassert>
#include <cstddef> // NULL

#define CXXTEST_ABORT_TEST_ON_FAIL
#include <TestSuite.h>

#include <iostream>
using std::cerr;
using std::endl;

#include "SafePointer.hh"
#include "Exception.hh"
#include "DataSection.hh"
#include "CodeSection.hh"
#include "ReferenceKey.hh"
#include "Chunk.hh"
#include "Application.hh" // debugLog

using namespace TPEF;
using namespace ReferenceManager;

/**
 * Implements tests needed to verify correctness of reference managing.
 */
class ReferenceManagerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testReferenceKeyEqualityOperator();
    void testPointerRequestWithSIKey();
    void testPointerRequestWithSOKey();
    void testPointerRequestWithSKey();
    void testPointerRequestWithFOKey();
    void testPointerRequestWithReferences();
    void testReferenceManagingUsingSIKeys();
    void testReferenceManagingWithPreReferences();
    void testReferenceManagingWithAllKeyTypes();
    void testReferenceManagingWhenReferencesRegistered();
    void testObjectKeyLookupWhenKeysAreFound();
    void testObjectKeyLookupWhenKeysAreNotFound();
    void testIsReferencedMethod();
    void testCleanupKeyTables();
    void testResolveWhenNoErrorsOccur();
    void testResolveWhenSectionCannotBeFound();
    void testResolveWhenSectionIsNotRaw();
    void testResolveWhenEverythingCannotBeResolved();
    void testInformDeletedSafePointable();
    void testInformDeletedSafePointer();
    void testMultipleReferenceRegistrationsForSameKey();
    void testCleanUp();

private:
    /// Null safepointable.
    static const SafePointable* nullSafePointable;

    /// Dummy SafePointable class for testing.
    class DummySafePointable : public SafePointable {
    };

};

const SafePointable* ReferenceManagerTest::nullSafePointable =
static_cast<SafePointable*>(NULL);

/**
 * Called before each test.
 */
inline 
void 
ReferenceManagerTest::setUp() {
}

/**
 * Cleans up memory allocations of ReferenceManager.
 */
inline 
void 
ReferenceManagerTest::tearDown() {

    SafePointer::cleanup();
}

/**
 * Tests the equality and inequality operators of different ReferenceKeys.
 */
inline
void 
ReferenceManagerTest::testReferenceKeyEqualityOperator() {

    SectionIndexKey SIKey1(1,  1), SIKey2(1, 1), SIKey3(1, 2);
    TS_ASSERT(SIKey1 == SIKey2);
    TS_ASSERT(SIKey1 != SIKey3);

    SectionOffsetKey SOKey1(1, 1), SOKey2(1, 1), SOKey3(1, 2);
    TS_ASSERT(SOKey1 == SOKey2);
    TS_ASSERT(SOKey1 != SOKey3);

    SectionKey SKey1(1), SKey2(1), SKey3(2);
    TS_ASSERT(SKey1 == SKey2);
    TS_ASSERT(SKey1 != SKey3);

    FileOffsetKey FOKey1(1), FOKey2(1), FOKey3(2);
    TS_ASSERT(FOKey1 == FOKey2);
    TS_ASSERT(FOKey1 != FOKey3);    
}

//@{
/**
 * testPointerRequest* methods test that SafePointers are returned 
 * and saved correctly using some different keys.
 */

/**
 * Test that SafePointer is returned correctly using SectionIndexKey.
 */
inline void 
ReferenceManagerTest::testPointerRequestWithSIKey() {

    // create dummy SectionIndexKey and request two SafePointers with it
    SectionIndexKey key(0, 2);
	
    SafePointer* a = CREATE_SAFEPOINTER(key);
    SafePointer* b = CREATE_SAFEPOINTER(key);

    TS_ASSERT_DIFFERS(a, b);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMap()->size()), 1);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMapAt(key)->length()), 2);
    TS_ASSERT_EQUALS(a->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(b->pointer(), nullSafePointable);
}

/**
 * Test that SafePointer is returned correctly using SectionOffsetKey.
 */
inline void 
ReferenceManagerTest::testPointerRequestWithSOKey() {

    // create dummy SectionOffsetKey and request two SafePointers with it
    SectionOffsetKey key(3, 2123);

    SafePointer* a = CREATE_SAFEPOINTER(key);
    SafePointer* b = CREATE_SAFEPOINTER(key);

    TS_ASSERT_DIFFERS(a, b);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SOMap()->size()), 1);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SOMapAt(key)->length()), 2);
    TS_ASSERT_EQUALS(a->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(b->pointer(), nullSafePointable);
}

/**
 * Test that SafePointer is returned correctly using SectionKey.
 */
inline void 
ReferenceManagerTest::testPointerRequestWithSKey() {

    // create dummy SectionKey and request two SafePointers with it
    SectionKey key(21222);
	
    SafePointer* a = CREATE_SAFEPOINTER(key);
    SafePointer* b = CREATE_SAFEPOINTER(key);

    TS_ASSERT_DIFFERS(a, b);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMap()->size()), 1);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMapAt(key)->length()), 2);
    TS_ASSERT_EQUALS(a->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(b->pointer(), nullSafePointable);
}

/**
 * Test that SafePointer is returned correctly using FileOffsetKey.
 */
inline void 
ReferenceManagerTest::testPointerRequestWithFOKey() {

    // create dummy FileOffsetKey and request two SafePointers with it
    FileOffsetKey key(12312222);
	
    SafePointer* a = CREATE_SAFEPOINTER(key);
    SafePointer* b = CREATE_SAFEPOINTER(key);

    TS_ASSERT_DIFFERS(a, b);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::FOMap()->size()), 1);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::FOMapAt(key)->length()), 2);
    TS_ASSERT_EQUALS(a->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(b->pointer(), nullSafePointable);
}


/**
 * Test that SafePointer is returned correctly using reference to the object.
 */
inline void 
ReferenceManagerTest::testPointerRequestWithReferences() {
    
    // create dummy (fake) object references and request 
    // SafePointers for them
    SafePointable* fake = reinterpret_cast<SafePointable*>(0x420);
    SafePointable* fake2 = reinterpret_cast<SafePointable*>(0x421);
    SafePointable* fake3 = fake;
 
    SafePointer* a = CREATE_SAFEPOINTER(fake);
    SafePointer* b = CREATE_SAFEPOINTER(fake2);
    SafePointer* c = CREATE_SAFEPOINTER(fake3);

    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::RMap()->size()), 2);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SOMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::FOMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMap()->size()), 0);
    TS_ASSERT_EQUALS(a->pointer(), fake);
    TS_ASSERT_EQUALS(b->pointer(), fake2);
    TS_ASSERT_EQUALS(c->pointer(), fake);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::RMapAt(fake)->length()), 2);
}   
//@}

//@{
/**
 * testReferenceManaging* methods test that keys are connected to objects
 * correctly.
 */

/**
 * Test that reference managing is working using SectionIndex keys.
 */
inline void 
ReferenceManagerTest::testReferenceManagingUsingSIKeys() {

    // create dummy SectionIndexKey and request 5 SafePointers with it
    SectionIndexKey key(21, 2);

    const unsigned int SAFEPOINTER_COUNT = 5;
    SafePointer* sp[SAFEPOINTER_COUNT] = {
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key)};

    for (unsigned i = 0; i < SAFEPOINTER_COUNT; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), nullSafePointable);
    }

    // make sure that initially there's no entry in reference map
    SafePointable* fake = (SafePointable*)0x666;

    TS_ASSERT_EQUALS(SafePointer::RMapAt(fake), 
		     reinterpret_cast<SafePointerList*>(NULL));

    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMapAt(key)->length()), 5);

    // insert a new entry for 'fake' and make sure it's inserted into 
    // the reference map and SectionIndexKeys we created earlier are
    // now connected to 'fake'
    SafePointer::addObjectReference(key, fake);
    
    TS_ASSERT_EQUALS(SafePointer::SIMapAt(key), 
		     SafePointer::RMapAt(fake));

    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMapAt(key)->length()), 5);

    for (unsigned i = 0; i < SAFEPOINTER_COUNT; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), fake);
    }
}

/**
 * Test that reference managing is working when SafePointers are requested
 * both with real reference and with (SectionIndex)key reference to the 
 * same object.
 */
inline void 
ReferenceManagerTest::testReferenceManagingWithPreReferences() {

    SectionIndexKey key(211, 21);
    SafePointable* fake = reinterpret_cast<SafePointable*>(0x7777);

    const unsigned int SAFEPOINTER_COUNT = 5;    
    SafePointer* sp[SAFEPOINTER_COUNT] = {
	CREATE_SAFEPOINTER(fake),
	CREATE_SAFEPOINTER(fake),
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key),
	CREATE_SAFEPOINTER(key)};
    
    for (unsigned i = 0; i < 2; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), fake);
    }

    for (unsigned i = 2; i < SAFEPOINTER_COUNT; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), nullSafePointable);
    }

    // after this call all keys created earlier should be connected to fake
    SafePointer::addObjectReference(key, fake);

    for (unsigned i = 0; i < SAFEPOINTER_COUNT; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), fake);
    }

    TS_ASSERT_EQUALS(SafePointer::SIMapAt(key), SafePointer::RMapAt(fake));    
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMapAt(key)->length()), 5);
}

/**
 * Test that reference managing is working when SafePointers are requested
 * with real reference and with every different type of key. 
 */
inline void 
ReferenceManagerTest::testReferenceManagingWithAllKeyTypes() {

    SafePointable* fake = reinterpret_cast<SafePointable*>(0x0700f001);
    SectionIndexKey SIKey(53132, 122321);
    SectionOffsetKey SOKey(2231, 3421);
    SectionKey SKey(9465);
    FileOffsetKey FOKey(9575625);

    const unsigned int KEY_COUNT = 10;
    SafePointer* sp[KEY_COUNT] = {
	CREATE_SAFEPOINTER(fake),
	CREATE_SAFEPOINTER(fake),
	CREATE_SAFEPOINTER(SIKey),
	CREATE_SAFEPOINTER(SIKey),
	CREATE_SAFEPOINTER(SOKey),
	CREATE_SAFEPOINTER(SOKey),
	CREATE_SAFEPOINTER(FOKey),
	CREATE_SAFEPOINTER(FOKey),
	CREATE_SAFEPOINTER(SKey),
	CREATE_SAFEPOINTER(SKey)};

    SafePointer::addObjectReference(SIKey, fake);
    TS_ASSERT_EQUALS(SafePointer::SIMapAt(SIKey), SafePointer::RMapAt(fake));

    SafePointer::addObjectReference(SOKey, fake);
    TS_ASSERT_EQUALS(SafePointer::SOMapAt(SOKey), SafePointer::RMapAt(fake));

    SafePointer::addObjectReference(FOKey, fake);
    TS_ASSERT_EQUALS(SafePointer::FOMapAt(FOKey), SafePointer::RMapAt(fake));

    SafePointer::addObjectReference(SKey, fake);
    TS_ASSERT_EQUALS(SafePointer::SMapAt(SKey), SafePointer::RMapAt(fake));

    for (unsigned i = 0; i < KEY_COUNT; i++) {
	TS_ASSERT_EQUALS(sp[i]->pointer(), fake);
    }

    TS_ASSERT_EQUALS(SafePointer::SIMapAt(SIKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::SOMapAt(SOKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::FOMapAt(FOKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::SMapAt(SKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::SIMapAt(SIKey)->length(), KEY_COUNT);
}

/**
 * Test that reference managing is working when SafePointers are requested
 * after references are added.
 */
inline void 
ReferenceManagerTest::testReferenceManagingWhenReferencesRegistered() {

    SafePointable* fake = reinterpret_cast<SafePointable*>(0x07cef001);
    SectionIndexKey SIKey(53632, 12321221);
    SectionOffsetKey SOKey(2231, 3521);
    SectionKey SKey(9465);
    FileOffsetKey FOKey(953275625);

    SafePointer::addObjectReference(SIKey, fake);
    SafePointer::addObjectReference(SOKey, fake);
    SafePointer::addObjectReference(SKey, fake);
    SafePointer::addObjectReference(FOKey, fake);

    TS_ASSERT_EQUALS(SafePointer::SIMapAt(SIKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::SOMapAt(SOKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::FOMapAt(FOKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(SafePointer::SMapAt(SKey), SafePointer::RMapAt(fake));
    TS_ASSERT_EQUALS(
        static_cast<int>(SafePointer::SIMapAt(SIKey)->length()), 0);

    SafePointer* newPointer = CREATE_SAFEPOINTER(SIKey);
    
    TS_ASSERT_EQUALS(newPointer->pointer(), fake);
}  
//@}

/**
 * Test that keys are returned for the object when keys are found. 
 * Try with all key types.
 */
inline void 
ReferenceManagerTest::testObjectKeyLookupWhenKeysAreFound() {

    SafePointable* fake = reinterpret_cast<SafePointable*>(0x07cef001);
    SectionIndexKey SIKey(53632, 12321221);
    SectionOffsetKey SOKey(231, 321);
    SectionKey SKey(9435);
    FileOffsetKey FOKey(953275625);

    SafePointer::addObjectReference(SIKey, fake);
    SafePointer::addObjectReference(SOKey, fake);
    SafePointer::addObjectReference(SKey, fake);
    SafePointer::addObjectReference(FOKey, fake);

    TS_ASSERT_EQUALS(SafePointer::sectionIndexKeyFor(fake), SIKey);
    TS_ASSERT_EQUALS(SafePointer::sectionOffsetKeyFor(fake), SOKey);
    TS_ASSERT_EQUALS(SafePointer::sectionKeyFor(fake), SKey);
    TS_ASSERT_EQUALS(SafePointer::fileOffsetKeyFor(fake), FOKey);

    // make sure there's no "dummy" SafePointer in the list because we
    // haven't requested one 
    TS_ASSERT_EQUALS(
        static_cast<int>(SafePointer::SIMapAt(SIKey)->length()), 0);
}

/**
 * Test that exception is thrown when key is requested for an object and
 * it cannot be found. Try with all key types.
 */
inline void 
ReferenceManagerTest::testObjectKeyLookupWhenKeysAreNotFound() {

    SafePointable* fake = reinterpret_cast<SafePointable*>(0x07cef001);
    SectionIndexKey SIKey(53632, 12321221);
    SectionOffsetKey SOKey(2231, 321);
    SectionKey SKey(9385);
    FileOffsetKey FOKey(953275625);

    TS_ASSERT_THROWS(SafePointer::fileOffsetKeyFor(fake), KeyNotFound);
    TS_ASSERT_THROWS(SafePointer::sectionIndexKeyFor(fake), KeyNotFound);
    TS_ASSERT_THROWS(SafePointer::sectionOffsetKeyFor(fake), KeyNotFound);
    TS_ASSERT_THROWS(SafePointer::sectionKeyFor(fake), KeyNotFound);

    SafePointer::addObjectReference(SIKey, fake);
    TS_ASSERT_THROWS_NOTHING(SafePointer::sectionIndexKeyFor(fake));

    SafePointer::addObjectReference(SOKey, fake);
    TS_ASSERT_THROWS_NOTHING(SafePointer::sectionOffsetKeyFor(fake));

    SafePointer::addObjectReference(SKey, fake);
    TS_ASSERT_THROWS_NOTHING(SafePointer::sectionKeyFor(fake));

    SafePointer::addObjectReference(FOKey, fake);
    TS_ASSERT_THROWS_NOTHING(SafePointer::fileOffsetKeyFor(fake));
}

/**
 * Test that SafePointer::isReferenced() tells correctly if the object is 
 * referenced or not.
 */
inline
void 
ReferenceManagerTest::testIsReferencedMethod() {
    SafePointable* fake1 = reinterpret_cast<SafePointable*>(0x0712f341);
    SafePointable* fake2 = reinterpret_cast<SafePointable*>(0x0712f342);

    SafePointer* test = CREATE_SAFEPOINTER(fake1);
    
    TS_ASSERT(SafePointer::isReferenced(fake1));
    TS_ASSERT(!SafePointer::isReferenced(fake2));

    delete test;
    test = NULL;

    TS_ASSERT(!SafePointer::isReferenced(fake1));
}


/**
 * Test that key tables are cleaned with SafePointer::cleanupKeyTables().
 */
inline void 
ReferenceManagerTest::testCleanupKeyTables() {

    SafePointable* fake = reinterpret_cast<SafePointable*>(0x0712f341);
    SectionIndexKey SIKey(58532, 342111);
    SectionOffsetKey SOKey(2431, 34331);
    SectionKey SKey(9165);
    FileOffsetKey FOKey(9323225);

    CREATE_SAFEPOINTER(fake);
    CREATE_SAFEPOINTER(fake);
    CREATE_SAFEPOINTER(SIKey);
    CREATE_SAFEPOINTER(SIKey);
    CREATE_SAFEPOINTER(SOKey);
    CREATE_SAFEPOINTER(SOKey);
    CREATE_SAFEPOINTER(FOKey);
    CREATE_SAFEPOINTER(FOKey);
    CREATE_SAFEPOINTER(SKey);
    CREATE_SAFEPOINTER(SKey);

    SafePointer::addObjectReference(SIKey, fake);
    
    SafePointer::cleanupKeyTables();
	
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::FOMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SOMap()->size()), 0);

    // cleanupKeyTables() should only clean key tables, not the reference map
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::RMapAt(fake)->length()), 4);
}

/**
 * Test that reference managing is working when resolving references
 * to a chunkable section.
 */
inline void 
ReferenceManagerTest::testResolveWhenNoErrorsOccur() {

    const unsigned int SECTION_ID = 100;
    const unsigned int KEY_COUNT = 5;
    const unsigned int RAW_DATA_SIZE = 150000;
    const SectionOffset offsets[KEY_COUNT] = {0, 100, 1000, 10000, 100000};

    SafePointer* sp[KEY_COUNT];

    for (unsigned int i = 0; i < KEY_COUNT; i++) {
	sp[i] = CREATE_SAFEPOINTER(SectionOffsetKey(SECTION_ID, offsets[i]));
	TS_ASSERT_DIFFERS(sp[i], static_cast<SafePointer*>(NULL));
    }

    // NOTE! we are trusting that next call works and ST_DATA is chunkable:
    DataSection* chunkableSection = 
	dynamic_cast<DataSection*>(Section::createSection(Section::ST_DATA));

    assert(chunkableSection != NULL);

    for (unsigned int i=0;i < RAW_DATA_SIZE;i++) {
	chunkableSection->addByte(0);
    }

    SafePointer::addObjectReference(SectionKey(SECTION_ID), chunkableSection);
    
    TS_ASSERT_EQUALS(SafePointer::RMapAt(chunkableSection),
		     SafePointer::SMapAt(SectionKey(SECTION_ID)));
    TS_ASSERT_EQUALS(
        static_cast<int>(SafePointer::SMapAt(
                             SectionKey(SECTION_ID))->length()), 0);
    
    // resolving should work because we have added reference to the section
    // where there are unresolved elements and that section is also chunkable
    TS_ASSERT_THROWS_NOTHING(SafePointer::resolve());

    /// @todo add a test (and code that complies to that test) where a chunk
    /// is requested with a greater offset than data size in the raw section.
    
    // make sure chunks are created as they should have
    for (unsigned int i = 0; i < KEY_COUNT; i++) {
	TS_ASSERT_DIFFERS(sp[i]->pointer(), nullSafePointable);
	Chunk* aChunk = dynamic_cast<Chunk*>(sp[i]->pointer());
	TS_ASSERT_EQUALS(aChunk->offset(), offsets[i]);
    }

    delete chunkableSection;
    chunkableSection = NULL;
}

/**
 * Test that reference managing throws exceptions when trying to resolve
 * unresolvable references and section from which to request chunk from
 * cannot be found.
 */
inline void 
ReferenceManagerTest::testResolveWhenSectionCannotBeFound() {
    CREATE_SAFEPOINTER(SectionOffsetKey(123, 12));
    TS_ASSERT_THROWS(SafePointer::resolve() , UnresolvedReference);
}

/**
 * Test that reference managing throws exceptions when trying to resolve
 * unresolvable references and section from which to request chunk from
 * in not a raw section.
 */
inline void 
ReferenceManagerTest::testResolveWhenSectionIsNotRaw() {

    CodeSection* nonChunkableSection =
	dynamic_cast<CodeSection*>(Section::createSection(Section::ST_CODE));
    
    assert(nonChunkableSection != NULL);

    CREATE_SAFEPOINTER(SectionOffsetKey(1, 12));
    SafePointer::addObjectReference(SectionKey(1), nonChunkableSection);

    TS_ASSERT_THROWS(SafePointer::resolve(), UnresolvedReference);

    delete nonChunkableSection;
    nonChunkableSection = NULL;
}

/**
 * Test that reference managing throws exceptions when trying to resolve
 * unresolvable references when at the end of the resolve() unresolved 
 * references are still found. 
 *
 * Test with all the different key(/map) types.
 */
inline void 
ReferenceManagerTest::testResolveWhenEverythingCannotBeResolved() {

    CREATE_SAFEPOINTER(SectionIndexKey(666, 888));
    TS_ASSERT_THROWS(SafePointer::resolve(), UnresolvedReference);
    
    SafePointer::cleanupKeyTables();

    CREATE_SAFEPOINTER(FileOffsetKey(666888));
    TS_ASSERT_THROWS(SafePointer::resolve(), UnresolvedReference);

    SafePointer::cleanupKeyTables();

    CREATE_SAFEPOINTER(SectionKey(6688));
    TS_ASSERT_THROWS(SafePointer::resolve(), UnresolvedReference);

    SafePointer::cleanupKeyTables();

    CREATE_SAFEPOINTER(SectionIndexKey(6688, 1));
    TS_ASSERT_THROWS(SafePointer::resolve(), UnresolvedReference);

    SafePointer::cleanupKeyTables();
    SafePointer::cleanup();
}

/**
 * Test that SafePointable informs of its deletion correctly and necessary
 * operations are done.
 */
inline void 
ReferenceManagerTest::testInformDeletedSafePointable() {

    SafePointable* dummy = new DummySafePointable();

    SectionIndexKey SIKey(0xfe, 0x12e);
    SectionOffsetKey SOKey(0x12, 0x3443);
    SectionKey SKey(0x12f);
    FileOffsetKey FOKey(0x121ffe);

    SafePointer* testSIP = CREATE_SAFEPOINTER(SIKey);
    SafePointer* testSOP = CREATE_SAFEPOINTER(SOKey);
    SafePointer* testSP = CREATE_SAFEPOINTER(SKey);
    SafePointer* testFOP = CREATE_SAFEPOINTER(FOKey);

    SafePointer::addObjectReference(SIKey, dummy);
    SafePointer::addObjectReference(SOKey, dummy);
    SafePointer::addObjectReference(SKey, dummy);
    SafePointer::addObjectReference(FOKey, dummy);
    
    delete dummy;
    dummy = NULL;

    // reference manager should set all SafePointers that were pointing
    // to the deleted SafePointable object to point to NULL
    TS_ASSERT_EQUALS(testSIP->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(testSOP->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(testSP->pointer(), nullSafePointable);
    TS_ASSERT_EQUALS(testFOP->pointer(), nullSafePointable);
    
    // dummy should be removed from the reference map
    TS_ASSERT_EQUALS(SafePointer::RMapAt(dummy), 
		     static_cast<SafePointerList*>(NULL));
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMapAt(SKey)->length()), 0);
}

/**
 * Test that SafePointer informs of its deletion correctly and necessary
 * operations are done.
 */
inline void 
ReferenceManagerTest::testInformDeletedSafePointer() {

    FileOffsetKey FOKey(0x121fefe);
    SafePointer* testFOP = CREATE_SAFEPOINTER(FOKey);
    delete testFOP;
    testFOP = NULL;

    // SafePointer should be removed from the set of alive safepointers
    TS_ASSERT(!SafePointer::isAlive(testFOP));
}

/**
 * Test that reference manager does not allow registering a new object for
 * the same key.
 */
inline void 
ReferenceManagerTest::testMultipleReferenceRegistrationsForSameKey() {

    FileOffsetKey FOKey(0x121fefe);
    CREATE_SAFEPOINTER(FOKey);
    
    SafePointable* dummy1 = new DummySafePointable();

    TS_ASSERT_THROWS_NOTHING(SafePointer::addObjectReference(FOKey, dummy1));

    TS_ASSERT_THROWS(SafePointer::addObjectReference(FOKey, dummy1),
		     KeyAlreadyExists);

    SafePointable* dummy2 = new DummySafePointable();
    TS_ASSERT_THROWS(SafePointer::addObjectReference(FOKey, dummy2),
		     KeyAlreadyExists);
    delete dummy2;
    dummy2 = NULL;

    delete dummy1;
    dummy1 = NULL;
}

/**
 * Test that cleanup() cleans up all the key and reference table.
 */
inline void 
ReferenceManagerTest::testCleanUp() {

    // make sure there's some garbage in all the maps
    SafePointable* fake = reinterpret_cast<SafePointable*>(0x0712f341);
    SectionIndexKey SIKey(58532, 342111);
    SectionOffsetKey SOKey(22231, 3331);
    SectionKey SKey(9465);
    FileOffsetKey FOKey(9323225);

    CREATE_SAFEPOINTER(fake);
    CREATE_SAFEPOINTER(fake);
    CREATE_SAFEPOINTER(SIKey);
    CREATE_SAFEPOINTER(SIKey);
    CREATE_SAFEPOINTER(SOKey);
    CREATE_SAFEPOINTER(SOKey);
    CREATE_SAFEPOINTER(FOKey);
    CREATE_SAFEPOINTER(FOKey);
    CREATE_SAFEPOINTER(SKey);
    CREATE_SAFEPOINTER(SKey);

    SafePointer::addObjectReference(SIKey, fake);

    SafePointer::cleanup();
	
    // Now all maps should be empty, all safepointer lists should have been
    // cleared and safepointers in them deleted. One should verify with some
    // memory leak check tool like Valgrind if there's something left 
    // allocated.
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SIMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::FOMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::SOMap()->size()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(SafePointer::RMap()->size()), 0);
}


#endif
