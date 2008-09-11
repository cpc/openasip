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
 * @file SectionIndexReplacer.cc
 *
 * Definition of SectionIndexReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
