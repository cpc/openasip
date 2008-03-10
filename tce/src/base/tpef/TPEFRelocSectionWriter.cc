/**
 * @file TPEFRelocSectionWriter.cc
 *
 * Definitions of TPEFRelocSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFRelocSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionSizeReplacer.hh"
#include "SectionElement.hh"
#include "SectionOffsetReplacer.hh"
#include "RelocElement.hh"
#include "SectionIdReplacer.hh"
#include "SectionIndexReplacer.hh"
#include "FileOffsetReplacer.hh"
#include "RelocSection.hh"
#include "TPEFCodeSectionWriter.hh"
#include "ImmediateElement.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::FileOffsetKey;

const TPEFRelocSectionWriter TPEFRelocSectionWriter::instance_;
const Word TPEFRelocSectionWriter::elementSize_ = 12;

/**
 * Construction.
 *
 * Registers itself to SectionWriter.
 */
TPEFRelocSectionWriter::TPEFRelocSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFRelocSectionWriter::~TPEFRelocSectionWriter() {
}

/**
 * Returns the type of the section this writer can write.
 *
 * @return The type of section this writer can write.
 */
Section::SectionType
TPEFRelocSectionWriter::type() const {
    return Section::ST_RELOC;
}

/**
 * Writes relocation section in to stream.
 *
 * @param stream The stream where to write.
 * @param section The section to be written.
 */
void
TPEFRelocSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* section) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), section->element(0));

    const RelocSection *rSection =
        dynamic_cast<const RelocSection*>(section);
    assert(rSection != NULL);


    for (Word i = 0; i < section->elementCount(); i++) {
        RelocElement* rElem = 
            dynamic_cast<RelocElement*>(section->element(i));
        assert(rElem != NULL);

        // r_offset
        if (rElem->location() != NULL) {
            SectionOffsetReplacer sReplacer(rElem->location());
            sReplacer.resolve();
        } else {
            bool locationOfRelocationMustBeSet = false;
            assert(locationOfRelocationMustBeSet);
        }
        
        // r_symbol
        if (rElem->symbol() != NULL) {
            SectionIndexReplacer indexReplacer(rElem->symbol(), sizeof(Word));
            indexReplacer.resolve();
        } else {
            bool symbolFieldOfRelocationMustBeSet = false;
            assert(symbolFieldOfRelocationMustBeSet);
        }

        // r_type
        Byte rType = 0;

        // chunked bit on if necessary.
        if (rElem->chunked()) {
            rType = rType | TPEFHeaders::STF_CHUNK;
        }

        // and relocation type
        rType = rType | static_cast<Byte>(rElem->type());

        stream.writeByte(rType);

        // r_asp
        SectionIndexReplacer indexReplacer(rElem->aSpace(), sizeof(Byte));
        indexReplacer.resolve();

        // r_size bit width of the value of element referred by location
        stream.writeByte(rElem->size());

        // r_bitpos for chunked relocations
        stream.writeByte(rElem->bitOffset());
    }

    SectionSizeReplacer::setSize(section,
                                 stream.writePosition() - startOffset);
}

/**
 * Returns size of element.
 *
 * @param section Section that is written.
 * @return Size of element of this section.
 */
Word
TPEFRelocSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

/**
 * Writes Info field of section header.
 *
 * @param stream The stream where to write.
 * @param sect Section to write.
 */
void
TPEFRelocSectionWriter::writeInfo(BinaryStream& stream,
				  const Section* sect) const {

    const Section* refSection =
        dynamic_cast<const RelocSection*>(sect)->referencedSection();
    
    SectionIdReplacer idReplacer(refSection);
    idReplacer.resolve();

    stream.writeHalfWord(0);
}

}
