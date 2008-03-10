/**
 * @file AsciiProgramImageWriter.hh
 *
 * Declaration of AsciiProgramImageWriter.hh
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ASCII_PROGRAM_IMAGE_WRITER_HH
#define TTA_ASCII_PROGRAM_IMAGE_WRITER_HH

#include "AsciiImageWriter.hh"

class InstructionBitVector;

/**
 * This class writes ASCII program images.
 */
class AsciiProgramImageWriter : public AsciiImageWriter {
public:
    AsciiProgramImageWriter(const InstructionBitVector& bits);
    virtual ~AsciiProgramImageWriter();

    virtual void writeImage(std::ostream& stream) const;
};

#endif
