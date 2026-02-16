/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file AOutTextSectionReader.hh
 *
 * Declaration of AOutTextSectionReader.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 23 October 2003 by pj, am, ll, jn
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_TEXT_SECTION_READER_HH
#define TTA_AOUT_TEXT_SECTION_READER_HH

#include "TPEFBaseType.hh"
#include "AOutSectionReader.hh"
#include "Exception.hh"

namespace TPEF {
    class ImmediateElement;
    class MoveElement;
/**
 * Reads text sections of a.out format.
 */
class AOutTextSectionReader : public AOutSectionReader {
public:
    static const int OFFSET_TO_IMMEDIATE_VALUE;

    virtual ~AOutTextSectionReader();

    AOutTextSectionReader(const AOutTextSectionReader&) = delete;
    AOutTextSectionReader& operator=(const AOutTextSectionReader&) = delete;

protected:
    AOutTextSectionReader();
    virtual Section::SectionType type() const;

    virtual void readData(BinaryStream& stream, Section* section) const;

private:
    void initializeImmediateMove(
        BinaryStream& stream, MoveElement* move,
        ImmediateElement* immediate) const;

    void initializeMove(BinaryStream& stream, MoveElement* move) const;

    void updateMoveDestination(MoveElement* move, const HalfWord dest) const;

    void updateMoveSource(MoveElement* move, const Word src) const;

    Word convertAOutIndexToTPEF(const Word reg) const;

    void setReference(
        InstructionElement* elem, SectionOffset sectionOffset,
        SectionId sectionID) const;

    /// Prototype to be registered to SectionReader.
    static AOutTextSectionReader proto_;
};
}

#endif
