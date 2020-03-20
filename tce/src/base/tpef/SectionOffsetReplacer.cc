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
 * @file SectionOffsetReplacer.cc
 *
 * Definition of SectionOffsetReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionOffsetReplacer.hh"
#include "SafePointer.hh"
#include "BinaryStream.hh"

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
SectionOffsetReplacer::tryToReplace() {
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
