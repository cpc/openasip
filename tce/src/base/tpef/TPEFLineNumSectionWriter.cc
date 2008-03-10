/**
 * @file TPEFLineNumSectionWriter.cc
 *
 * Definition of TPEFLineNumSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFLineNumSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "LineNumSection.hh"
#include "LineNumProcedure.hh"
#include "LineNumElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIndexReplacer.hh"
#include "SectionIdReplacer.hh"

namespace TPEF {

using std::list;

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFLineNumSectionWriter TPEFLineNumSectionWriter::instance_;
const Word TPEFLineNumSectionWriter::elementSize_ = 6;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFLineNumSectionWriter::TPEFLineNumSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFLineNumSectionWriter::~TPEFLineNumSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFLineNumSectionWriter::type() const {
    return Section::ST_LINENO;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFLineNumSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();
    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    for (Word i = 0; i < sect->elementCount(); i++) {
        LineNumProcedure* elem =
            dynamic_cast<LineNumProcedure*>(sect->element(i));
        assert(elem != NULL);

        if (elem->procedureSymbol() != NULL) {
            SectionIndexReplacer
                replacer(elem->procedureSymbol(), sizeof(Word));
            replacer.resolve();
        } else {
            bool procedureSymbolMustBeSet = false;
            assert(procedureSymbolMustBeSet);
        }

        // line number zero represents procedure
        stream.writeHalfWord(0);

        // write line nums...
        for(Word lineIndex = 0; lineIndex < elem->lineCount(); lineIndex++) {

            const LineNumElement* lineElem = elem->line(lineIndex);

            // write each line
            SectionOffsetReplacer replacer(lineElem->instruction());
            replacer.resolve();
            stream.writeHalfWord(lineElem->lineNumber());
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
TPEFLineNumSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

/**
 * Writes Info field of section header.
 *
 * @param stream The stream where to write.
 * @param sect Section to write.
 */
void
TPEFLineNumSectionWriter::writeInfo(
    BinaryStream& stream, const Section* sect) const {

    const Section* codeSection =
        dynamic_cast<const LineNumSection*>(sect)->codeSection();

    SectionIdReplacer idReplacer(codeSection);
    idReplacer.resolve();

    stream.writeHalfWord(0);
}

}
