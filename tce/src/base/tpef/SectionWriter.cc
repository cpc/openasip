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
    const BinaryWriter* writer) {

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
    const BinaryWriter* writer) {

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
