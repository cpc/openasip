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
 * @file SafePointer.hh
 *
 * Declaration of SafePointer class and utility classes and types related
 * to ReferenceManager.
 *
 * @author Pekka J��skel�inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 29 Aug 2003 by rm, ml, ac, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_SAFEPOINTER_HH
#define TTA_SAFEPOINTER_HH

#include <cstddef> // NULL
#include <set>
#include <map>
#include <list>
#include <iterator>
#include <sstream>

//#include "hash_map.hh"
#include "Application.hh"
#include "ReferenceKey.hh"
#include "Exception.hh" // IllegalParameters, UnresolvedReference

namespace TPEF {

class SafePointable;

/**
 *
 * Contains classes, utility classes and types related to reference managing.
 *
 */
namespace ReferenceManager {

class SafePointer;

///////////////////////////////////////////////////////////////////////////////
// SafePointerList
///////////////////////////////////////////////////////////////////////////////
/**
 * List of SafePointers that are all pointing to the same object.
 *
 */
class SafePointerList {
public:
    /// Type of the container to hold the SafePointers in.
    typedef std::list<SafePointer*> SafePointerListType;

    /// Type of the list length value.
    typedef std::string::size_type LengthType;

    SafePointerList();
    SafePointerList(SafePointerList &aList);

    virtual ~SafePointerList();

    void setReference(const SafePointable* obj);
    SafePointable* reference() const;

    void append(SafePointer* newSafePointer);
    void append(SafePointerList* anotherSafePointerList);

    SafePointer* front() const;

    void cleanup();
    void cleanupDead();

    LengthType length() const;

private:
    /// Object that SafePointers in this list are pointing to.
    SafePointable* reference_;

    /// Container for SafePointers.
    SafePointerListType list_;
};

/**
 * Class containing all the hash functions if hash_map is
 * used. However the use of hash_map will make program very slow
 * unless MapTools::containsValue() is not optimised for 
 * hash_tables or that function should be circumvented.
 */
class HashFunctions {
public:
    size_t operator()(const SectionIndexKey& key) const {
        return key.index()^
            (static_cast<size_t>(key.sectionId()) <<
             ((sizeof(size_t) - sizeof(SectionId)) *
              BYTE_BITWIDTH));
    }

    size_t operator()(const SectionOffsetKey& key) const {
        return key.offset()^
            (static_cast<size_t>(key.sectionId()) <<
             ((sizeof(size_t) - sizeof(SectionId)) *
              BYTE_BITWIDTH));
    }

    size_t operator()(const FileOffsetKey& key) const {
        return key.fileOffset();
    }

    size_t operator()(const SectionKey& key) const {
        return key.sectionId();
    }

    size_t operator()(const SafePointable* const & key) const {
        return reinterpret_cast<size_t>(key);
    }
};

// TODO: for hash map implementation check SafePointer.cc line 424 TODO
//       MapTools::containsValue call takes very long time for hash maps..

/// Unordered set of SafePointers.
typedef std::set<SafePointer*> SafePointerSet;

/// Map for SafePointers that are requested using SectionIndexKeys.
typedef std::map<SectionIndexKey, SafePointerList*> SectionIndexMap;
//typedef hash_map<SectionIndexKey, SafePointerList*,
//                 HashFunctions> SectionIndexMap;

/// Map for SafePointers that are requested using SectionOffsetKeys.
typedef std::map<SectionOffsetKey, SafePointerList*> SectionOffsetMap;
//typedef hash_map<SectionOffsetKey, SafePointerList*,
//                 HashFunctions> SectionOffsetMap;

/// Map for SafePointers that are requested using FileOffsetKeys.
typedef std::map<FileOffsetKey, SafePointerList*> FileOffsetMap;
//typedef hash_map<FileOffsetKey, SafePointerList*,
//                 HashFunctions> FileOffsetMap;

/// Map for SafePointers that are requested using SectionKeys.
typedef std::map<SectionKey, SafePointerList*> SectionMap;
//typedef hash_map<SectionKey, SafePointerList*,
//                 HashFunctions> SectionMap;


/// Map for resolved references, that is SafePointers that are pointing to
/// the created object.
typedef std::map<const SafePointable*, SafePointerList*> ReferenceMap;
//typedef hash_map<const SafePointable*, SafePointerList*,
//                 HashFunctions> ReferenceMap;


///////////////////////////////////////////////////////////////////////////////
// SafePointer
///////////////////////////////////////////////////////////////////////////////
/**
 * Indirection for object references.
 *
 * Allows updating references later on, thus allows referencing objects that
 * are created later. Helps with the dangling pointers problem, too.
 *
 * References to objects that are not created yet are made with different kind
 * of keys. These keys refer for example to sections and offsets in source
 * binary file while reading the binary. After the binary is read and the
 * object model of binary is constructed, these keys and keytables have no use.
 */
class SafePointer {
public:
    virtual ~SafePointer();

    SafePointable* pointer() const;
    void setPointer(SafePointable* object);

    static bool isAlive(SafePointer* pointerToCheck);
    static bool isReferenced(const SafePointable* object);

