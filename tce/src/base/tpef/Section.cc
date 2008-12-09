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
 * @file Section.cc
 *
 * Non-inline definitions of Section class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "Section.hh"

#include <map>
#include <list>
#include <cmath>

#include "MapTools.hh"
#include "SafePointer.hh"

namespace TPEF {

using std::map;
using std::list;

using ReferenceManager::SafePointer;
using ReferenceManager::SectionOffsetKey;

/////////////////////////////////////////////////////////////////////////////
// Section
/////////////////////////////////////////////////////////////////////////////

const Byte Section::PROGRAM_SECTION_MASK = 0x80;

// initializes instance of static memeber variable
Section::SectionPrototypeMap* Section::prototypes_ = NULL;

/**
 * Constructor.
 */
Section::Section() :
    SafePointable(),
    link_(&SafePointer::null),
    aSpace_(&SafePointer::null),
    name_(&SafePointer::null),
    flags_(0),
    startingAddress_(0) {
}

/**
 * Destructor.
 */
Section::~Section() {
    while (elements_.size() != 0) {
        delete elements_[elements_.size() - 1];
        elements_.pop_back();
    }
}

/**
 * Creates instance of concrete section type.
 *
 * @param type Type of section instance that is to be created.
 * @return Pointer to newly created section.
 * @exception InstanceNotFound If there is no registered instance for type.
 */
Section*
Section::createSection(SectionType type)
    throw (InstanceNotFound) {

    if (prototypes_ == NULL ||
        !MapTools::containsKey(*prototypes_,type)) {

        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, 
            "No prototype for section type: " + 
            static_cast<int>(type));
    }

    Section* section = (*prototypes_)[type]->clone();
    return section;
}

/**
 * Registers section instance that implements some section type.
 *
 * Every registered prototype implements one of used sections. These
 * registered sections are used to clone section instances by SectionType.
 *
 * @param section Section instance that will be registered.
 */
void
Section::registerSection(const Section* section) {

    // We can't create prototypes_ map statically, because we don't know
    //if it is initialized before this method is called.
    if (prototypes_ == NULL) {
        prototypes_ = new SectionPrototypeMap();
    }

    assert(!MapTools::containsKey(*prototypes_, section->type()));

    (*prototypes_)[section->type()] = section;
}

/**
 * Adds an element to section.
 *
 * @param element Element that is added to section.
 */
void
Section::addElement(SectionElement* element) {
    assert(element != NULL);
    elements_.push_back(element);
}

/**
 * Sets replaces an element in given index with another.
 *
 * @param index Index of element that is replaced.
 * @param element Element that is set to given index.
 */
void
Section::setElement(Word index, SectionElement* element) {
    assert(element != NULL);
    assert(index < elementCount());
    elements_[index] = element;
}

/**
 * Returns true if section is chunkable.
 *
 * @return True if section is chunkable.
 */
bool
Section::isChunkable() const {
    return false;
}

/**
 * Returns a chunk that has offset to a data of section.
 *
 * @param offset Offset to a data that is wanted to be referenced.
 * @return Chunk for raw data section.
 * @exception NotChunkable If Section is not derived from RawSection.
 */
Chunk*
Section::chunk(SectionOffset /*offset*/) const
    throw (NotChunkable) {

    throw NotChunkable(__FILE__, __LINE__, "Section::chunk");
}

/////////////////////////////////////////////////////////////////////////////
// RawSection
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
RawSection::RawSection() : Section(), length_(0) {
}

/**
 * Destructor.
 */
RawSection::~RawSection() {
    MapTools::deleteAllValues(dataChunks_);
}

/**
 * Returns true if section is chunkable.
 *
 * @return True if section is chunkable.
 */
bool
RawSection::isChunkable() const {
    return true;
}

/**
 * Returns a chunk object to data in the section at a given offset.
 *
 * Checks that there is something in the section before returning chunk.
 * In the other words, Chunks can not be given from outside of
 * section's data.
 *
 * @param offset The offset of wanted data chunk.
 * @return Chunk pointing data.
 * @exception NotChunkable If called on a section that is not RawSection.
 */
