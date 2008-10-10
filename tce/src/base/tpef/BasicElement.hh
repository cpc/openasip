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
 * @file BasicElement.hh
 *
 * Declaration of BasicElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BASIC_ELEMENT_HH
#define TTA_BASIC_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Basic element type.
 *
 * Allows representing data of any element which doesn't have own class
 * for representing it. Can contain any number of bytes inside.
 */
class BasicElement : public SectionElement {
public:
    BasicElement();
    virtual ~BasicElement();

    void addByte(const Byte& data);

    Byte byte(Word index) const
        throw (OutOfRange);

    Word length() const;

private:
    /// Storage of bytes that element contains.
    std::vector<Byte> bytes_;
};
}

#include "BasicElement.icc"

#endif
