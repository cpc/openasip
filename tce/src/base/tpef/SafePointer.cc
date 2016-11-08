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
 * @file SafePointer.cc
 *
 * Implementation of the SafePointer class.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 29 Aug 2003 by rm, ml, ac, tr
 *
 * @note rating: yellow
 */

#include <set>
using std::set;

#include <cassert>
#include <iterator>
#include <sstream>

using std::insert_iterator;

#include <algorithm>
using std::copy;

#include "SafePointer.hh"
#include "Section.hh"
#include "Conversion.hh"
#include "MapTools.hh"
#include "AssocTools.hh"

namespace TPEF {
namespace ReferenceManager {

// static member variable initialization
// These need to be pointers to be able to call SafePointer::cleanup()
// safely from ~Binary() (which can be from global objects aswell).
// Otherwise these globals can be freed before the global Binary is 
// causing invalid free() calls.
SectionIndexMap* SafePointer::sectionIndexMap_ = new SectionIndexMap;
SectionOffsetMap* SafePointer::sectionOffsetMap_ = new SectionOffsetMap;
FileOffsetMap* SafePointer::fileOffsetMap_ = new FileOffsetMap;
SectionMap* SafePointer::sectionMap_= new SectionMap;

SafePointer::KeyForCacheMap* SafePointer::keyForCache_ = 
    new SafePointer::KeyForCacheMap;

// These are constructed to heap to make sure they are not
// destructed before any global or static object.
ReferenceMap* SafePointer::referenceMap_ = new ReferenceMap;
SafePointerSet* SafePointer::aliveSafePointers_ = new SafePointerSet;

//////////////////////////////////////////////////////////////////////////////
// SafePointerList
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 */
SafePointerList::SafePointerList() : reference_(NULL) {
}

/**
 * Copy constructor.
 *
 */
SafePointerList::SafePointerList(SafePointerList &aList) :
    reference_(aList.reference_) {

    for (SafePointerListType::iterator i = aList.list_.begin();
         i != aList.list_.end(); i++) {

        list_.push_back(*i);
    }
}

/**
 * Destructor.
 *
 */
SafePointerList::~SafePointerList() {
}

/**
 * Sets the object all SafePointers in the list are pointing to.
 *
 * @param obj The pointer to the object all SafePointers in the list
 * should be pointing to.
 */
void
SafePointerList::setReference(const SafePointable* obj) {

    reference_ = const_cast<SafePointable*>(obj);

    for (SafePointerListType::iterator i = list_.begin();
         i != list_.end(); i++) {
        if (SafePointer::isAlive((*i))) {
            (*i)->setPointer(reference_);
        }
    }
}

/**
 * Appends a new SafePointer at the end of the list and sets it's reference
 * to the reference of the list.
 *
 * @param newSafePointer The SafePointer to add.
 */
void
SafePointerList::append(SafePointer* newSafePointer) {

    newSafePointer->setPointer(reference());
    list_.push_back(newSafePointer);
}

/**
 * Appends another SafePointerList at the end of the list and sets it's
 * reference to the reference of the list.
 *
 * @param anotherSafePointerList The SafePointerList to add.
 */
void
SafePointerList::append(SafePointerList* anotherSafePointerList) {

    if (anotherSafePointerList == NULL) {
        return;
    }
    anotherSafePointerList->setReference(reference());

    insert_iterator<SafePointerListType> ii(list_,
                                            list_.end());

    copy(anotherSafePointerList->list_.begin(),
         anotherSafePointerList->list_.end(), ii);

}

/**
 * Sets all SafePointers in the list NULL and clears the list.
 */
void
SafePointerList::cleanup() {
    setReference(NULL);
    list_.clear();
}

/**
 * Removes all dead (deleted) SafePointers from the list.
 */
void
SafePointerList::cleanupDead() {


    for (SafePointerListType::iterator i = list_.begin();
         i != list_.end();) {
        if (!SafePointer::isAlive((*i))) {
            i = list_.erase(i);
        } else {
            ++i;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// SafePointer
//////////////////////////////////////////////////////////////////////////////

const SafePointer
SafePointer::null(static_cast<SafePointable*>(NULL));

/**
 * Construct a SafePointer using SectionIndexKey.
 *
 * @param key Key to use while requesting the reference.
 */
SafePointer::SafePointer(SectionIndexKey key) :
    object_(NULL) {

    genericRegisterPointer(key, *sectionIndexMap_, this);
}

/**
 * Construct a SafePointer using SectionOffsetKey.
 *
 * @param key Key to use while requesting the reference.
 */
SafePointer::SafePointer(SectionOffsetKey key) :
    object_(NULL) {

    genericRegisterPointer(key, *sectionOffsetMap_, this);
}

/**
 * Construct a SafePointer using FileOffsetKey.
 *
 * @param key Key to use while requesting the reference.
 */
SafePointer::SafePointer(FileOffsetKey key) :
    object_(NULL) {

    genericRegisterPointer(key, *fileOffsetMap_, this);
}

/**
 * Construct a SafePointer using section reference.
 *
 * @param key Key object to use while requesting the reference.
 */
SafePointer::SafePointer(SectionKey key) :
    object_(NULL) {

    genericRegisterPointer(key, *sectionMap_, this);
}

/**
 * Construct a SafePointer using object reference.
 *
 * @param object Object to use while requesting the reference.
 */
SafePointer::SafePointer(SafePointable* object) :
    object_(object) {

    if (object != NULL) {
        genericRegisterPointer(object, *referenceMap_, this);
    }
}

/**
 * Destructor.
 *
 * Informs of deletion of itself to the reference manager.
 */
SafePointer::~SafePointer() {
    notifyDeleted(this);
}

/**
 * Check if SafePointer is alive, that is it's not deleted yet.
 *
 * @param pointerToCheck The pointer to check.
 * @return True if SafePointer is alive.
 */
bool
SafePointer::isAlive(SafePointer* pointerToCheck) {

    return
        (pointerToCheck != NULL &&
         AssocTools::containsKey(*aliveSafePointers_, pointerToCheck));
}

/**
 * Check if object is referenced, that is reference manager is managing
 * SafePointers that point to that object.
 *
 * @param object Object to check.
 * @return True if object is referenced.
 */
bool
SafePointer::isReferenced(const SafePointable* object) {

    if (!MapTools::containsKey(*referenceMap_, object)) {
        return false;
    }

    SafePointerList* theList = (*referenceMap_)[object];
    theList->cleanupDead();

    return (theList->length() > 0);
}


/**
 * Inform of a connection between a SectionIndexKey and an object.
 *
 * Connects given key with object and sets possible old SafePointers
 * connected with the key to point to this object.
 *
 * @param key Key that the object is connected to.
 * @param obj Pointer to the connected object.
 * @exception KeyAlreadyExists If entry with given key already exists.
 */
void
SafePointer::addObjectReference(SectionIndexKey key, const SafePointable* obj)
    throw (KeyAlreadyExists) {

    genericAddObjectReference(key, *sectionIndexMap_, obj);
}

/**
 * Inform of a connection between a SectionOffsetKey and an object.
 *
 * Connects given key with object and sets possible old SafePointers
 * connected with the key to point to this object.
 *
 * @param key Key that the object is connected to.
 * @param obj Pointer to the connected object.
 * @exception KeyAlreadyExists If entry with given key already exists.
 */
void
SafePointer::addObjectReference(
    SectionOffsetKey key, const SafePointable* obj)
    throw (KeyAlreadyExists) {

    genericAddObjectReference(key, *sectionOffsetMap_, obj);
}

/**
 * Inform of a connection between a FileOffsetKey and an object.
 *
 * Connects given key with object and sets possible old SafePointers
 * connected with the key to point to this object.
 *
 * @param key Key that the object is connected to.
 * @param obj Pointer to the connected object.
 * @exception KeyAlreadyExists If entry with given key already exists.
 */
void
SafePointer::addObjectReference(FileOffsetKey key, const SafePointable* obj)
    throw (KeyAlreadyExists) {

    genericAddObjectReference(key, *fileOffsetMap_, obj);
}

/**
 * Inform of a connection between a SectionKey and an object.
 *
 * Connects given key with object and sets possible old SafePointers
 * connected with the key to point to this object.
 *
 * @param key Key that the object is connected to.
 * @param obj Pointer to the connected object.
 * @exception KeyAlreadyExists If entry with given key already exists.
 */
void
SafePointer::addObjectReference(SectionKey key, const SafePointable* obj)
    throw (KeyAlreadyExists) {

    genericAddObjectReference(key, *sectionMap_, obj);
}

/**
 *
 * Returns the SectionIndexKey connected to given object.
 *
 * @return The key.
 * @param obj Object to look the key for.
 * @exception KeyNotFound If the key cannot be found for the object.
 *
 */
SectionIndexKey
SafePointer::sectionIndexKeyFor(const SafePointable* obj)
    throw (KeyNotFound) {

    return genericKeyFor<SectionIndexKey>(obj, *sectionIndexMap_);
}

/**
 *
 * Returns the SectionOffsetKey connected to given object.
 *
 * @return The key.
 * @param obj Object to look the key for.
 * @exception KeyNotFound If the key cannot be found for the object.
 *
 */
SectionOffsetKey
SafePointer::sectionOffsetKeyFor(const SafePointable* obj)
    throw (KeyNotFound) {

    return genericKeyFor<SectionOffsetKey>(obj, *sectionOffsetMap_);
}

/**
 *
 * Returns the FileOffsetKey connected to given object.
 *
 * @return The key.
 * @param obj Object to look the key for.
 * @exception KeyNotFound If the key cannot be found for the object.
 *
 */
FileOffsetKey
SafePointer::fileOffsetKeyFor(const SafePointable* obj)
    throw (KeyNotFound) {

    return genericKeyFor<FileOffsetKey>(obj, *fileOffsetMap_);
}

/**
 *
 * Returns the SectionKey connected to given object.
 *
 * @return The key.
 * @param obj Object to look the key for.
 * @exception KeyNotFound If the key cannot be found for the object.
 *
 */
SectionKey
SafePointer::sectionKeyFor(const SafePointable* obj)
    throw (KeyNotFound) {

    return genericKeyFor<SectionKey>(obj, *sectionMap_);
}

/**
 * Inform of deletion of an object.
 *
 * All SafePointers that are pointing to the deleted object are set
 * to NULL and the object's entry is removed from the ReferenceMap.
 *
 * @param obj Deleted object.
 */
void
SafePointer::notifyDeleted(const SafePointable* obj) {

    if (!MapTools::containsKey(*referenceMap_, obj)) {
        return;
    }

    SafePointerList *listOfObj = (*referenceMap_)[obj];

    assert(listOfObj != NULL);
    listOfObj->cleanup();

    // if the safe pointer list we just cleaned up is not referenced in any
    // map anymore, it can be deleted safely

    // TODO: for hashmap implementation this will take *very* long time
    //       so fix this before change typedefs in SafePointer.hh
    if (!MapTools::containsValue(*sectionMap_,       listOfObj) &&
        !MapTools::containsValue(*sectionIndexMap_,  listOfObj) &&
        !MapTools::containsValue(*sectionOffsetMap_, listOfObj) &&
        !MapTools::containsValue(*fileOffsetMap_,    listOfObj)) {
        delete listOfObj;
    }

    referenceMap_->erase(obj);
}

/**
 * Inform of deletion of a SafePointer.
 *
 * Removes SafePointer from the list of alive SafePointers.
 *
 * @param safePointer The deleted SafePointer.
 */
void
SafePointer::notifyDeleted(SafePointer* safePointer) {

    // the null safe pointer is an exception, it shouldn't be never deleted
    if (safePointer == &null) {
        return;
    }

    unsigned int pointersFound = 0;
    pointersFound = (*aliveSafePointers_).erase(safePointer);
    assert(pointersFound == 1);
}

/**
 * Tries to resolve unresolved references.
 *
 * Unreselved references are tried to be resolved by requesting a chunk with
 * the section's chunk() method the reference points to. Chunks mean
 * practically offset in the section and the size of the referenced data.
 * If there's an unresolved reference to an offset in a section that can
 * be chunked (section contains raw data), then we can create new elements
 * using chunk() 'on demand'.
 *
 * @exception UnresolvedReference If unresolvable references are found.
 */
void
SafePointer::resolve()
    throw (UnresolvedReference) {

    // try to resolve references in sectionOffsetMap
    for (SectionOffsetMap::iterator i = sectionOffsetMap_->begin();
         i != sectionOffsetMap_->end(); i++) {

        SectionOffsetKey key = (*i).first;

        SafePointerList* l = (*i).second;
        assert(l != NULL);
        SafePointable* object = l->reference();

        // if reference is resolved continue
        if (MapTools::containsKey(*referenceMap_, object)) {
            continue;
        }

        // there is no section with the identification code of the section
        // offset key
        if (!MapTools::containsKey(
                *sectionMap_, SectionKey(key.sectionId()))) {
            std::stringstream errorMessage;
            errorMessage << "Cannot find section with identification code "
                         << key.sectionId() << " in the section map.";

#ifndef NDEBUG
            errorMessage << " SafePointer created in : "
                         << l->front()->debugString();
#endif

            throw UnresolvedReference(
                __FILE__, __LINE__, __func__, errorMessage.str());
        }

        // try to get the pointer to the section to request chunk from
        SafePointerList* pointersToSection =
            (*sectionMap_)[SectionKey(key.sectionId())];

        assert(pointersToSection != NULL);

        Section* theSection =
            dynamic_cast<Section*>(pointersToSection->reference());

        if (theSection == NULL) {
            std::stringstream errorMessage;
            errorMessage << "Can't get section of reference pointed "
                         << "by SectionOffsetKey.";

#ifndef NDEBUG
            errorMessage << " SafePointer created in : "
                         << l->front()->debugString();
#endif

            throw UnresolvedReference(
                __FILE__, __LINE__, __func__, errorMessage.str());
        }

        SafePointable* chunk = NULL;
        try {
            chunk =
                static_cast<SafePointable*>(theSection->chunk(key.offset()));

        } catch (const NotChunkable& e) {
            // Probably there is some section that should be referred by 
            // section offsets but, while reading that section all necessary 
            // keys has not been stored.
            std::stringstream errorMessage;
            errorMessage << "Can't resolve section offset, because there"
                         << " no object stored stored for key sectionId: "
                         << key.sectionId() << " offset: " << key.offset();
#ifndef NDEBUG
            errorMessage << " SafePointer created in : "
                         << l->front()->debugString();
#endif
            throw UnresolvedReference(__FILE__,
                                      __LINE__,
                                      "SafePointer::resolve()",
                                      errorMessage.str());
        }

        assert(chunk != NULL);
        addObjectReference(key, chunk);
    }

    // check if key maps still contain unresolved references and 
    // give propriate error messages.
    std::stringstream errorMessage;
    const ReferenceKey *unresolvedKey = NULL;
    SafePointer *firstUnresolvedPointerOfList = NULL;

    if (unresolvedReferences(*sectionMap_, &unresolvedKey)) {
        const SectionKey *sectionKey =
            dynamic_cast<const SectionKey*>(unresolvedKey);

        errorMessage << "with section keys. Requested identification code: "
                     << sectionKey->sectionId() << std::endl;

        // get first of unresolved pointers in safe pointer list
        firstUnresolvedPointerOfList =
            (*sectionMap_->find(*sectionKey)).second->front();
    }

    if (unresolvedReferences(*sectionIndexMap_,  &unresolvedKey)) {
        const SectionIndexKey *indexKey =
            dynamic_cast<const SectionIndexKey*>(unresolvedKey);

        errorMessage << "Problem with section index keys. "
                     << "Requested section identification code: "
                     << static_cast<int>(indexKey->sectionId())
                     << " index: "
                     << indexKey->index() << std::endl;

        firstUnresolvedPointerOfList =
            (*sectionIndexMap_->find(*indexKey)).second->front();
    }

    if (unresolvedReferences(*sectionOffsetMap_,  &unresolvedKey)) {

        const SectionOffsetKey *sectionOffsetKey =
            dynamic_cast<const SectionOffsetKey*>(unresolvedKey);

        errorMessage << "Problem with section offset keys. "
                     << "Requested section identification code: "
                     << static_cast<int>(sectionOffsetKey->sectionId())
                     << " offset: "
                     << sectionOffsetKey->offset() << std::endl;

        firstUnresolvedPointerOfList =
            (*sectionOffsetMap_->find(*sectionOffsetKey)).second->front();
    }

    if (unresolvedReferences(*fileOffsetMap_,  &unresolvedKey)) {
        const FileOffsetKey *fileOffsetKey =
            dynamic_cast<const FileOffsetKey*>(unresolvedKey);

        errorMessage << "Problem with file offset keys. "
                     << "Requested section identification code: "
                     << fileOffsetKey->fileOffset() << std::endl;

        firstUnresolvedPointerOfList =
            (*fileOffsetMap_->find(*fileOffsetKey)).second->front();
    }

    if (errorMessage.str() != "") {

#ifndef NDEBUG
        assert(firstUnresolvedPointerOfList != NULL);
        errorMessage << " SafePointer created in : "
                     << firstUnresolvedPointerOfList->debugString();
#endif
        throw UnresolvedReference(
            __FILE__, __LINE__, __func__, errorMessage.str());
    }
}

/**
 * Deletes all entries in all key maps, doesn't clean the ReferenceMap.
 *
 * Deletes also the SafePointer lists found in the maps if they are not found
 * in the reference map. Does not delete SafePointers. This can be called
 * to free some needless resources after the object model of binary file is
 * fully constructed, and resolve() is called without an exception.
 *
 */
void
SafePointer::cleanupKeyTables() {

    set<SafePointerList*> listsToDelete;
    safelyCleanupKeyTable(*sectionIndexMap_, listsToDelete);
    safelyCleanupKeyTable(*sectionOffsetMap_, listsToDelete);
    safelyCleanupKeyTable(*fileOffsetMap_, listsToDelete);
    safelyCleanupKeyTable(*sectionMap_, listsToDelete);

    AssocTools::deleteAllItems(listsToDelete);

    // clear keyForCache...
    keyForCache_->clear();
}

/**
 * Frees all dynamically allocated memory consumed in reference managing.
 *
 * Deletes all entires in all maps, safe pointer lists in them and all
 * alive safe pointers. This method should be called in the destructor of
 * Binary or in the end of the program.
 *
 */
void
SafePointer::cleanup() {

    cleanupKeyTables();
    MapTools::deleteAllValues(*referenceMap_);
}

// Debugging methods.......

#ifndef NDEBUG
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// functions for testing
const SectionIndexMap*
SafePointer::SIMap() {
    return sectionIndexMap_;
}

const SafePointerList*
SafePointer::SIMapAt(SectionIndexKey k) {
    // make sure we don't change the map
    if (MapTools::containsKey(*sectionIndexMap_, k)) {
        return (*sectionIndexMap_)[k];
    }
    return NULL;
}

const SectionOffsetMap*
SafePointer::SOMap() {
    return sectionOffsetMap_;
}

const SafePointerList*
SafePointer::SOMapAt(SectionOffsetKey k) {
    // make sure we don't change the map
    if (MapTools::containsKey(*sectionOffsetMap_, k)) {
        return (*sectionOffsetMap_)[k];
    }
    return NULL;
}

const FileOffsetMap*
SafePointer::FOMap() {

    return fileOffsetMap_;
}

const SafePointerList*
SafePointer::FOMapAt(FileOffsetKey k) {
    // make sure we don't change the map
    if (MapTools::containsKey(*fileOffsetMap_, k)) {
        return (*fileOffsetMap_)[k];
    }
    return NULL;
}

const ReferenceMap*
SafePointer::RMap() {
    return referenceMap_;
}

const SafePointerList*
SafePointer::RMapAt(SafePointable* k) {
    // make sure we don't change the map
    if (MapTools::containsKey(*referenceMap_, k)) {
        return (*referenceMap_)[k];
    }
    return NULL;
}

const SectionMap*
SafePointer::SMap() {
    return sectionMap_;
}

const SafePointerList*
SafePointer::SMapAt(SectionKey k) {
    // make sure we don't change the map
    if (MapTools::containsKey(*sectionMap_, k)) {
        return (*sectionMap_)[k];
    }
    return NULL;
}

void
SafePointer::setDebugString(std::string aString) {
    debugString_ = aString;
}

std::string
SafePointer::debugString() const {
    return debugString_;
}

// end of test methods
#endif
#endif

} // namespace ReferenceManager

}
