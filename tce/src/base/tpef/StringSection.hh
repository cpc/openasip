/**
 * @file StringSection.hh
 *
 * Declaration of StringSection class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 * @note reviewed 17 October 2003 by am, pj, rm, kl
 *
 * @note rating: yellow
 */

#ifndef TTA_STRINGSECTION_HH
#define TTA_STRINGSECTION_HH

#include "Section.hh"
#include "DataSection.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Represents a string table section.
 */
class StringSection : public DataSection {
public:
    virtual ~StringSection();

    virtual SectionType type() const;

    std::string chunk2String(const Chunk* chunk) const
        throw (UnexpectedValue);

    Chunk* string2Chunk(const std::string &str);

protected:
    StringSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype of the section.
    static StringSection proto_;
};
}

#endif
