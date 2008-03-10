/**
 * @file TPEFTools.cc
 *
 * Implementation of TPEFTools class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: yellow
 */

#include "TPEFTools.hh"

#include "MapTools.hh"
#include "Binary.hh"
#include "ASpaceSection.hh"
#include "StringSection.hh"
#include "ASpaceElement.hh"
#include "ResourceElement.hh"
#include "ResourceSection.hh"
#include "RelocElement.hh"
#include "CodeSection.hh"
#include "InstructionElement.hh"
#include "Application.hh"

namespace TPEF {

/**
 * Constructor.
 *
 * @param aTpef TPEF that is accessed with toolkit.
 */
TPEFTools::TPEFTools(const Binary &aTpef) : tpef_(aTpef) {
}

/**
 * Clears cache of object.
 */
void
TPEFTools::clearCache() const {
    relocationCache_.clear();
}

/**
 * Return section of the requested element.
 *
 * @param element Element whose section is needed.
 * @return Section of the requested element.
 */
Section&
TPEFTools::sectionOfElement(const SectionElement &element) {
    return sectionOfElement(tpef_, element);
}

/**
 * Finds name string of a resource.
 *
 * @param type Type of the resource.
 * @param resourceId Id of the resource.
 * @return Name of the requested resource.
 */
std::string
TPEFTools::resourceName(ResourceElement::ResourceType type,
                        HalfWord resourceId) const {
    return resourceName(tpef_, type, resourceId);
}

/**
 * Finds name string of an address space element.
 *
 * @param aSpace Element whose name is needed.
 * @return Name of the requested address space.
 */
std::string
TPEFTools::addressSpaceName(const ASpaceElement& aSpace) const {
    return addressSpaceName(tpef_, aSpace);
}

/**
 * Checks if the requested immediate or chunk is relocated.
 *
 * @param element Element whose name is needed.
 * @return True if the requested element has corresponding relocation element.
 */
bool
TPEFTools::hasRelocation(const SectionElement& element) const {
    initRelocationCache();
    return MapTools::containsKey(relocationCache_, &element);
}

/**
 * Retrieves the relocation element requested immediate or chunk is relocated.
 *
 * @param element Element relocations are checked.
 * @return Relocation element for the element given in parameter.
 */
const RelocElement&
TPEFTools::relocation(const SectionElement& element) const {
    initRelocationCache();
    return *MapTools::valueForKey<const RelocElement*>(
        relocationCache_, &element);
}

/**
 * Return section of the requested element.
 *
 * NOTE: method does not work for LineNumElements
 *       (line number section element desing is wrong).
 *
 * @param bin TPEF which is checked.
 * @param element Element whose section is needed.
 * @return Section of the requested element.
 */
Section&
TPEFTools::sectionOfElement(
    const Binary &bin,
    const SectionElement &element) {

    // check if chunkable element
    const Chunk* chunk = dynamic_cast<const Chunk*>(&element);

    if (chunk != NULL) {
        for (Word i = 0; i < bin.sectionCount(); i++) {
            RawSection* currSect =
                dynamic_cast<RawSection*>(bin.section(i));

            if (currSect == NULL) {
                continue;
            }

            if (chunk->offset() < currSect->length() &&
                currSect->chunk(chunk->offset()) == chunk) {
                return *currSect;
            }
        }

    } else {
        for (Word i = 0; i < bin.sectionCount(); i++) {
            Section* currSect = bin.section(i);

            if (currSect->isChunkable()) {
                continue;
            }

            // optimization for code section
            if (currSect->type() == Section::ST_CODE) {
                CodeSection* codeSect =
                    dynamic_cast<CodeSection*>(currSect);

                const InstructionElement* instr =
                    dynamic_cast<const InstructionElement*>(&element);

                if (instr == NULL) {
                    continue;
                }

                if (codeSect->isInSection(*instr)) {
                    return *currSect;
                } else {
                    continue;
                }
            }

            // other sections a bit slower
            for (Word j = 0; j < currSect->elementCount(); j++) {
                if (&element == currSect->element(j)) {
                    return *currSect;
                }
            }
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Requested element is not found from any section of binary.");
}

/**
 * Finds name string of a resource.
 *
 * @param bin TPEF which is checked.
 * @param type Type of the resource.
 * @param resourceId Id of the resource.
 * @return Name of the requested resource.
 */
std::string
TPEFTools::resourceName(
    const Binary &bin, ResourceElement::ResourceType type, 
    HalfWord resourceId) {

    for (Word i = 0; i < bin.sectionCount(Section::ST_MR); i++) {
        ResourceSection* resources =
            dynamic_cast<ResourceSection*>(bin.section(Section::ST_MR, i));

        assert(resources != NULL);

        if (resources->hasResource(type, resourceId)) {

            ResourceElement& resource =
                dynamic_cast<ResourceElement&>(
                    resources->findResource(type, resourceId));

            StringSection *strings =
                dynamic_cast<StringSection*>(resources->link());

            assert(strings != NULL);

            return strings->chunk2String(resource.name());
        }
    }
    
    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "Resource wasn't found id:" + Conversion::toString(resourceId));
}

/**
 * Finds name string of an address space element.
 *
 * @param bin TPEF which is checked.
 * @param aSpace Element whose name is needed.
 * @return Name of the requested address space.
 */
std::string
TPEFTools::addressSpaceName(const Binary &bin, const ASpaceElement& aSpace) {

    assert(bin.sectionCount(Section::ST_ADDRSP) == 1);

    Section *aSpaceSection = bin.section(Section::ST_ADDRSP, 0);

    StringSection *strings = dynamic_cast<StringSection*>(
        aSpaceSection->link());

    assert(strings != NULL);

    return strings->chunk2String(aSpace.name());
}

/**
 * Checks if the requested immediate or chunk is relocated.
 *
 * @param bin TPEF which is checked.
 * @param element Element whose name is needed.
 * @return True if the requested element has corresponding relocation element.
 */
bool
TPEFTools::hasRelocation(const Binary& bin, const SectionElement& element) {

    for (Word i = 0; i < bin.sectionCount(Section::ST_RELOC); i++) {
        Section *relocSect = bin.section(Section::ST_RELOC, i);

        for (Word j = 0; j < relocSect->elementCount(); j++) {
            RelocElement *reloc =
                dynamic_cast<RelocElement*>(relocSect->element(j));

            assert(reloc != NULL);

            if (reloc->location() == &element) {
                return true;
            }
        }
    }

    return false;
}

/**
 * Retrieves the relocation element requested immediate or chunk is relocated.
 *
 * @param bin TPEF which is checked.
 * @param element Element relocations are checked.
 * @return Relocation element for the element given in parameter.
 */
const RelocElement&
TPEFTools::relocation(const Binary& bin, const SectionElement& element) {

    for (Word i = 0; i < bin.sectionCount(Section::ST_RELOC); i++) {
        Section *relocSect = bin.section(Section::ST_RELOC, i);

        for (Word j = 0; j < relocSect->elementCount(); j++) {
            RelocElement *reloc =
                dynamic_cast<RelocElement*>(relocSect->element(j));

            assert(reloc != NULL);

            if (reloc->location() == &element) {
                return *reloc;
            }
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        "There is no relocation for requested element.");
}


/**
 * Inits cache for hasRelocation and relocation methods.
 */
void
TPEFTools::initRelocationCache() const {

    if (relocationCache_.empty()) {
        for (Word i = 0; i < tpef_.sectionCount(Section::ST_RELOC); i++) {
            Section *relocSect = tpef_.section(Section::ST_RELOC, i);

            for (Word j = 0; j < relocSect->elementCount(); j++) {
                RelocElement *reloc =
                    dynamic_cast<RelocElement*>(relocSect->element(j));

                assert(reloc != NULL);

                relocationCache_[reloc->location()] = reloc;
            }
        }
    }
}

}

