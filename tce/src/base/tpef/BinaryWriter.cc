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
