/**
 * @file Chunk.cc
 *
 * Non-inline definitions of Chunk class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "Chunk.hh"

namespace TPEF {

/**
 * Constructor.
 *
 * @param offset Offset to data where chunk is pointing to.
 */
Chunk::Chunk(SectionOffset offset) :
    SectionElement(), offset_(offset) {
}

/**
 * Destructor.
 */
Chunk::~Chunk() {
}

}
