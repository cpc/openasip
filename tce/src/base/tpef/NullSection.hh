/**
 * @file NullSection.hh
 *
 * Declaration of NullSection class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_NULL_SECTION_HH
#define TTA_NULL_SECTION_HH

#include "Section.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Null section.
 */
class NullSection : public Section {
public:
    virtual ~NullSection();

    virtual SectionType type() const;

protected:
    NullSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static NullSection proto_;
};
}

#endif
