/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file TPEFNullSectionWriter.cc
 *
 * Definitions of TPEFNullSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFNullSectionWriter.hh"
#include "Exception.hh"
#include "SectionSizeReplacer.hh"
#include "TPEFBaseType.hh"
#include "ReferenceKey.hh"
#include "SafePointer.hh"
#include "BinaryStream.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::FileOffsetKey;

const TPEFNullSectionWriter TPEFNullSectionWriter::instance_;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFNullSectionWriter::TPEFNullSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFNullSectionWriter::~TPEFNullSectionWriter() {
}

/**
 * Returns which type of sections can be written out by this writer.
 *
 * @return The type of section that writer can write out.
 */
Section::SectionType
TPEFNullSectionWriter::type() const {
    return Section::ST_NULL;
}

/**
 * Returns the size of section elements. The null section has no elements,
 * so the size of elements is undefined. Returns always zero.
 *
 * @param section Section whose element size is to be written out (not used).
 * @return Zero.
 */
Word
TPEFNullSectionWriter::elementSize(const Section*) const {
    return 0;
}

/**
 * Returns the identification code of the section.
 *
 * Zero is the numeric code reserved for identifying null sections.
 *
 * @return Zero.
 */
SectionId
TPEFNullSectionWriter::getSectionId() const {
    return 0;
}

/**
 * Writes the size of a null section into an output stream.
 *
 * The size of a null section is always zero.
 *
 * @param stream Output stream.
 * @param sect Section whose size has to be written out (not used).
 */
void
TPEFNullSectionWriter::writeSize(BinaryStream& stream,
    const Section*) const {

    stream.writeWord(0);
}

}
