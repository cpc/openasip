/**
 * @file LineNumSection.hh
 *
 * Declaration of LineNumSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LINE_NUM_SECTION_HH
#define TTA_LINE_NUM_SECTION_HH

#include "Section.hh"
#include "CodeSection.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Prosessor Line number section.
 */
class LineNumSection : public Section {
public:
    virtual ~LineNumSection();

    virtual SectionType type() const;

    void setCodeSection(const ReferenceManager::SafePointer* codeSect);
    void setCodeSection(CodeSection* codeSect);
    CodeSection* codeSection() const;

protected:
    LineNumSection(bool init);
    virtual Section* clone() const;

private:
    /// Stores text section which we refer with lines.
    const ReferenceManager::SafePointer* codeSection_;
    /// Protorype instance of section.
    static LineNumSection proto_;
};
}

#endif
