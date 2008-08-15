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
