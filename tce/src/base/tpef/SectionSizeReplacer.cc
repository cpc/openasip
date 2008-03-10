/**
 * @file SectionSizeReplacer.cc
 *
 * Definition of SectionSizeReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionSizeReplacer.hh"
#include "SafePointer.hh"
#include "MapTools.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

std::map<const SafePointable*, Word> SectionSizeReplacer::sizeMap;

/**
 * Constructor.
 */
SectionSizeReplacer::SectionSizeReplacer(const SafePointable* obj) :
    ValueReplacer(obj) {
}

/**
 * Copy constructor.
 */
SectionSizeReplacer::SectionSizeReplacer(
    const SectionSizeReplacer& replacer) :
    ValueReplacer(replacer) {
}

/**
 * Destructor.
 */
SectionSizeReplacer::~SectionSizeReplacer() {
}

/**
 * Tries to do replacement in to the stream.
 *
 * @return True if replacement were done.
 */
bool
SectionSizeReplacer::tryToReplace()
    throw (UnreachableStream, WritePastEOF) {

    if(MapTools::containsKey(sizeMap, reference())) {
        stream().setWritePosition(streamPosition());
        stream().writeWord(sizeMap[reference()]);
        return true;
    }

    stream().writeWord(0);
    return false;
}

/**
 * Creates dynamically allocated copy of replacer.
 *
 * @return Dynamically allocated copy of replacer.
 */
ValueReplacer*
SectionSizeReplacer::clone() {
    return new SectionSizeReplacer(*this);
}

/**
 * Set's size for referred object.
 *
 * @param obj Objects which for size is stored.
 * @param size Size to store.
 */
void
SectionSizeReplacer::setSize(const SafePointable* obj, Word size) {
    assert(obj != NULL);
    sizeMap[obj] = size;
}

/**
 * Clears all elements from size map.
 */
void
SectionSizeReplacer::clear() {
    sizeMap.clear();
}

}
