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
 * @file ImmediateElement.hh
 *
 * Declaration of ImmediateElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rl, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_IMMEDIATE_ELEMENT_HH
#define TTA_IMMEDIATE_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Immediate data element of CodeSection.
 */
class ImmediateElement : public InstructionElement {
public:
    ImmediateElement();
    virtual ~ImmediateElement();

    bool isInline() const;

    void addByte(Byte aByte);

    Byte byte(unsigned int index) const
        throw (OutOfRange);

    void setByte(unsigned int index, Byte aValue)
        throw (OutOfRange);

    void setWord(Word aValue);

    Word word() const
        throw (OutOfRange);

    unsigned int length() const;
    
    Byte destinationUnit() const;
    void setDestinationUnit(Byte aDestinationUnit);

    Byte destinationIndex() const;
    void setDestinationIndex(Byte aDestinationIndex);

private:
    /// Value of immediate.
    std::vector<Byte> value_;
    /// Destination unit of immediate.
    Byte destinationUnit_;
    /// Destination index of immediate.
    Byte destinationIndex_;
};
}

#include "ImmediateElement.icc"

#endif
