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
 * @file ArrayProgramImageWriter.cc
 *
 * Implementation of ArrayProgramImageWriter class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ArrayProgramImageWriter.hh"
#include "InstructionBitVector.hh"
#include "Application.hh"

/**
 * The constructor.
 *
 * @param bits The program image to write.
 */
ArrayProgramImageWriter::ArrayProgramImageWriter(
    const InstructionBitVector& bits) : ArrayImageWriter(bits, 0) {
}


/**
 * The destructor.
 */
ArrayProgramImageWriter::~ArrayProgramImageWriter() {
}


/**
 * Writes the program image to the given output stream.
 *
 * @param stream The output stream.
 */
void
ArrayProgramImageWriter::writeImage(std::ostream& stream) const {

    const BitVector& bits = this->bits();
    const InstructionBitVector* programBits = 
        dynamic_cast<const InstructionBitVector*>(&bits);
    assert(programBits != NULL);

    unsigned int previousBoundary(0);
    for (unsigned int i = 1; i < programBits->instructionCount(); i++) {
        unsigned int instructionBoundary = 
            programBits->instructionStartingPoint(i);
        stream << "\"";
        writeSequence(stream, instructionBoundary - previousBoundary);
        stream << "\"," << std::endl;
        previousBoundary = instructionBoundary;
    }
    stream << "\"";
    writeSequence(stream, bits.size() - previousBoundary);
    stream << "\"";
}

    
