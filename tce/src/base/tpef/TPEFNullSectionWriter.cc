/**
 * @file TPEFNullSectionWriter.cc
 *
 * Definitions of TPEFNullSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFNullSectionWriter.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "TPEFBaseType.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;

const TPEFNullSectionWriter TPEFNullSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFNullSectionWriter::TPEFNullSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFNullSectionWriter::~TPEFNullSectionWriter() {
}

/**
 * Returns which type of sections can be written out by this writer.
 *
 * @return The type of section that writer can write out.
 */
Section::SectionType
TPEFNullSectionWriter::type() const {
    return Section::ST_NULL;
}

/**
 * Returns the size of section elements. The null section has no elements,
 * so the size of elements is undefined. Returns always zero.
 *
 * @param section Section whose element size is to be written out (not used).
 * @return Zero.
 */
Word
TPEFNullSectionWriter::elementSize(const Section*) const {
    return 0;
}

/**
 * Returns the identification code of the section.
 *
 * Zero is the numeric code reserved for identifying null sections.
 *
 * @return Zero.
 */
SectionId
TPEFNullSectionWriter::getSectionId() const {
    return 0;
}

/**
 * Writes the size of a null section into an output stream.
 *
 * The size of a null section is always zero.
 *
 * @param stream Output stream.
 * @param sect Section whose size has to be written out (not used).
 */
void
TPEFNullSectionWriter::writeSize(BinaryStream& stream,
    const Section*) const {

    stream.writeWord(0);
}

}
