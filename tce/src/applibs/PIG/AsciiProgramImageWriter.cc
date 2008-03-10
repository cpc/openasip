/**
 * @file AsciiProgramImageWriter.cc
 *
 * Implementation of AsciiProgramImageWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "AsciiProgramImageWriter.hh"
#include "InstructionBitVector.hh"
#include "Application.hh"

/**
 * The constructor.
 *
 * @param bits The program bits to be written.
 */
AsciiProgramImageWriter::AsciiProgramImageWriter(
    const InstructionBitVector& bits) : AsciiImageWriter(bits, 0) {
}


/**
 * The destructor.
 */
AsciiProgramImageWriter::~AsciiProgramImageWriter() {
}


/**
 * Writes the program image to the given output stream.
 *
 * @param stream The output stream.
 */
void
AsciiProgramImageWriter::writeImage(std::ostream& stream) const {

    const BitVector& bits = this->bits();
    const InstructionBitVector* programBits = 
        dynamic_cast<const InstructionBitVector*>(&bits);
    assert(programBits != NULL);
    
    unsigned int previousBoundary(0);
    for (unsigned int i = 1; i < programBits->instructionCount(); i++) {
        unsigned int instructionBoundary = 
            programBits->instructionStartingPoint(i);
        writeSequence(stream, instructionBoundary - previousBoundary);
        stream << std::endl;
        previousBoundary = instructionBoundary;
    }
    writeSequence(stream, bits.size() - previousBoundary);
    stream << std::endl;
}


