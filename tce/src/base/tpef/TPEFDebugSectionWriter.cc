/**
 * @file TPEFDebugSectionWriter.cc
 *
 * Definition of TPEFDebugSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFDebugSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIdReplacer.hh"
#include "DebugElement.hh"
#include "SectionSymElement.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFDebugSectionWriter TPEFDebugSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFDebugSectionWriter::TPEFDebugSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFDebugSectionWriter::~TPEFDebugSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFDebugSectionWriter::type() const {
    return Section::ST_DEBUG;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFDebugSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    for (Word i = 0; i < sect->elementCount(); i++) {

        DebugElement* elem = dynamic_cast<DebugElement*>(sect->element(i));
        assert(elem != NULL);

        // write type
        stream.writeHalfWord(static_cast<HalfWord>(elem->type()));

        // debug string
        assert(elem->debugString() != NULL);
        SectionOffsetReplacer sOffReplacer(elem->debugString());
        sOffReplacer.resolve();

        // write variable data
        stream.writeWord(elem->length());

        for (unsigned int j = 0; j < elem->length(); j++) {
            stream.writeByte(elem->byte(j));
        }
    }
    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Returns the fixed size length of section elements.
 *
 * @return The fixed size length of section elements.
 */
Word
TPEFDebugSectionWriter::elementSize(const Section* /*section*/) const {
    return 0;
}

/**
 * Creates needed keys for section if noBits flag is set.
 *
 * @param sect Section which for keys will be created.
 */
void
TPEFDebugSectionWriter::createKeys(const Section* sect) const {
    SectionSizeReplacer::setSize(sect, 0x0);
}

}
