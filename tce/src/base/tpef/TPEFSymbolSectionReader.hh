/**
 * @file TPEFSymbolSectionReader.hh
 *
 * Declaration of TPEFSymbolSectionReader.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SYMBOL_SECTION_READER_HH
#define TTA_TPEF_SYMBOL_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Reads symbol section from TPEF binary file.
 */
class TPEFSymbolSectionReader : public TPEFSectionReader {
protected:
    TPEFSymbolSectionReader();
    virtual ~TPEFSymbolSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFSymbolSectionReader(const TPEFSymbolSectionReader&);
    /// Assignment not allowed.
    TPEFSymbolSectionReader& operator=(TPEFSymbolSectionReader&);

    SymbolElement* createSymbol(
        SymbolElement::SymbolType symType,
        Word aValue,
        Word aSize,
        SectionId sectToBelong) const;

    /// Mask for getting type of symbol from st_info field of symbol element.
    static const Byte SYMBOL_TYPE_MASK;
    /// Prototype instance of TPEFSymbolSectionReader to be registered to
    /// SectionReader.
    static TPEFSymbolSectionReader proto_;
};
}

#endif
