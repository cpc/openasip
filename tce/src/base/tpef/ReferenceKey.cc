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
 * @file ReferenceKey.cc
 *
 * Definitions of different key types used in reference managing.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 17 Oct 2003 by am, rm, kl
 *
 * @note rating: yellow
 */

#include "ReferenceKey.hh"

namespace TPEF {

namespace ReferenceManager {

//////////////////////////////////////////////////////////////////////////////
// ReferenceKey
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 */
ReferenceKey::ReferenceKey() {
}

/**
 * The destructor.
 */
ReferenceKey::~ReferenceKey() {
}

//////////////////////////////////////////////////////////////////////////////
// SectionIndexKey
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param sectionId Identification code of the section.
 * @param index Index of the element in section.
 */
SectionIndexKey::SectionIndexKey(SectionId sectionId, SectionIndex index) :
    ReferenceKey(), sectionId_(sectionId), index_(index) {
}

/**
 * The destructor.
 */
SectionIndexKey::~SectionIndexKey() {
}

//////////////////////////////////////////////////////////////////////////////
// SectionOffsetKey
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param sectionId Identification code of the section.
 * @param offset Offset of the element in section.
 */
SectionOffsetKey::SectionOffsetKey(SectionId sectionId, SectionOffset offset) :
    ReferenceKey(), sectionId_(sectionId), offset_(offset) {
}

/**
 * The destructor.
 */
SectionOffsetKey::~SectionOffsetKey() {
}

//////////////////////////////////////////////////////////////////////////////
// FileOffsetKey
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param fileOffset File offset.
 */
FileOffsetKey::FileOffsetKey(FileOffset fileOffset) :
    ReferenceKey(), fileOffset_(fileOffset) {
}

/**
 * The destructor.
 */
FileOffsetKey::~FileOffsetKey() {
}

//////////////////////////////////////////////////////////////////////////////
// SectionKey
//////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * @param sectionId Identification code of the section.
 */
SectionKey::SectionKey(SectionId sectionId) :
    sectionId_(sectionId) {
}

/**
 * The destructor.
 */
SectionKey::~SectionKey() {
}

}

}
