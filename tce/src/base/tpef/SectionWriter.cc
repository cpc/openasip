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
 * @file SectionWriter.cc
 *
 * Definition of SectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionWriter.hh"
#include "BinaryWriter.hh"

namespace TPEF {

SectionWriter::MapType* SectionWriter::prototypes_ = NULL;

/**
 * Constructor
 */
SectionWriter::SectionWriter() {
}

/**
 * Destructor
 */
SectionWriter::~SectionWriter() {
}

/**
 * Finds correct SectionWriter instance and use it to write section header.
 *
 * @param stream Stream where to write.
 * @param sect Section which header will be written.
 * @param writer BinaryWriter which filetype we want to write.
 * @exception InstanceNotFound If there wasn't found any writer for section.
 */
void
SectionWriter::writeHeader(
    BinaryStream& stream,
    const Section* sect,
    const BinaryWriter* writer)
    throw (InstanceNotFound) {

    findSectionWriter(sect->type(), writer)->actualWriteHeader(stream, sect);
}

/**
 * Finds correct SectionWriter instance and command it to write section data.
 *
 * @param stream Stream where to write.
 * @param sect Section which data will be written.
 * @param writer BinaryWriter which filetype we want to write.
 * @exception InstanceNotFound If there wasn't found any writer for section.
 */
void
SectionWriter::writeData(
    BinaryStream& stream,
    const Section* sect,
    const BinaryWriter* writer)
    throw (InstanceNotFound) {

    findSectionWriter(sect->type(), writer)->actualWriteData(stream, sect);
}

/**
 * Writes those values to stream, which can't be written before all objects
 * are written down.
 *
 * This is used for example when some section writer has to update data of
 * other section or sections.
 *
 * @param stream Stream that contains binary to finalize.
 * @param Binary Binary file which needs finalization.
 * @param writer Writer, which was used for writing binary.
 */
void
SectionWriter::finalizeBinary(
    BinaryStream& stream,
    const Binary* bin,
    const BinaryWriter* writer) {

    for (Word i = 0; i < bin->sectionCount(); i++) {
        // get section and finalize it
        Section* sect =bin->section(i);

        try {
            const SectionWriter* sectionWriter =
                findSectionWriter(sect->type(), writer);
            sectionWriter->finalize(stream, sect);

        } catch (const InstanceNotFound &e) {
            // there is not always reader for every created section
            // and it's ok.
        }
    }
}

/**
 * Finds SectionWriter instance by SectionType and BinaryWriter*.
 *
 * @param type Type of section to find.
 * @param bWriter BinaryWriter which requested finding section.
 * @return Instance which can write section.
 * @exception InstanceNotFound Writer instance was not found.
 */
const SectionWriter*
SectionWriter::findSectionWriter(
    const Section::SectionType type,
    const BinaryWriter* bWriter)
    throw (InstanceNotFound) {

    MapKey key(type, bWriter);

    if (prototypes_ == NULL ||
        !MapTools::containsKey(*prototypes_, key)) {
        throw InstanceNotFound(__FILE__, __LINE__,
                               "SectionWriter::findSectionWriter");
    }

    return (*prototypes_)[key];
}

/**
 * Registers SectionWriter instance for writing specific section type.
 *
 * @param sWriter Instance to register for writing.
 */
void
SectionWriter::registerSectionWriter(const SectionWriter* sWriter) {

    MapKey key(sWriter->type(), &sWriter->parent());

    // We can't create prototypes_ map statically, because we don't know
    // if it is initialized before this method is called.
    if (prototypes_ == NULL) {
        prototypes_ = new MapType();
    }

    assert(!MapTools::containsKey(*prototypes_, key));

    (*prototypes_)[key] = sWriter;

}

/**
 * Generates a unique section identification code.
 *
 * If several files are written out, the section identification codes of
 * each file will be starting from a different numeric code. Never returns
 * the reserved identification code for null/invalid section.
 */
SectionId
SectionWriter::getUniqueSectionId() {

    // counter used to generate unique identification codes
    static SectionId counter = 0;

    counter++;
    // skip zero (reserved identification code) if the counter wraps around
    if (counter == 0) counter++;
    return counter;
}

/**
 * Default finalizer method.
 *
 * This is used, if section does not have its own method defined.
 *
 * @param stream Stream that contains binary to finalize.
 * @param section Section to finalize.
 */
void
SectionWriter::finalize(
    BinaryStream&, Section*) const {
}

}
