/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SectionReader.cc
 *
 * Definition of SectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionReader.hh"

#include <map>
#include <utility>

#include "BinaryStream.hh"
#include "Section.hh"
#include "Binary.hh"
#include "ContainerTools.hh"
#include "AssocTools.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

using std::map;
using std::pair;

SectionReader::MapType* SectionReader::prototypes_ = NULL;

/**
 * Constructor
 */
SectionReader::SectionReader() {
}

/**
 * Destructor
 */
SectionReader::~SectionReader() {
}

/**
 * Reads a section from BinaryStream.
 *
 * Finds correct concrete SectionReader object for reading section and
 * uses it for reading.
 *
 * @param stream Stream from which section's data is read.
 * @param section Section instance where section elements are stored.
 * @param reader BinaryReader* of file type that we try to read.
 * @exception InstanceNotFound There is no prototype registred for reading.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read values were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
SectionReader::readSection(
    BinaryStream& stream, Section* section, BinaryReader* reader) {
    const SectionReader* sectionReader =
        findSectionReader(section->type(), reader);

    sectionReader->readData(stream, section);
}

/**
 * Finalizes binary whose all data is read and resolved.
 *
 * @param bin Binary to resolve.
 * @param reader Reader which was used for reading.
 */
void
SectionReader::finalizeBinary(Binary* bin, BinaryReader* reader) {

    for (Word i = 0; i < bin->sectionCount(); i++) {
        // get section and finalize it
        Section* sect = bin->section(i);

        try {
            const SectionReader* sectionReader =
                findSectionReader(sect->type(), reader);
            sectionReader->finalize(sect);

        } catch (const InstanceNotFound &e) {
            // there is not always reader for every created section
            // and it's ok.
        }
    }
}

/**
 * Finds SectionReader instance by SectionType and BinaryReader*.
 *
 * @param type Type of section to find.
 * @param reader BinaryReader which requested finding section.
 * @return Instance which can read section.
 * @exception InstanceNotFound Reader instance was not found.
*/
const SectionReader*
SectionReader::findSectionReader(
    const Section::SectionType type, const BinaryReader* reader) {
    MapKey key(type, reader);

    if (prototypes_ == NULL ||
        !MapTools::containsKey(*prototypes_, key)) {
        throw InstanceNotFound(__FILE__, __LINE__,
                               "SectionReader::findSectionReader");
    }

    return (*prototypes_)[key];
}

/**
 * Registers SectionReader instance for reading specific section type.
 *
 * @param reader Instance to register for reading.
 */
void
SectionReader::registerSectionReader(const SectionReader* reader) {

    MapKey key(reader->type(), reader->parent());

    // We can't define a static prototypes_ map, because we cannot guarantee
    // that it is initialized before this method is called.
    if (prototypes_ == NULL) {
        prototypes_ = new MapType();
    }

    assert(!MapTools::containsKey(*prototypes_, key));

    (*prototypes_)[key] = reader;
}

/**
 * Default finalizer method all sections.
 *
 * Does nothing. This method is runned for every read section after body of
 * every section is read and references are resolved once.
 *
 * @param section Section to finalize.
 */
void
SectionReader::finalize(Section* /*section*/) const {
}

}
