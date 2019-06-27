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
 * @file UDataSection.cc
 *
 * Non-inline definitions of UDataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#include "UDataSection.hh"

namespace TPEF {

UDataSection UDataSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
UDataSection::UDataSection(bool init) : RawSection() {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    setFlagNoBits();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
UDataSection::~UDataSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
UDataSection::type() const {
    return ST_UDATA;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
UDataSection::clone() const {
    return new UDataSection(false);
}

/**
 * Returns byte from section.
 *
 * In this case section contains only zeros.
 *
 * @param chunk The chunk in which offset byte is wanted.
 * @return Byte from the offset of the chunk.
 */
Byte
UDataSection::byte(const Chunk* /*chunk*/) const {
    return 0x00;
}

}
