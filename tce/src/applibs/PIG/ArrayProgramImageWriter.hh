/**
 * @file ArrayProgramImageWriter.hh
 *
 * Declaration of ArrayProgramImageWriter class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ARRAY_PROGRAM_IMAGE_WRITER_HH
#define TTA_ARRAY_PROGRAM_IMAGE_WRITER_HH

#include "ArrayImageWriter.hh"

class InstructionBitVector;

/**
 * A program image writer that writes the program as an array. It can be
 * copied to a VHDL file to initialize an array, for example.
 */
class ArrayProgramImageWriter : public ArrayImageWriter {
public:
    ArrayProgramImageWriter(const InstructionBitVector& bits);
    virtual ~ArrayProgramImageWriter();

    virtual void writeImage(std::ostream& stream) const;
};

#endif
