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
 * @file AOutDataSectionReader.cc
 *
 * Definition of AOutDataSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#include "AOutDataSectionReader.hh"
#include "DataSection.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "AOutSymbolSectionReader.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;

AOutDataSectionReader AOutDataSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
AOutDataSectionReader::AOutDataSectionReader() : AOutSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
AOutDataSectionReader::~AOutDataSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
AOutDataSectionReader::type() const {
    return Section::ST_DATA;
}

/**
 * Reads data section of a.out binary file.
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
AOutDataSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    AOutReader* bReader = dynamic_cast<AOutReader*>(parent());
    assert(bReader != NULL);

    DataSection* theDataSection = dynamic_cast<DataSection*>(section);
    assert(theDataSection != NULL);

    for (SectionOffset i = 0;
         i < bReader->header().sectionSizeData(); i++) {
        theDataSection->addByte(stream.readByte());
    }
}

}
