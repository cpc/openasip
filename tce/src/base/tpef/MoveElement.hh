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
 * @file MoveElement.hh
 *
 * Declaration of MoveElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rm, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_MOVE_ELEMENT_HH
#define TTA_MOVE_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"

namespace TPEF {

/**
 * Move instruction.
 */
class MoveElement : public InstructionElement {
public:
    /**
     * Type of resource that is referred by unit / index pair.
     */
    enum FieldType {
        MF_NULL, ///< Illegal field.
        MF_RF,   ///< Register file.
        MF_IMM,  ///< Immediate.
        MF_UNIT  ///< Function unit.
    };

    MoveElement();
    virtual ~MoveElement();

    bool isEmpty() const;
    void setEmpty(bool flag);

    Byte bus() const;
    void setBus(Byte aBus);

    FieldType sourceType() const;
    void setSourceType(FieldType aType);

    Byte sourceUnit() const;
    void setSourceUnit(Byte aSourceUnit);

    HalfWord sourceIndex() const;
    void setSourceIndex(HalfWord aSourceIndex);

    FieldType destinationType() const;
    void setDestinationType(FieldType aType);

    Byte destinationUnit() const;
    void setDestinationUnit(Byte aDestinationUnit);

    HalfWord destinationIndex() const;
    void setDestinationIndex(HalfWord aDestinationIndex);

    FieldType guardType() const;
    void setGuardType(FieldType gType);

    Byte guardUnit() const;
    void setGuardUnit(Byte aGuardUnit);

    HalfWord guardIndex() const;
    void setGuardIndex(HalfWord aGuardIndex);

    bool isGuarded() const;
    void setGuarded(bool flag);

    bool isGuardInverted() const;
    void setGuardInverted(bool flag);

private:
    /// Empty move bit.
    bool isEmpty_;
    /// Move bus identifier.
    Byte bus_;
    /// The type of Move source.
    FieldType sourceType_;
    /// The type of move destination.
    FieldType destinationType_;
    /// Move source unit or register file.
    Byte sourceUnit_;
    /// Move destination unit or register file identifier.
    Byte destinationUnit_;
    /// Move source register index.
    HalfWord sourceIndex_;
    /// Move destination register index.
    HalfWord destinationIndex_;
    /// Move guard enable bit.
    bool isGuarded_;
    /// Move guard inverted bit.
    bool isGuardInverted_;
    /// Guard type.
    FieldType guardType_;
    /// Guard register file / unit identifier.
    Byte guardUnit_;
    /// Guard register index.
    HalfWord guardIndex_;
};
}

#include "MoveElement.icc"

#endif
