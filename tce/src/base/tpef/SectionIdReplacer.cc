/**
 * @file SectionIdReplacer.cc
 *
 * Definition of SectionIdReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionIdReplacer.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;

/**
 * Constructor.
 */
SectionIdReplacer::SectionIdReplacer(const SafePointable* obj) :
    ValueReplacer(obj) {
}

/**
 * Copy constructor.
 */
SectionIdReplacer::SectionIdReplacer(const SectionIdReplacer& replacer) :
    ValueReplacer(replacer) {
}

/**
 * Destructor.
 */
SectionIdReplacer::~SectionIdReplacer() {
}

/**
 * Tries to do replacement in to the stream.
 *
 * @return True if replacement were done.
 */
bool
SectionIdReplacer::tryToReplace()
    throw (UnreachableStream, WritePastEOF) {

    try {
        SectionKey key =
            SafePointer::sectionKeyFor(reference());

        stream().setWritePosition(streamPosition());
        stream().writeHalfWord(key.sectionId());
        return true;

    } catch (KeyNotFound& e) {
        stream().writeHalfWord(0);
        return false;
    }
}

/**
 * Creates dynamically allocated copy of replacer.
 *
 * @return Dynamically allocated copy of replacer.
 */
ValueReplacer*
SectionIdReplacer::clone() {
    return new SectionIdReplacer(*this);
}

}
