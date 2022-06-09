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
 * @file TPEFDisassembler.hh
 *
 * Declaration of TPEFDisassembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TPEF_DISASSEMBLER_HH
#define TTA_TPEF_DISASSEMBLER_HH

#include <vector>

#include "BaseType.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "Binary.hh"

class DisassemblyInstruction;
class DisassemblyElement;

/**
 * TPEF Disassembler.
 *
 * Retrievs data from TPEF and builds Disassembler instructions
 * out of TPEF hierarchy.
 *
 * Contains internal cache to speedup instruction fetching, so if
 * instruction ordering of TPEF is changed cache will not be
 * valid anymore and must be cleared.
 *
 * NOTE: Class disassembles only the first of code sections in binary.
 */
class TPEFDisassembler {
public:
    TPEFDisassembler(const TPEF::Binary &aTpef);
    virtual ~TPEFDisassembler();

    virtual DisassemblyInstruction* createInstruction(
        Word instructionIndex) const;

    virtual Word startAddress() const;
    virtual Word instructionCount() const;

    void clearCache() const;

protected:
    typedef std::pair<Word,Word> ImmediateKey;
    typedef std::map<ImmediateKey, TPEF::ImmediateElement*> ImmediateMap;

    void initCache() const;

    DisassemblyElement* createDisassemblyElement(
	TPEF::MoveElement::FieldType type, Word unit, Word index,
	ImmediateMap &immediateMap) const;

private:
    /// Binary where data for disassembler is retrieved.
    const TPEF::Binary *tpef_;

    /// Cache of starting elements of instructions.
    mutable std::vector<Word> instructionStartCache_;
};
#endif
