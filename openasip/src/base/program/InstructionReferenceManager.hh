/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file InstructionReferenceManager.hh
 *
 * Declaration of InstructionReferenceManager class.
 *
 * The instruction reference handles references to instructions.
 * 
 * The design consists of 3 classes:
 *
 * InstructionReferenceManager: a single high-level object. Contains the
 * external interface, maps to locate a reference to an instruction
 * based on instruction, and ways to iterate over all instruction references.
 *
 * InstructionReference: The class which is shown to a client. Encapsulates
 * a reference into a single instruction. Multiple InstructionReferences
 * may point to a single instruction and these can be freely copied etc.
 * You should not use pointers to these classes, but always copy your 
 * existing objects with copy constructor/assignment into the new one when
 * returning from functions etc.
 *
 * InstructionReferenceImpl: This is internal intermediate implementation 
 * class. It is not shown to the user. There is always exactly one
 * InstructionReferenceImpl per target instruction. 
 * If two instruction references that originally pointed to different 
 * instructions (and so had different InstructionReferenceImpl objects), 
 * these objects are automatically merged and all the internal pointers 
 * and bookkeeping updated accordingly. 
 *
 * Multiple instructionreferences typically point to InstructionReferenceImpl,
 * and instructionReferenceImpl contains pointers to all of these, so it
 * can notify them in case the instructionReferenceImpl is merged with
 * another.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_REFERENCE_MANAGER_HH
#define TTA_INSTRUCTION_REFERENCE_MANAGER_HH

#include <map>
#include "Exception.hh"
#include "InstructionReferenceImpl.hh"

namespace TTAProgram {

class Instruction;
class InstructionReference;
class InstructionReferenceImpl;

/**
 * Helps in keeping instructions referenced in POM up-to-date.
 *
 * Instructions are not referenced directly, but through an
 * InstructionReference instance. The actual Instruction instance
 * referred can be changed as needed.
 */
class InstructionReferenceManager {
public:
    /// Map for instruction references. faster to search than list.
    typedef std::map<Instruction*, InstructionReferenceImpl*> RefMap;

    InstructionReferenceManager();
    virtual ~InstructionReferenceManager();

    InstructionReference createReference(Instruction& ins);
    void replace(Instruction& insA, Instruction& insB);
    void clearReferences();
    bool hasReference(Instruction& ins) const;
    unsigned int referenceCount(Instruction& ins) const;
    void referenceDied(Instruction* ins);

    void validate();

    class Iterator {
    public:
        inline Iterator& operator++(); // ++i
        inline const InstructionReference& operator*() const;
        inline const InstructionReference* operator->() const;
        inline bool operator !=(const Iterator& i) const;
        inline Iterator(RefMap::iterator iter);
        inline Iterator(RefMap::iterator& iter);
    private:
        RefMap::iterator iter_;
    };
        
    inline Iterator begin();
    inline Iterator end();

private:
    // disable copying and assignment.
    InstructionReferenceManager(const InstructionReferenceManager&);
    InstructionReferenceManager& operator=(const InstructionReferenceManager&);

    /// Instruction references to maintain.
    RefMap references_;

};

#include "InstructionReferenceManager.icc"

}


#endif
