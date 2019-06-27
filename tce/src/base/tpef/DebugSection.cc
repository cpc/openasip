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
 * @file DebugSection.cc
 *
 * Definition of DebugSection class.
 *
 * @author Mikael Lepist� 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DebugSection.hh"

namespace TPEF {

DebugSection DebugSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
DebugSection::DebugSection(bool init) : Section() {
    if (init) {
        Section::registerSection(this);
    }
    setFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
DebugSection::~DebugSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
DebugSection::type() const {
    return ST_DEBUG;
}

/**
 * Creates an instance of class.
 *
 * @return Newly created section.
 */
Section*
DebugSection::clone() const {
    return new DebugSection(false);
}

} // namespace TPEF
