/**
 * @file TPEFStringSectionWriter.cc
 *
 * Definition of TPEFStringSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFStringSectionWriter.hh"
#include "StringSection.hh"
#include "SafePointer.hh"
#include "SectionSizeReplacer.hh"
#include "ReferenceKey.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::FileOffsetKey;


const TPEFStringSectionWriter TPEFStringSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFStringSectionWriter::TPEFStringSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFStringSectionWriter::~TPEFStringSectionWriter() {
}

/**
 * Returns the type of the section this writer can write.
 *
 * @return The type of the section writer can write.
 */
Section::SectionType
TPEFStringSectionWriter::type() const {
    return Section::ST_STRTAB;
}

/**
 * Writes the data of the section to the stream.
 *
 * @param stream The stream to be written.
 * @param sect The section which data is written to stream.
 */
void
TPEFStringSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    const StringSection* stringSection =
        dynamic_cast<const StringSection*>(sect);

    assert(stringSection != NULL);

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(FileOffsetKey(startOffset),
                                    sect->chunk(0));

    SectionOffset sectOffset = 0;
    SectionKey sKey = SafePointer::sectionKeyFor(stringSection);
    SectionId id = sKey.sectionId();

    // first byte
    assert(stringSection->byte(stringSection->chunk(0)) == 0);

    // last byte must be also zero
    assert(stringSection->byte(
               stringSection->chunk(
                   stringSection->length() - 1)) == 0);

    for (unsigned int i = 0; i < stringSection->length(); i++) {
        sectOffset = stream.writePosition() - startOffset;

        Chunk* chunk = NULL;

        try {
            chunk = stringSection->chunk(sectOffset);
        } catch (const NotChunkable& n) {
            bool sectionToBeWrittenIsNotChunkable = false;
            assert(sectionToBeWrittenIsNotChunkable);
        }

        if (SafePointer::isReferenced(chunk)) {

            SafePointer::addObjectReference(
                SectionOffsetKey(id, sectOffset), chunk);
        }

        stream.writeByte(stringSection->byte(chunk));
    }

    SectionSizeReplacer::setSize(
        stringSection, stream.writePosition() - startOffset);
}

/**
 * Returns size of char.
 *
 * @return Size of char.
 */
Word
TPEFStringSectionWriter::elementSize(const Section* /*section*/) const {
    return sizeof(char);
}

}
