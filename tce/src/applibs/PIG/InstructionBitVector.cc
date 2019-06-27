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
 * @file InstructionBitVector.cc
 *
 * Implementation of InstructionBitVector class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "InstructionBitVector.hh"
#include "MapTools.hh"
#include "MathTools.hh"
#include "Application.hh"

using std::string;
using TTAProgram::Instruction;

/**
 * The constructor.
 */
InstructionBitVector::InstructionBitVector() : 
    BitVector(), currentTable_(NULL) {
}


/**
 * The copy constructor.
 *
 * @param toCopy Bit vector to copy.
 */
InstructionBitVector::InstructionBitVector(
    const InstructionBitVector& toCopy) : 
    BitVector(toCopy), currentTable_(NULL), 
    instructionBoundaries_(toCopy.instructionBoundaries_),
    instructionAddresses_(toCopy.instructionAddresses_) {

    // copy reference map
    for (ReferenceMap::const_iterator iter = toCopy.references_.begin();
         iter != toCopy.references_.end(); iter++) {
        const Instruction* instruction = (*iter).first;
        IndexBoundSet* indexBounds = (*iter).second;
        IndexBoundSet* newSet = new IndexBoundSet;
        for (IndexBoundSet::const_iterator iter = indexBounds->begin();
             iter != indexBounds->end(); iter++) {
            IndexBoundTable* table = *iter;
            IndexBoundTable* newTable = new IndexBoundTable(*table);
            newSet->insert(newTable);
            if (toCopy.currentTable_ == table) {
                currentTable_ = newTable;
            }
        }
        references_.insert(
            std::pair<const Instruction*, IndexBoundSet*>(
                instruction, newSet));
    }    
}
    

/**
 * The destructor.
 */
InstructionBitVector::~InstructionBitVector() {
}


/**
 * Concatenates the given instruction bit vector to this vector.
 *
 * Fixes the instruction references of the given bit vector with the known
 * instruction addresses.
 *
 * @param bits The bit vector to be concatenated to this vector.
 * @exception OutOfRange If some of the instruction references cannot be
 *                       fixed due to insufficient bit width.
 */
void
InstructionBitVector::pushBack(const InstructionBitVector& bits) {
    for (ReferenceMap::const_iterator iter = bits.references_.begin();
         iter != bits.references_.end(); iter++) {
        const Instruction* instruction = (*iter).first;
        IndexBoundSet* indexBoundSet = (*iter).second;
        if (!MapTools::containsKey(references_, instruction)) {
            IndexBoundSet* newSet = new IndexBoundSet;
            addIndexBoundTables(*indexBoundSet, *newSet);
            references_.insert(
                std::pair<const Instruction*, IndexBoundSet*>(
                    instruction, newSet));
        } else {
            IndexBoundSet* oldSet = MapTools::valueForKey<IndexBoundSet*>(
                references_, instruction);
            addIndexBoundTables(*indexBoundSet, *oldSet);
        }
    }

    InstructionBitVector bitsCopy(bits);
    
    // fix the instruction references of the given bit vector
    for (ReferenceMap::const_iterator iter = bits.references_.begin();
         iter != bits.references_.end(); iter++) {
        const Instruction* instruction = iter->first;
        IndexBoundSet* indexBoundSet = iter->second;
        if (MapTools::containsKey(instructionAddresses_, instruction)) {
            for (IndexBoundSet::const_iterator setIter = 
                     indexBoundSet->begin(); 
                 setIter != indexBoundSet->end(); setIter++) {
                IndexBoundTable* table = *setIter;
                bitsCopy.fixBits(
                    *table, 
                    MapTools::valueForKey<unsigned int>(
                        instructionAddresses_, instruction));
            }
        }
    }           
         
    insert(end(), bitsCopy.begin(), bitsCopy.end());
}

/**
 * Concatenates the given bit vector to this vector.
 *
 * @param bits The bit vector to be concatenated to this vector.
 */
void
InstructionBitVector::pushBack(const BitVector& bits) {
    BitVector::pushBack(bits);
}


/**
 * Starts setting an instruction reference.
 *
 * @param instruction The instruction being referenced.
 */
void
InstructionBitVector::startSettingInstructionReference(
    const TTAProgram::Instruction& instruction) {

    IndexBoundSet& setForInstruction = indexBounds(instruction);
    IndexBoundTable* newTable = new IndexBoundTable;
    currentTable_ = newTable;
    setForInstruction.insert(newTable);
}


/**
 * Adds a pair of indexes that bound a part of the bit vector that refer
 * to address of another instruction (given in 
 * startSettingInstructionReference).
 *
 * @param start The start index (included in the boundary).
 * @param end The end index (included in the boundary).
 */
void
InstructionBitVector::addIndexBoundsForReference(IndexBound bounds) {

    assert(currentTable_ != NULL);
    assert(bounds.slotStartIndex() <= bounds.slotEndIndex());
    currentTable_->push_back(bounds);
}


/**
 * Fixes the address of the given instruction.
 *
 * When this is called, all the references to the given instruction are 
 * fixed with the given value of the address.
 *
 * @param instruction The instruction.
 * @param address The address of the instruction.
 * @exception OutOfRange If the given value is too large to fit to a boundary
 *                       that refers to the instruction.
 */
void
InstructionBitVector::fixInstructionAddress(
    const TTAProgram::Instruction& instruction, unsigned int address) {
    instructionAddresses_.insert(
        std::pair<const Instruction*, unsigned int>(&instruction, address));
    if (!MapTools::containsKey(references_, &instruction)) {
        return;
    } else {
        IndexBoundSet& setForInstruction = indexBounds(instruction);
        for (IndexBoundSet::const_iterator iter = setForInstruction.begin();
             iter != setForInstruction.end(); iter++) {
            IndexBoundTable* table = *iter;
            fixBits(*table, address);
        }
    }
}

