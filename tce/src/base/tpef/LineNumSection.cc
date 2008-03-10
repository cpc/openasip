/**
 * @file LineNumSection.cc
 *
 * Non-inline definitions of LineNumSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "LineNumSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

LineNumSection LineNumSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
LineNumSection::LineNumSection(bool init) : Section(),
    codeSection_(&SafePointer::null) {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagNoBits();
    unsetFlagVLen();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
LineNumSection::~LineNumSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
LineNumSection::type() const {
    return ST_LINENO;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
LineNumSection::clone() const {
    return new LineNumSection(false);
}

/**
 * Sets text section which instructions are referred in this section.
 *
 * @param Section to set.
 */
void
LineNumSection::setCodeSection(
    const ReferenceManager::SafePointer* codeSect) {
    codeSection_ = codeSect;
}

/**
 * Sets text section which instructions are referred in this section.
 *
 * @param Section to set.
 */
void
LineNumSection::setCodeSection(CodeSection* codeSect) {
    codeSection_ = SafePointer::replaceReference(codeSection_, codeSect);
}

/**
 * Returns text section which lines are stored in this section.
 *
 * @return Text section which lines are stored in this section.
 */
CodeSection*
LineNumSection::codeSection() const {
    return dynamic_cast<CodeSection*>(codeSection_->pointer());
}

}
