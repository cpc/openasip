/**
 * @file UDataSection.hh
 *
 * Declaration of UDataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_UDATA_SECTION_HH
#define TTA_UDATA_SECTION_HH

#include "Section.hh"
#include "BaseType.hh"
#include "Chunk.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Uninitilized data section.
 */
class UDataSection : public RawSection {
public:
    virtual ~UDataSection();

    virtual SectionType type() const;

    virtual Byte byte(const Chunk* chunk) const;

protected:
    UDataSection(bool init);
    virtual Section* clone() const;

private:
    /// Protorype instance of section.
    static UDataSection proto_;
};
}

#endif