Chunk*
RawSection::chunk(SectionOffset offset) const
    throw (NotChunkable) {

    Chunk* newChunk = NULL;

    try {
        assureInSection(offset);
    } catch (const OutOfRange& e) {
        throw NotChunkable(__FILE__, __LINE__, __func__, e.errorMessage()); 
    }
    if (!MapTools::containsKey(dataChunks_, offset)) {
        newChunk = new Chunk(offset);
        dataChunks_[offset]  = newChunk;
        referredChunksCache_.clear();
    } else {
        newChunk = dataChunks_[offset];
    }

    return newChunk;
}

/** 
 * Returns true if the chunk in parameter belongs to this section.
 *
 * @param chunk Chunk that is checked.
 * @return True if the chunk in parameter belongs to this section.
 */
bool 
RawSection::belongsToSection(const Chunk* chunk) const {

    if (MapTools::containsKey(dataChunks_, chunk->offset())) {
        return dataChunks_[chunk->offset()] == chunk;
    } 

    return false;
}

/**
 * Sets the length of section's data.
 *
 * @param length  bytes of data in section.
 */
void
RawSection::setDataLength(Word length) {
    // should this method be overrided to throw exception if
    // called with DataSection or StringSection classes
    length_ = length;
}

/**
 * Sets the length of section's data.
 *
 * @param length Section length in MAUs.
 */
void
RawSection::setLengthInMAUs(Word length) {
    setDataLength(MAUsToBytes(length));
}

/**
 * Returns length of data inside section.
 *
 * @return Length of data inside section.
 */
Word
RawSection::length() const {
    return length_;
}

/**
 * Returns length of data section in MAUs.
 *
 * @return Length of data inside sect.
 */
Word
RawSection::lengthInMAUs() const {
    return bytesToMAUs(length());
}

/**
 * Returns how many MAUs is amount of bytes described in parameter.
 *
 * @param bytes Number of bytes to convert.
 * @return Number of MAUs.
 */
Word
RawSection::bytesToMAUs(Word bytes) const {
    assert(aSpace() != NULL);

    if (aSpace()->MAU() == 0) {
        abortWithError("MAU of address space can't be zero.");
    }

    Word mauSize = static_cast<Word>(
        ceil(static_cast<double>(aSpace()->MAU()) /
             static_cast<double>(BYTE_BITWIDTH)));

    // must be even
    assert ((bytes % mauSize) == 0);

    return bytes / mauSize;
}

/**
 * Returns how many bytes some amount of MAUs are.
 *
 * @param maus Number of MAUs to convert.
 * @return Number of bytes that are needed for storing some amount of MAUs.
 */
Word
RawSection::MAUsToBytes(Word maus) const {
    assert(aSpace() != NULL);

    if (aSpace()->MAU() == 0) {
        abortWithError("MAU of address space can't be zero for this method.");
    }

    Word mauSize = static_cast<Word>(
        ceil(static_cast<double>(aSpace()->MAU()) /
             static_cast<double>(BYTE_BITWIDTH)));

    return maus * mauSize;
}

/**
 * Returns MAU index to given chunk.
 *
 * @param chunk Chunk whose MAU index should be found.
 * @return MAU index to data referred by the chunk.
 */
Word
RawSection::chunkToMAUIndex(const Chunk* chunk) const {
    return bytesToMAUs(chunk->offset());
}

/**
 * Checks that section is not indexed out of bounds.
 *
 * If section is indexed out of bounds, exception is thrown.
 *
 * @param offset The offset being checked that it is in the area of section.
 * @exception OutOfRange if not in area of section.
 */
void
RawSection::assureInSection(
    SectionOffset offset) const {
    if (offset >= length())
        throw OutOfRange(__FILE__, __LINE__, __func__);
}

/**
 * Returns number of chunks that are requested from section.
 *
 * @return Number of requested chunks.
 */
Word
RawSection::referredChunkCount() const {
    return dataChunks_.size();
}


/**
 * Returns chunks that is requested from section before.
 *
 * @param Index of chunk to return.
 * @return Chunk to return.
 */
Chunk*
RawSection::referredChunk(Word index) const {
    if (referredChunksCache_.empty()) {
	ChunkMap::iterator iter = dataChunks_.begin();

	while (iter != dataChunks_.end()) {
	    referredChunksCache_.push_back((*iter).second);
	    iter++;
	}
    }

    return referredChunksCache_[index];
}

}
