/**
 * @file TPEFBaseType.hh
 *
 * Contains definitions of base types used in TPEF Binary representation.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2004  (pjaaskel@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEFBASETYPE_HH
#define TTA_TPEFBASETYPE_HH

#include "BaseType.hh"

namespace TPEF {

/// Type for storing binary file section ids.
typedef HalfWord SectionId;

/// Type for storing section indexes.
typedef Word SectionIndex;

/// Type for storing offsets relative to a given base offset value.
typedef Word SectionOffset;

/// Type for storing absolute file offsets.
typedef Word FileOffset;

}

#endif
