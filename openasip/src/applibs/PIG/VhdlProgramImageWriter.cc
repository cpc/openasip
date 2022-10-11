/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file VhdlProgramImageWriter.cc
 *
 * Implementation of VhdlProgramImageWriter class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "InstructionBitVector.hh"
#include "ArrayProgramImageWriter.hh"
#include "VhdlProgramImageWriter.hh"
using std::endl;

/**
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
VhdlProgramImageWriter::VhdlProgramImageWriter(
    const InstructionBitVector& bits, const std::string& entityName):
    ArrayProgramImageWriter(bits), entityName_(entityName) {
}

/**
 * The destructor.
 */
VhdlProgramImageWriter::~VhdlProgramImageWriter() {
}


/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void VhdlProgramImageWriter::writeImage(std::ostream& stream) const {
    writeHeader(stream);
    ArrayProgramImageWriter::writeImage(stream);
    writeEnding(stream);
}

/**
 * Writes the vhdl declaration stuff to the beginning of the stream
 */
void VhdlProgramImageWriter::writeHeader(std::ostream& stream) const {
    stream << "library ieee;" << endl
           << "use ieee.std_logic_1164.all;" << endl
           << "use ieee.std_logic_arith.all;" << endl
           << "use work.";
    if (!entityName_.empty()) {
        stream << entityName_ << "_";
    }
    stream << "imem_mau.all;" << endl << endl;

    stream << "package " << packageName() << " is" << endl << endl
           << "  type std_logic_imem_matrix is array (natural range <>) of "
           << "std_logic_vector(IMEMMAUWIDTH-1 downto 0);" << endl << endl;

    stream << "  constant imem_array : std_logic_imem_matrix := (" << endl;
}

/**
 * Writes the end declarations to the stream
 */
void VhdlProgramImageWriter::writeEnding(std::ostream& stream) const {
    stream << ");" << endl << endl
           << "end " << packageName() << ";" << endl;
}

std::string
VhdlProgramImageWriter::packageName() const {
    
    std::string package = "imem_image";
    if (!entityName_.empty()) {
        package = entityName_ + "_" + package;
    }
    return package;
}
