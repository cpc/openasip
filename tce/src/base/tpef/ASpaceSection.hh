/**
 * @file ASpaceSection.hh
 *
 * Declaration of ASpaceSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_ASPACESECTION_HH
#define TTA_ASPACESECTION_HH

#include "Section.hh"

namespace TPEF {

/**
 * Address space section.
 */
class ASpaceSection : public Section {
public:
    virtual ~ASpaceSection();

    virtual SectionType type() const;

    bool isUndefined(ASpaceElement *aSpace) const;

    void setUndefinedASpace(ASpaceElement* aSpace);
    ASpaceElement* undefinedASpace() const;

protected:
    ASpaceSection(bool init);

    virtual Section* clone() const;

private:
    /// Prototype instance.
    static ASpaceSection proto_;

    /// Undefined address space. This element is not allowed to be 
    /// removed from section is recognized as special undefined address space.
    const ReferenceManager::SafePointer* undefinedElement_;
};
}

#endif
