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
 * @file TPEFDataSectionReader.cc
 *
 * Definition of TPEFDataSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFDataSectionReader.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "SafePointer.hh"

#include "DataSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::FileOffsetKey;
using std::string;

TPEFDataSectionReader TPEFDataSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFDataSectionReader::TPEFDataSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFDataSectionReader::~TPEFDataSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFDataSectionReader::type() const {
    return Section::ST_DATA;
}

/**
 * Reads data section of TPEF binary file.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFDataSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    DataSection* dataSection = dynamic_cast<DataSection*>(section);
    assert(dataSection != NULL);

    // check that link section is defined properly
    assert(header().linkId == 0);

    if (!section->noBits()) {
        while (stream.readPosition() <
               header().bodyOffset + header().bodyLength) {
            dataSection->addByte(stream.readByte());
        }

        dynamic_cast<TPEFReader*>(
            parent())->addSectionSize(section, header().bodyLength);
    }
}

}
