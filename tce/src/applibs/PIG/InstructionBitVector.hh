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
 * @file InstructionBitVector.hh
 *
 * Declaration of InstructionBitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_BIT_VECTOR_HH
#define TTA_INSTRUCTION_BIT_VECTOR_HH

#include <set>
#include <map>

#include "BitVector.hh"
#include "Exception.hh"
#include "IndexBound.hh"

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
    void addIndexBoundsForReference(IndexBound bounds);

    void fixInstructionAddress(
        const TTAProgram::Instruction& instruction,
        unsigned int address)
        throw (OutOfRange);

    void markInstructionStartingPoint(unsigned int position);
    unsigned int instructionCount() const;
    unsigned int instructionStartingPoint(unsigned int index) const
        throw (OutOfRange);
    
private:
    /// A vector type that stores index bounds.
    typedef std::vector<IndexBound> IndexBoundTable;
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


