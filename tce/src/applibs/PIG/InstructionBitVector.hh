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


