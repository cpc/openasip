/**
 * @file Chunk.hh
 *
 * Declaration of Chunk class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_CHUNK_HH
#define TTA_CHUNK_HH

#include "TPEFBaseType.hh"
#include "SafePointer.hh"
#include "SectionElement.hh"

namespace TPEF {

/**
 * Chunk class for representing raw data offset in section.
 */
class Chunk : public SectionElement {
public:
    Chunk(SectionOffset offset);
    virtual ~Chunk();

    SectionOffset offset() const;

private:
    /// Offset to data where chunk is pointing.
    SectionOffset offset_;
};
}

#include "Chunk.icc"

#endif
