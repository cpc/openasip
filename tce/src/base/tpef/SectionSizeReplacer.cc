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
 * @file SectionSizeReplacer.cc
 *
 * Definition of SectionSizeReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionSizeReplacer.hh"
#include "SafePointer.hh"
#include "MapTools.hh"
#include "BinaryStream.hh"

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
SectionSizeReplacer::tryToReplace() {
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
