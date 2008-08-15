/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
