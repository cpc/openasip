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
 * @file BinaryReader.cc
 *
 * Non-inline definitions of BinaryReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "BinaryReader.hh"

#include <set>

#include "Application.hh"
#include "TPEFBaseType.hh"
#include "BinaryStream.hh"
#include "Binary.hh"
#include "SafePointer.hh"
#include "SectionReader.hh"
#include "Exception.hh"

namespace TPEF {

using std::set;
using std::string;
using ReferenceManager::SafePointer;

// initializes an instance of static member
set<BinaryReader*>* BinaryReader::prototypes_ = NULL;

/**
 * Constructor.
 */
BinaryReader::BinaryReader() {
}

/**
 * Destructor.
 */
BinaryReader::~BinaryReader() {
}

/**
 * Constructs a Binary object by reading data from the given binary stream.
 *
 * Looks for the concrete binary reader class that can read the given binary
 * stream. If found, that reader is used to read and construct a Binary
 * object. This method cleans keytables of reference manager so after running
 * this method reference manager contain keys of just read binary file.
 *
 * @param stream Stream to read binarys data from.
 * @return Binary that was created from the given stream.
 * @exception InstanceNotFound If instance for reading wasn't found.
 * @exception UnreachableStream If given stream can't be read.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some read value was out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 * @exception UnresolvedReference If there was unresolved references during
 * reading.
 */
Binary*
BinaryReader::readBinary(BinaryStream& stream)
    throw (Exception) {

    if (prototypes_ != NULL) {

        unsigned long startPos = stream.readPosition();

        set<BinaryReader*>::iterator readers = prototypes_->begin();

        // checks if any BinaryReader registered can read this stream
        while (readers != prototypes_->end()) {
            // isMyStreamType should not move stream position.
            assert(stream.readPosition() == startPos);

            if ((*readers)->isMyStreamType(stream)) {

	        // isMyStreamtype must leave stream as it was
                assert(stream.readPosition() == startPos);

                SafePointer::cleanupKeyTables();

                Binary *readBinary = (*readers)->readData(stream);

                try {
                    SafePointer::resolve();
                } catch (const UnresolvedReference &e) {
                    std::stringstream newErrorMsg;
                    newErrorMsg 
                        << "Error was probably caused by a broken input file."
                        << std::endl 
                        << "Unresolved references during reading: ";

                    UnresolvedReference error(
                        __FILE__, __LINE__, __func__,
                        newErrorMsg.str() + e.errorMessage());

                    error.setCause(e);
                    throw error;
                }

                SectionReader::finalizeBinary(readBinary, (*readers));

                try {
                    SafePointer::resolve();
                } catch (const UnresolvedReference &e) {
                    std::stringstream newErrorMsg;
                    newErrorMsg 
                        << "Error was probably caused by a broken input file."
                        << std::endl 
                        << "Unresolved references during finalization of "
                        << "reading: ";

                    UnresolvedReference error(
                        __FILE__, __LINE__, __func__,
                        newErrorMsg.str() + e.errorMessage());

                    error.setCause(e);
                    throw error;
                }

                // clean up after reading, to avoid abuse of reference manager
                SafePointer::cleanupKeyTables();

                return readBinary;
            }
            readers++;
        }
    }
    throw InstanceNotFound(
            __FILE__, __LINE__, __func__, 
            "Cannot find suitable reader implementation for file.");
}

/**
 * Registers a concrete BinaryReader instance for reading a file type.
 *
 * @param reader Concrete reader to be registred.
 */
void
BinaryReader::registerBinaryReader(BinaryReader* reader) {

    if (prototypes_ == NULL) {
        prototypes_ = new set<BinaryReader*>();
    }

    prototypes_->insert(prototypes_->end(),reader);
}

}
