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
 * @file TPEFDisassembler.hh
 *
 * Declaration of TPEFDisassembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TPEF_DISASSEMBLER_HH
#define TTA_TPEF_DISASSEMBLER_HH

#include <vector>

#include "BaseType.hh"
#include "Disassembler.hh"
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
class TPEFDisassembler : public Disassembler {
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
