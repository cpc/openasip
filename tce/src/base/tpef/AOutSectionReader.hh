/**
 * @file AOutSectionReader.hh
 *
 * Declaration of AOutSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_SECTION_READER_HH
#define TTA_AOUT_SECTION_READER_HH

#include "SectionReader.hh"
#include "AOutReader.hh"

namespace TPEF {

/**
 * Abstract base class for AOutSectionReaders.
 */
class AOutSectionReader : public SectionReader {
public:
    virtual ~AOutSectionReader();

protected:
    AOutSectionReader();

    virtual BinaryReader* parent() const;

private:
    /// Copying is not allowed.
    AOutSectionReader(const AOutSectionReader&);
};
}

#endif
