/**
 * @file AOutTextSectionReader.hh
 *
 * Declaration of AOutTextSectionReader.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 23 October 2003 by pj, am, ll, jn
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_TEXT_SECTION_READER_HH
#define TTA_AOUT_TEXT_SECTION_READER_HH

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"
#include "AOutSectionReader.hh"
#include "BinaryStream.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "AOutReader.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"

namespace TPEF {

/**
 * Reads text sections of a.out format.
 */
class AOutTextSectionReader : public AOutSectionReader {
public:
    static const int OFFSET_TO_IMMEDIATE_VALUE;

    virtual ~AOutTextSectionReader();

protected:
    AOutTextSectionReader();
    virtual Section::SectionType type() const;

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

private:
    void initializeImmediateMove(
        BinaryStream& stream,
        MoveElement* move,
        ImmediateElement* immediate) const
        throw (OutOfRange, UnreachableStream);

    void initializeMove(BinaryStream& stream, MoveElement* move) const
        throw (OutOfRange, UnreachableStream);

    void updateMoveDestination(MoveElement* move, const HalfWord dest) const
        throw (OutOfRange);

    void updateMoveSource(MoveElement* move, const Word src) const
        throw (OutOfRange);

    Word convertAOutIndexToTPEF(const Word reg) const;

    void setReference(
        InstructionElement* elem,
        SectionOffset sectionOffset,
        SectionId sectionID) const
        throw (KeyAlreadyExists);

    /// Copying not allowed.
    AOutTextSectionReader(const AOutTextSectionReader&);
    /// Assignment not allowed.
    AOutTextSectionReader& operator=(const AOutTextSectionReader&);

    /// Prototype to be registered to SectionReader.
    static AOutTextSectionReader proto_;
};
}

#endif
