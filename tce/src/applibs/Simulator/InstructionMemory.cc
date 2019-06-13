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
 * @file InstructionMemory.cc
 *
 * Definition of InstructionMemory class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "SequenceTools.hh"
#include "Application.hh"
#include "Conversion.hh"

/**
 * Constructor.
 *
 * This constructor is for constant width instructions only.
 *
 * @param startAddress The starting address of the instruction memory.
 */
InstructionMemory::InstructionMemory(
    InstructionAddress startAddress) : 
    startAddress_(startAddress) {
}

/**
 * Destructor.
 */
InstructionMemory::~InstructionMemory() {

    for (InstructionContainer::const_iterator i = instructions_.begin();
         i != instructions_.end();  ++i) {
        delete (*i);
    }   
    instructions_.clear();
}

/**
 * Adds new ExecutableInstruction to memory.
 *
 * @param instruction Instruction to be added.
 */
void
InstructionMemory::addExecutableInstruction(
    ExecutableInstruction* instruction) {

    instructions_.push_back(instruction);
}

/**
 * Resets execution counters of all instructions.
 *
 */
void
InstructionMemory::resetExecutionCounts() {
    for (InstructionContainer::iterator i = instructions_.begin();
         i != instructions_.end(); ++i) {
        (*i)->resetExecutionCounts();
    }
}
