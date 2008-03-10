/**
 * @file ReferenceKey.cc
 *
 * Definitions of different key types used in reference managing.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel@cs.tut.fi)
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
