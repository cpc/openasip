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
 * @file DataSymElement.cc
 *
 * Definition of DataSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DataSymElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
DataSymElement::DataSymElement() :
    SymbolElement(),
    reference_(&SafePointer::null),
    size_(0) {
}

/**
 * Destructor.
 */
DataSymElement::~DataSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
DataSymElement::type() const {
    return STT_DATA;
}

/**
 * Returns object which element refers.
 *
 * @return object which element refers.
 */
Chunk*
DataSymElement::reference() const {
    return dynamic_cast<Chunk*>(reference_->pointer());
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
DataSymElement::setReference(Chunk* aReference) {
    reference_ = SafePointer::replaceReference(reference_, aReference);
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
DataSymElement::setReference(
    const ReferenceManager::SafePointer* aReference) {
    reference_ = aReference;
}

/**
 * Returns size of the referred object in MAUs.
 *
 * @return Size of the referred object in MAUs.
 */
Word
DataSymElement::size() const {
    return size_;
}

/**
 * Sets size of referred object in MAUs.
 *
 * @param aSize of the referred object in MAUs.
 */
void
DataSymElement::setSize(Word aSize) {
    size_ = aSize;
}

}
