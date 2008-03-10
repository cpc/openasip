/**
 * @file ArrayProgramImageWriter.cc
 *
 * Implementation of ArrayProgramImageWriter class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
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

    
