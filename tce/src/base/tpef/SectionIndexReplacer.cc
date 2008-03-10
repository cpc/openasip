/**
 * @file SectionIndexReplacer.cc
 *
 * Definition of SectionIndexReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionIndexReplacer.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;

/**
 * Constructor.
 */
SectionIndexReplacer::SectionIndexReplacer(
    const SafePointable* obj, Byte size) :
    ValueReplacer(obj), fieldSize_(size) {
}

/**
 * Copy constructor.
 */
SectionIndexReplacer::SectionIndexReplacer(
    const SectionIndexReplacer& replacer) :
    ValueReplacer(replacer), fieldSize_(replacer.fieldSize_) {
}

/**
 * Destructor.
 */
SectionIndexReplacer::~SectionIndexReplacer() {
}

/**
 * Tries to do replacement in to the stream.
 *
 * @return True if replacement were done.
 */
bool
SectionIndexReplacer::tryToReplace()
    throw (UnreachableStream, WritePastEOF) {
    try {
        SectionIndexKey key =
            SafePointer::sectionIndexKeyFor(reference());

        stream().setWritePosition(streamPosition());
        writeReplacement(key.index());
        return true;

    } catch (KeyNotFound& e) {
        writeReplacement(0);
        return false;
    }
}

/**
 * Creates dynamically allocated copy of replacer.
 *
 * @return Dynamically allocated copy of replacer.
 */
ValueReplacer*
SectionIndexReplacer::clone() {
    return new SectionIndexReplacer(*this);
}

/**
 * Writes value with one, two or four bytes wide.
 *
 * @param value Value to be written.
 */
void
SectionIndexReplacer::writeReplacement(Word value) {
    switch (fieldSize_) {
    case 1: {
        stream().writeByte(value);
        break;
	}
    case 2: {
        stream().writeHalfWord(value);
        break;
	}
    case 3: {
	    Byte firstByte = (value >> HALFWORD_BITWIDTH);
	    HalfWord lastBytes = (value << HALFWORD_BITWIDTH)
                                    >> HALFWORD_BITWIDTH;

	    // writing three bytes in big endian format
        stream().writeHalfWord(lastBytes);
	    stream().writeByte(firstByte);
        break;
	}
    case 4: {
        stream().writeWord(value);
        break;
	}
    default:
	    bool unsupportedFieldSizeOfReplacement = false;
        assert(unsupportedFieldSizeOfReplacement);
        }
}

}
