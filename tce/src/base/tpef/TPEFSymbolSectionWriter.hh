/**
 * @file TPEFSymbolSectionWriter.hh
 *
 * Declaration of TPEFSymbolSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SYMBOL_SECTION_WRITER_HH
#define TTA_TPEF_SYMBOL_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Writes TPEF symbol table to stream.
 */
class TPEFSymbolSectionWriter : public TPEFSectionWriter {
protected:
    TPEFSymbolSectionWriter();
    virtual ~TPEFSymbolSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFSymbolSectionWriter(const TPEFSymbolSectionWriter&);
    /// Assignment not allowed.
    TPEFSymbolSectionWriter operator=(const TPEFSymbolSectionWriter&);

    void writeValueAndSize(
        BinaryStream &stream,
        const SymbolElement* elem) const;

    /// An unique instance of class.
    static const TPEFSymbolSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