/**
 * Marks the instruction starting point.
 *
 * The instruction starting point should be marked to the position of
 * the first bit (the smallest index in the vector) of instruction.
 *
 * @param position The position of the starting point.
 */
void
InstructionBitVector::markInstructionStartingPoint(unsigned int position) {
    if (instructionBoundaries_.size() == 0) {
        instructionBoundaries_.push_back(position);
    } else {
        if (instructionBoundaries_.back() < position) {
            instructionBoundaries_.push_back(position);
        } else {
            for (BoundaryTable::iterator iter = instructionBoundaries_.begin();
                 iter != instructionBoundaries_.end(); iter++) {
                if (*iter > position) {
                    instructionBoundaries_.insert(iter, position);
                    return;
                } else if (*iter == position) {
                    return;
                }
            }
            assert(false);
        }
    }
}


/**
 * Returns the number of instructions in the vector.
 *
 * The count is determined by means of the number of instruction boundaries.
 *
 * @return The number of instructions.
 */
unsigned int
InstructionBitVector::instructionCount() const {
    return instructionBoundaries_.size();
}


/**
 * Returns the instruction starting point at the given position.
 *
 * The boundaries are returned in the correct order, that is, if parameter 0 is
 * given, the starting point of the first instruction is returned.
 *
 * @return The instruction starting point at the given position.
 * @exception OutOfRange If the given index is too large.
 */
unsigned int
InstructionBitVector::instructionStartingPoint(unsigned int index) const {
    if (index >= instructionBoundaries_.size()) {
        const string procName = "InstructionBitVector::instructionBoundary";
        throw OutOfRange(__FILE__, __LINE__, procName);
    } else {
        return instructionBoundaries_[index];
    }
}

/**
 * Returns the set of index bounds that refer to the given instruction.
 *
 * @param instruction The instruction.
 */
InstructionBitVector::IndexBoundSet&
InstructionBitVector::indexBounds(
    const TTAProgram::Instruction& instruction) {

    if (!MapTools::containsKey(references_, &instruction)) {
        IndexBoundSet* newSet = new IndexBoundSet;
        std::pair<const Instruction*, IndexBoundSet*> newPair(
            &instruction, newSet);
        references_.insert(newPair);
        return *newSet;
    } else {
        return *MapTools::valueForKey<IndexBoundSet*>(
            references_, &instruction);
    }
}    


/**
 * Adds the IndexBoundTables from the given IndexBoundSet to the given
 * IndexBoundSet.
 *
 * @param from The IndexBoundSet from which the tables are copied.
 * @param to The IndexBoundSet to which the tables are added.
 */
void
InstructionBitVector::addIndexBoundTables(
    const IndexBoundSet& from, 
    IndexBoundSet& to) {

    for (IndexBoundSet::const_iterator iter = from.begin(); 
         iter != from.end(); iter++) {

        const IndexBoundTable* fromTable = *iter;
        IndexBoundTable* newTable = new IndexBoundTable(*fromTable);
        
        for (IndexBoundTable::iterator iter = newTable->begin();
             iter != newTable->end(); iter++) {
            (*iter).incrStartIndex(size());
            (*iter).incrEndIndex(size());
        }

        to.insert(newTable);
    }
}


/**
 * Fixes the sections given in the IndexBoundTable of the bit vector with
 * the given value.
 *
 * @param indexes The table of section being fixed.
 * @param value The value.
 * @exception OutOfRange If the value is too large to fit to the boundary.
 */
void
InstructionBitVector::fixBits(
    const IndexBoundTable& indexes, unsigned int value) {
    int requiredSize = MathTools::requiredBits(value);
    int availableSize = this->availableSize(indexes);
    if (requiredSize > availableSize) {
        string errorMsg = "Unable to fix instruction reference with the "
            "correct instruction address due to insuffient space reserved "
            "for the immediate.";
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }

    int currentBit = 0;
    int stopBit = requiredSize;

    for (IndexBoundTable::const_reverse_iterator iter = indexes.rbegin();
         iter != indexes.rend(); iter++) {

        const IndexBound& iBound = *iter;

        unsigned int startIndex = iBound.slotStartIndex();
        unsigned int endIndex = iBound.slotEndIndex();

        if ((*iter).isLimmEncoded()) {
            // Check limm slice indices and update iteration boundaries.
            // LIMM is written from LSB to MSB. Start from the rightmost bit
            // of *this* limm slot.
            currentBit = iBound.limmRightIndex();
            stopBit = iBound.limmLeftIndex();
            assert((stopBit-currentBit) == (iBound.limmWidth()-1));
        }
        for (unsigned int index = endIndex; index >= startIndex; index--) {
            // Rewrite with new value required bits, zero the higher ones
            if (currentBit <= stopBit) {
                operator[](index) = MathTools::bit(value, currentBit);
            } else {
                operator[](index) = false;
            }
            currentBit++;
        }
    }
}

/**
 * Returns the number of bits available in the given IndexBoundTable.
 *
 * @param indexes The IndexBoundTable.
 * @return The number of bits.
 */
unsigned int
InstructionBitVector::availableSize(const IndexBoundTable& indexes) {

    unsigned int size(0);
    for (IndexBoundTable::const_iterator iter = indexes.begin();
         iter != indexes.end(); iter++) {
        size += (*iter).slotEndIndex() - (*iter).slotStartIndex() + 1;
    }
    return size;
}
