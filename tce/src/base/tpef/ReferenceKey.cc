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
