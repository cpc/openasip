/**
 * @file RelocSection.hh
 *
 * Declaration of RelocSection class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_RELOC_SECTION_HH
#define TTA_RELOC_SECTION_HH

#include "Section.hh"
#include "SafePointer.hh"

namespace TPEF {

/**
 * Relocation table section.
 */
class RelocSection : public Section {

public:
    virtual ~RelocSection();

    virtual SectionType type() const;

    Section* referencedSection() const;
    void setReferencedSection(Section* section);
    void setReferencedSection(const ReferenceManager::SafePointer* section);

protected:
    RelocSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static RelocSection proto_;
    /// Section whose elements are being relocated by
    /// relocations of this section.
    const ReferenceManager::SafePointer* refSection_;
};
}

#include "RelocSection.icc"

#endif
