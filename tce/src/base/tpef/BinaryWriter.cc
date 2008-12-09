/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BinaryWriter.cc
 *
 * Definition of BinaryWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "BinaryWriter.hh"

#include "SectionWriter.hh"
#include "ValueReplacer.hh"
#include "SectionSizeReplacer.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
BinaryWriter::BinaryWriter() {
}

/**
 * Destructor.
 */
BinaryWriter::~BinaryWriter() {
}

/**
 * Stores a binary program representation to given binary stream.
 *
 * @param stream Stream where to write the program representation.
 * @param bin Binary class hierarchy to write.
 */
void
BinaryWriter::writeBinary(BinaryStream& stream, const Binary* bin) const {

    assert(writerToUse_ != NULL);

    // init referencemanager and replacers
    SafePointer::cleanupKeyTables();
    ValueReplacer::initialize(stream);
    SectionSizeReplacer::clear();

    writerToUse_->actualWriteBinary(stream, bin);

    // try catch blocks are just for debug use right now
    try {
        ValueReplacer::finalize();
    } catch(const MissingKeys &e) {
        std::cerr << "Missing keys during writing binary file! : \n"
                  << e.procedureName() << std::endl
                  << e.errorMessage() << std::endl
                  << std::endl;

        assert(false);
    } catch( ... ) {
        std::cerr << "Problems with stream during writing binary file"
                  << std::endl;
        assert(false);
    }

    // finalize was called successfully so init must be run again
    ValueReplacer::initialize(stream);

    SectionWriter::finalizeBinary(stream, bin, writerToUse_);

    // try catch blocks are just for debug use right now
    try {
        ValueReplacer::finalize();
    } catch(const MissingKeys &e) {
        std::cerr << "Missing keys during finalization of writing! \n"
                  << e.procedureName() << std::endl
                  << e.errorMessage() << std::endl
                  << std::endl;
        assert(false);
    } catch( ... ) {
        std::cerr << "Problems with stream during finalization of writing"
                  << std::endl;
        assert(false);
    }

    ReferenceManager::SafePointer::cleanupKeyTables();
}

/**
 * Sets the concrete binary writer to be used for writing binary program
 * representations to a binary stream.
 *
 * @param writer Writer instance that we use for writing binary.
 */
void
BinaryWriter::setWriter(const BinaryWriter* writer) {
    writerToUse_ = writer;
}

}
