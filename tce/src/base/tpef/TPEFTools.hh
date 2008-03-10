/**
 * @file TPEFTools.hh
 *
 * Declaration of TPEFTools.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_TOOLS_HH
#define TTA_TPEF_TOOLS_HH

#include <string>

#include "Application.hh"
#include "ResourceElement.hh"

namespace TPEF {

class Binary;
class ASpaceElement;
class SectionElement;
class RelocElement;
class Section;

/**
 * Contains many helper functions that makes TPEF handling
 * a bit easier.
 *
 * These are kind of functions that doesn't fit to any Sections' or 
 * SectionElements' interfaces. 
 *
 * Class can be used stright through static interface, 
 * or client may create instance of TPEFTools, which enables 
 * caching functionality. Basically, if TPEFTools is highly utilized
 * it should be used as a object.
 */
class TPEFTools {
public:
    TPEFTools(const Binary &aTpef);

    // clears cache tables
    void clearCache() const;

    Section& sectionOfElement(
        const SectionElement& element);

    std::string resourceName(
        ResourceElement::ResourceType type, 
        HalfWord resourceId) const;

    std::string addressSpaceName(
        const ASpaceElement& aSpace) const;

    bool hasRelocation(const SectionElement& element) const;

    const RelocElement& relocation(const SectionElement& element) const;

    static Section& sectionOfElement(
        const Binary& bin, 
        const SectionElement& element);

    static std::string resourceName(
        const Binary& bin,
        ResourceElement::ResourceType type,
        HalfWord resourceId);

    static std::string addressSpaceName(
        const Binary& bin,
        const ASpaceElement& aSpace);

    static bool hasRelocation(
        const Binary& bin,
        const SectionElement& element);

    static const RelocElement& relocation(
        const Binary& bin,
        const SectionElement& element);

private:
    /// TPEF whose resources are accessed.
    const Binary& tpef_;
    
    /// Cache of Chunks and immediates, which are relocated.
    mutable std::map<
        const SectionElement*, const RelocElement*>  relocationCache_;

    void initRelocationCache() const;

};

}

#endif
