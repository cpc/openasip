/**
 * @file MifImageWriter.cc
 *
 * Implementation of MifImageWriter class.
 *
 * @author Otto Esko 2009 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
#include <cmath>

#include "MifImageWriter.hh"
#include "BitVector.hh"

using std::string;
using std::endl;

const string COMMENT = "-- Memory initialization file";
const string WIDTH = "WIDTH = ";
const string DEPTH = "DEPTH = ";
const string A_RADIX = "ADDRESS_RADIX = DEC;";
const string D_RADIX = "DATA_RADIX = BIN;";
const string BEGIN = "CONTENT BEGIN";
const string END = "END;";
const string INDENT = "   ";

/**
 * The constructor.
 *
 * @param bits The bits to be written.
 * @param rowLength The length of the row in the output stream.
 */
// MifImageWriter::MifImageWriter(const BitVector& bits, int rowLength):
//     bits_(bits), rowLength_(rowLength) {
// }
MifImageWriter::MifImageWriter(const BitVector& bits, int rowLength):
    AsciiImageWriter(bits,rowLength) {
}

/**
 * The destructor.
 */
MifImageWriter::~MifImageWriter() {
}

/**
 * Writes the bits to the given stream.
 *
 * @param stream The output stream.
 */
void MifImageWriter::writeImage(std::ostream& stream) const {
    int wordCount = 
        static_cast<int>(ceil((float)bits().size() / rowLength()));
    writeHeader(stream);

    int address = 0;
    if (wordCount == 0) {
        // empty MIF is invalid, fill one row with zeroes
        stream << address << INDENT << ":" << INDENT;
        for (int i = 0; i < rowLength(); i++) {
            stream << "0";
        }
        stream << ";" << endl;
    } else {
        bool padEndings = false;
        for (int i = 0; i < wordCount-1; i++) {
            stream << address << INDENT << ":" << INDENT;
            writeSequence(stream, rowLength(), padEndings);
            stream << ";" << endl;
            address++;
        }
        // last line might need to be padded
        padEndings = true;
        stream << address << INDENT << ":" << INDENT;
        writeSequence(stream, rowLength(), padEndings);
        stream << ";" << endl;
    }
    stream << END << endl;
}

/**
 * Writes the MIF header to the given stream.
 *
 * @param stream The output stream.
 */
void MifImageWriter::writeHeader(std::ostream& stream) const {
    int wordCount = static_cast<int>(ceil(bits().size() / rowLength()));
    stream << COMMENT << endl
           << WIDTH << rowLength() << ";" << endl;

    // empty MIF is invalid
    if (wordCount == 0) {
        stream << DEPTH << "1" << ";" << endl;
    } else {
        stream << DEPTH << wordCount << ";" << endl;
    }

    stream << A_RADIX << endl
           << D_RADIX << endl << endl
           << BEGIN << endl;
}
