/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file VhdlImageWriter.cc
 *
 * Implementation of VhdlImageWriter class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "InstructionBitVector.hh"
#include "ArrayImageWriter.hh"
#include "VhdlImageWriter.hh"
using std::endl;

/**
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
VhdlImageWriter::VhdlImageWriter(
    const BitVector& bits, int rowLength) :
    ArrayImageWriter(bits, rowLength), dataWidth_(rowLength) {
}

/**
 * The destructor.
 */
VhdlImageWriter::~VhdlImageWriter() {
}

/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void VhdlImageWriter::writeImage(std::ostream& stream) const {
    writeHeader(stream);
    ArrayImageWriter::writeImage(stream);
    writeEnding(stream);
}


/**
 * Writes the vhdl declaration stuff to the beginning of the stream
 */
void VhdlImageWriter::writeHeader(std::ostream& stream) const {
    stream << "library ieee;" << endl
           << "use ieee.std_logic_1164.all;" << endl
           << "use ieee.std_logic_arith.all;" << endl << endl;

    stream << "package dmem_image is" << endl << endl
           << "  type std_logic_dmem_matrix is array (natural range <>) of "
           << "std_logic_vector(" << dataWidth_ << "-1 downto 0);" 
           << endl << endl;

    stream << "  constant dmem_array : std_logic_dmem_matrix := (" << endl;
}

/**
 * Writes the end declarations to the stream
 */
void VhdlImageWriter::writeEnding(std::ostream& stream) const {
    stream << ");" << endl << endl
           << "end dmem_image;" << endl;
}
