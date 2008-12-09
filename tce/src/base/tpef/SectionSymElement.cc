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
 * @file SectionSymElement.cc
 *
 * Definition of SectionSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionSymElement.hh"

namespace TPEF {

/**
 * Constructor.
 */
SectionSymElement::SectionSymElement() :
    SymbolElement(), value_(0), size_(0) {

}

/**
 * Destructor.
 */
SectionSymElement::~SectionSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
SectionSymElement::type() const {
    return STT_SECTION;
}

/**
 * Returns value of the element.
 *
 * This might be for example relocation information.
 *
 * @return Value of the element.
 */
Word
SectionSymElement::value() const {
    return value_;
}

/**
 * Sets value of the element.
 *
 * This might be for example relocation information.
 *
 * @param aValue Value of the element.
 */
void
SectionSymElement::setValue(Word aValue) {
    value_ = aValue;
}

/**
 * Returns size of the referred object in MAUs.
 *
 * @return Size of the referred object in MAUs.
 */
Word
SectionSymElement::size() const {
    return size_;
}

/**
 * Sets size of referred object in MAUs.
 *
 * @param aSize of the referred object in MAUs.
 */
void
SectionSymElement::setSize(Word aSize) {
    size_ = aSize;
}

}
