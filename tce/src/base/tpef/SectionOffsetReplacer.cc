/**
 * @file SectionOffsetReplacer.cc
 *
 * Definition of SectionOffsetReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionOffsetReplacer.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionOffsetKey;

/**
 * Constructor.
 */
SectionOffsetReplacer::SectionOffsetReplacer(const SafePointable* obj) :
    ValueReplacer(obj) {
}

/**
 * Copy constructor.
 */
SectionOffsetReplacer::SectionOffsetReplacer(
    const SectionOffsetReplacer& replacer) : ValueReplacer(replacer) {
}

/**
 * Destructor.
 */
SectionOffsetReplacer::~SectionOffsetReplacer() {
}

/**
 * Tries to do replacement in to the stream.
 *
 * @return True if replacement were done.
 */
bool
SectionOffsetReplacer::tryToReplace()
    throw (UnreachableStream, WritePastEOF) {
    try {
        SectionOffsetKey key =
            SafePointer::sectionOffsetKeyFor(reference());

        stream().setWritePosition(streamPosition());
        stream().writeWord(key.offset());
        return true;

    } catch (KeyNotFound& e) {
        stream().writeWord(0);
        return false;
    }
}

/**
 * Creates dynamically allocated copy of replacer.
 *
 * @return Dynamically allocated copy of replacer.
 */
ValueReplacer*
SectionOffsetReplacer::clone() {
    return new SectionOffsetReplacer(*this);
}

}
