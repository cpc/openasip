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
 * @file DataSymElement.cc
 *
 * Definition of DataSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
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
