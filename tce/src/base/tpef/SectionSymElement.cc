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
 * @file SectionSymElement.cc
 *
 * Definition of SectionSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
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
