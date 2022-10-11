/*
  Copyright (c) 2002-2019 Tampere University of Technology.

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
 * @file Bin2nProgramImageWriter.cc
 *
 * Implementation of Bin2nProgramImageWriter class.
 *
 * @author Topi Leppanen 2019 (topi.leppanen-no.spam-tuni.fi)
 * @note rating: red
 */

#include <string>

#include "Bin2nProgramImageWriter.hh"
#include "InstructionBitVector.hh"
#include "Application.hh"

/**
 * The constructor.
 *
 * @param bits The program bits to be written.
 */
Bin2nProgramImageWriter::Bin2nProgramImageWriter(const InstructionBitVector& bits) :
    Bin2nImageWriter(bits, 0) {
}

/**
 * The destructor.
 */
Bin2nProgramImageWriter::~Bin2nProgramImageWriter(){
}

/**
 * Writes the program image to the given output stream.
 *
 * @param stream The output stream.
 */
void
Bin2nProgramImageWriter::writeImage(std::ostream& stream) const{

    const BitVector& bits = this->bits();
    const InstructionBitVector* programBits =
        dynamic_cast<const InstructionBitVector*>(&bits);

    unsigned int previousBoundary(0);

    // Loop through all insctruction boundaries (missing last instruction)
    for (unsigned int i = 1; i < programBits->instructionCount(); i++) {

        unsigned int instructionBoundary =
	    programBits->instructionStartingPoint(i);
        writeSequence(stream, instructionBoundary - previousBoundary);
        previousBoundary = instructionBoundary;

    }

    // Write last instruction from remaining bits
    writeSequence(stream, bits.size() - previousBoundary);

}
