/**
 * @file TPEFDataSectionWriter.cc
 *
 * Definition of TPEFDataSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath>

#include "TPEFDataSectionWriter.hh"
#include "DataSection.hh"
#include "Chunk.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "ASpaceElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;

const TPEFDataSectionWriter TPEFDataSectionWriter::instance_;

/**
 * Constructor.
 */
TPEFDataSectionWriter::TPEFDataSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFDataSectionWriter::~TPEFDataSectionWriter() {
}

/**
 * Returns section type which for writer is implemented.
 *
 * @return Section type which of section that class can write.
 */
Section::SectionType
TPEFDataSectionWriter::type() const {
    return Section::ST_DATA;
}

/**
 * Writes section's data into stream.
 *
 * @param stream Stream where to write.
 * @param sect Section to be written.
 */
void
TPEFDataSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    const DataSection* dSection = dynamic_cast<const DataSection*>(sect);

    assert(dSection != NULL);

    FileOffset startOffset = stream.writePosition();

    SectionOffset sectOffset = 0;
    SectionKey sKey = SafePointer::sectionKeyFor(dSection);
    SectionId id = sKey.sectionId();

    for (; sectOffset < dSection->length(); sectOffset++) {

        Chunk* chunk = NULL;

        try {
            chunk = dSection->chunk(sectOffset);
        } catch (const NotChunkable& n) {
            bool sectionToBeWrittenIsNotChunkable = false;
            assert(sectionToBeWrittenIsNotChunkable);
        }

        // if referenced or first element of section
        if (SafePointer::isReferenced(chunk)) {
            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), chunk);
        }

        stream.writeByte(dSection->byte(chunk));
    }

    // needed for writing section header
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), dSection->chunk(0));

    SectionSizeReplacer::setSize(
        dSection, stream.writePosition() - startOffset);
}

/**
 * Returns the fixed size of section elements, in this case smallest
 * amount of bytes that is needed to store one MAU of address space.
 *
 * @param section Section which is written.
 * @return Size of uninitialized data element in bytes.
 */
Word
TPEFDataSectionWriter::elementSize(const Section* section) const {
    return static_cast<Word>(
        ceil(static_cast<double>(section->aSpace()->MAU()) /
             static_cast<double>(BYTE_BITWIDTH)));
}

}
