/**
 * @file AOutRelocationSectionReader.hh
 *
 * Declaration of AOutRelocationSectionReader.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_RELOCATION_SECTION_READER_HH
#define TTA_AOUT_RELOCATION_SECTION_READER_HH

#include "AOutSectionReader.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "BinaryReader.hh"
#include "Exception.hh"
#include "RelocElement.hh"
#include "ReferenceKey.hh"

namespace TPEF {

/**
 * Reads relocation sections of a.out binary.
 */
class AOutRelocationSectionReader : public AOutSectionReader {
public:
    virtual ~AOutRelocationSectionReader();

protected:
    AOutRelocationSectionReader();

    virtual void finalize(Section* section) const;

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /**
     * Relocation types of a.out
     */
    enum RelocType {
        RELOC_8,   ///< 1 byte relocation 
        RELOC_16,  ///< 2 bytes relocation 
        RELOC_32,  ///< 4 bytes relocation
        NO_RELOC   ///< no relocation
    };

    void initializeRelocElement(
        BinaryStream& stream,
        RelocElement* elem,
        SectionId refSectionID,
        AOutReader* reader) const
        throw (UnexpectedValue, UnreachableStream, OutOfRange);

    bool checkIfExtern(Word word) const;

    /// Copying not allowed.
    AOutRelocationSectionReader(const AOutRelocationSectionReader&);
    /// Assignment not allowed.
    AOutRelocationSectionReader& operator=(AOutRelocationSectionReader&);

    RelocElement::RelocType aOutToTPEFRelocType(
        RelocType aOutRelocType) const;

    /// Class-wide (unique) prototype instance of AOutRelocationSectionReader
    /// registered into SectionReader.
    static AOutRelocationSectionReader proto_;

    /// Mask for extracting relocation type.
    static const Byte RELOCATION_TYPE_MASK;
};
}

#include "AOutRelocationSectionReader.icc"

#endif
