/**
 * @file TPEFResourceSectionWriter.cc
 *
 * Definition of TPEFResourceSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>
#include <set>

#include "TPEFResourceSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "ResourceElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFResourceSectionWriter TPEFResourceSectionWriter::instance_;
const Word TPEFResourceSectionWriter::elementSize_ = 11;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFResourceSectionWriter::TPEFResourceSectionWriter() :
    TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFResourceSectionWriter::~TPEFResourceSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFResourceSectionWriter::type() const {
    return Section::ST_MR;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFResourceSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to body of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    // set for checking multiple items
    std::multiset<
        std::pair<HalfWord, ResourceElement::ResourceType> > checkMultiple;

    for (Word i = 0; i < sect->elementCount(); i++) {
        ResourceElement* elem =
            dynamic_cast<ResourceElement*>(sect->element(i));

        assert(elem != NULL);

        // identification codes of resources must be unique among all
        // resources of the same type
        std::pair<HalfWord, ResourceElement::ResourceType>
            pairToFind(elem->id(), elem->type());

        // TODO: check that OP, SR and PORT share the same id space and don't
        // collide

        if (checkMultiple.find(pairToFind) != checkMultiple.end()) {
            bool multipleResourcesWithSameIdAndType = false;
            assert(multipleResourcesWithSameIdAndType);
        } else {
            checkMultiple.insert(pairToFind);
        }

        stream.writeHalfWord(elem->id());
        stream.writeByte(static_cast<Byte>(elem->type()));

        assert(elem->name() != NULL);

        SectionOffsetReplacer replacer(elem->name());
        replacer.resolve();

        stream.writeWord(elem->info());
    }
    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Returns the fixed size length of section elements.
 *
 * @return The fixed size length of section elements.
 */
Word
TPEFResourceSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

}