    static void addObjectReference(
        SectionIndexKey key, const SafePointable* obj);
    static void addObjectReference(
        SectionOffsetKey key, const SafePointable* obj);
    static void addObjectReference(FileOffsetKey key, const SafePointable* obj);
    static void addObjectReference(SectionKey key, const SafePointable* obj);

    static SectionIndexKey sectionIndexKeyFor(const SafePointable* obj);
    static SectionOffsetKey sectionOffsetKeyFor(const SafePointable* obj);
    static FileOffsetKey fileOffsetKeyFor(const SafePointable* obj);
    static SectionKey sectionKeyFor(const SafePointable* obj);

    static void notifyDeleted(const SafePointable* obj);
    static void notifyDeleted(SafePointer* safePointer);

    static void resolve();

    static void cleanupKeyTables();
    static void cleanup();

    template <typename MapType>
    static bool unresolvedReferences(const MapType& mapToCheck,
                                     const ReferenceKey **unresolvedKey);

    static const SafePointer* replaceReference(
        const SafePointer* old, SafePointable* obj);

    static void replaceAllReferences(
        SafePointable* newObj, SafePointable* oldObj);

    /// The default SafePointer that is used in null references.
    static const SafePointer null;

#ifndef NDEBUG
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * Creates SafePointer with debug data of creation place of pointer.
 *
 * Just calls debugCreate method, with full parameters.
 *
 * Creation place of SafePointer is very important to know
 * because it's very hard to find where SafePointer that points
 * to wrong place was created.
 */
#define CREATE_SAFEPOINTER(x) \
    ReferenceManager::SafePointer::debugCreate( x , __FILE__, __LINE__)

    // Debug methods... don't delete.

    // methods for testing
    static const SectionIndexMap* SIMap();
    static const SafePointerList* SIMapAt(SectionIndexKey k);
    static const SectionOffsetMap* SOMap();
    static const SafePointerList* SOMapAt(SectionOffsetKey k);
    static const FileOffsetMap* FOMap();
    static const SafePointerList* FOMapAt(FileOffsetKey k);
    static const ReferenceMap* RMap();
    static const SafePointerList* RMapAt(SafePointable* k);
    static const SectionMap* SMap();
    static const SafePointerList* SMapAt(SectionKey k);

    // methods to set and get debug string
    void setDebugString(std::string aStr);
    std::string debugString() const;

    template <typename ObjType>
    static SafePointer* debugCreate(ObjType obj, const char *file, int line);

#endif // DOXYGEN_SHOULD_SKIP_THIS

#else

/**
 * Creates SafePointer without debug data.
 *
 * Just calls genericCreate method.
 */
#define CREATE_SAFEPOINTER(x) \
    ReferenceManager::SafePointer::genericCreate(x)
#endif

    template <typename ObjType>
    static SafePointer* genericCreate(ObjType obj);


protected:
    SafePointer(SectionIndexKey key);
    SafePointer(SectionOffsetKey key);
    SafePointer(FileOffsetKey key);
    SafePointer(SectionKey key);
    SafePointer(SafePointable* object);

    template <typename KeyType, typename MapType>
    void genericRegisterPointer(
        const KeyType& key,
        MapType& destinationMap,
        SafePointer* newSafePointer);

    template <typename KeyType, typename MapType>
    static void genericAddObjectReference(
        const KeyType& key, MapType& destinationMap, const SafePointable* obj);

    template <typename KeyType, typename MapType>
    static KeyType genericKeyFor(const SafePointable* obj, MapType& sourceMap);

    template <typename MapType>
    static void safelyCleanupKeyTable(
        MapType& sourceMap,
        std::set<SafePointerList*>&
        listsToDelete);

private:

    /// The reference to the real object.
    SafePointable* object_;

    /// Map of SafePointers that are requested using SectionIndexKeys.
    static SectionIndexMap* sectionIndexMap_;

    /// Map of SafePointers that are requested using SectionOffsetKeys.
    static SectionOffsetMap* sectionOffsetMap_;

    /// Map of SafePointers that are requested using FileOffsetKeys.
    static FileOffsetMap* fileOffsetMap_;

    /// Map of SafePointers that are requested using SectionKeys.
    static SectionMap* sectionMap_;

    /// Map of SafePointers that have resolved references.
    static ReferenceMap* referenceMap_;

    /// Set that cointains all alive (not deleted) SafePointers for extra
    /// safety.
    static SafePointerSet* aliveSafePointers_;

    // KeyFor cache stuff to make genericKeyFor function to work O(1) speed
    // after first call

    /// Key type for cache, void* is pointer to key map
    /// (sectioMap_, sectionOffsetMap, ...)
    typedef std::pair<const SafePointable*, void*> KeyForCacheKey;
    typedef std::map<KeyForCacheKey,const ReferenceKey*> KeyForCacheMap;

    /// Map for cache.
    static KeyForCacheMap* keyForCache_;

    /// Assignment not allowed.
    SafePointer& operator=(SafePointer&);

    /// Copying not allowed.
    SafePointer(SafePointer&);

#ifndef NDEBUG
    std::string debugString_;
#endif

};

} // namespace ReferenceManager
}

#include "SafePointer.icc"

#endif
