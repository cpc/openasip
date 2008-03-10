/**
 * @file DebugSection.hh
 *
 * Declaration of DebugSection class.
 *
 * @author Mikael Lepistö 2006 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DEBUG_SECTION_HH
#define TTA_DEBUG_SECTION_HH

#include "Section.hh"

namespace TPEF {

/**
 * Debug section.
 */
class DebugSection : public Section {
public:
    virtual ~DebugSection();

    virtual SectionType type() const;

protected:
    DebugSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static DebugSection proto_;
};
}

#endif
