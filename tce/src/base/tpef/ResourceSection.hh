/**
 * @file ResourceSection.hh
 *
 * Declaration of ResourceSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_RESOURCE_SECTION_HH
#define TTA_RESOURCE_SECTION_HH

#include "Section.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "ResourceElement.hh"

namespace TPEF {

/**
 * Prosessor Resource Table section.
 */
class ResourceSection : public Section {
public:
    virtual ~ResourceSection();

    virtual SectionType type() const;

    ResourceElement& findResource(
        ResourceElement::ResourceType aType, HalfWord anId) const;

    bool hasResource(
        ResourceElement::ResourceType aType, HalfWord anId) const;

protected:
    ResourceSection(bool init);
    virtual Section* clone() const;

private:
    /// Protorype instance of section.
    static ResourceSection proto_;
};
}

#endif
