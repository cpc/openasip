/**
 * @file FileOffsetReplacer.cc
 *
 * Non-inline implementations of FileOffsetReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "FileOffsetReplacer.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;

/**
 * Constructor.
 */
FileOffsetReplacer::FileOffsetReplacer(const SafePointable* obj) :
    ValueReplacer(obj) {
}

/**
 * Copy constructor.
 */
FileOffsetReplacer::FileOffsetReplacer(
    const FileOffsetReplacer& replacer) : ValueReplacer(replacer) {
}

/**
 * Destructor.
 */
FileOffsetReplacer::~FileOffsetReplacer() {
}

/**
 * Tries to do replacement in to the stream.
 *
 * @return True if replacement were done.
 */
bool
FileOffsetReplacer::tryToReplace()
    throw (UnreachableStream, WritePastEOF) {

    try {
        FileOffsetKey key =
            SafePointer::fileOffsetKeyFor(reference());

        stream().setWritePosition(streamPosition());
        stream().writeWord(key.fileOffset());
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
FileOffsetReplacer::clone() {
    return new FileOffsetReplacer(*this);
}

}
