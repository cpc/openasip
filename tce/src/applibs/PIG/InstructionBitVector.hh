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
 * @file InstructionBitVector.hh
 *
 * Declaration of InstructionBitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_BIT_VECTOR_HH
#define TTA_INSTRUCTION_BIT_VECTOR_HH

#include <set>
#include <map>

#include "BitVector.hh"
#include "Exception.hh"

namespace TTAProgram {
    class Instruction;
}

/**
 * A bit vector class that provides instruction relocation capabilities.
 */
class InstructionBitVector : public BitVector {
public:
    InstructionBitVector();
    InstructionBitVector(const InstructionBitVector& toCopy);
    virtual ~InstructionBitVector();

    void pushBack(const InstructionBitVector& bits)
        throw (OutOfRange);
    void pushBack(const BitVector& bits);
    InstructionBitVector* subVector(
        unsigned int firstIndex, unsigned int lastIndex) const;

    void startSettingInstructionReference(
        const TTAProgram::Instruction& instruction);
    void addIndexBoundsForReference(unsigned int start, unsigned int end);

    void fixInstructionAddress(
        const TTAProgram::Instruction& instruction,
        unsigned int address)
        throw (OutOfRange);

    void markInstructionStartingPoint(unsigned int position);
    unsigned int instructionCount() const;
    unsigned int instructionStartingPoint(unsigned int index) const
        throw (OutOfRange);
    
private:
    /// A vector type that stores pairs of indexes.
    typedef std::vector<std::pair<unsigned int, unsigned int> > 
    IndexBoundTable;
    /// A set type that stores IndexBoundTables.
    typedef std::set<IndexBoundTable*> IndexBoundSet;
    /// A map type that maps Instructions to IndexBoundSets.
    typedef std::map<const TTAProgram::Instruction*, IndexBoundSet*> 
    ReferenceMap;
    /// A vector type that stores instruction boundaries.
    typedef std::vector<unsigned int> BoundaryTable;
    /// A map type for instruction addresses.
    typedef std::map<const TTAProgram::Instruction*, unsigned int>
    InstructionAddressTable;

    IndexBoundSet& indexBounds(
        const TTAProgram::Instruction& instruction);
    void addIndexBoundTables(const IndexBoundSet& from, IndexBoundSet& to);
    void fixBits(const IndexBoundTable& indexes, unsigned int value)
        throw (OutOfRange);
    static unsigned int availableSize(const IndexBoundTable& indexes);

    /// Contains information of the parts of the bit vector that refer to
    /// an instruction address.
    ReferenceMap references_;
    /// IndexBoundTable being under construction.
    IndexBoundTable* currentTable_;
    /// Stores the instruction boundaries.
    BoundaryTable instructionBoundaries_;
    /// Addresses of the instructions.
    InstructionAddressTable instructionAddresses_;
};

#endif


