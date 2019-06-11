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
 * @file TPEFLineNumSectionReader.cc
 *
 * Definition of TPEFLineNumSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFLineNumSectionReader.hh"
#include "LineNumSection.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "LineNumProcedure.hh"
#include "BinaryStream.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionKey;

TPEFLineNumSectionReader TPEFLineNumSectionReader::proto_;
SectionId TPEFLineNumSectionReader::codeSectionId_ = 0;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFLineNumSectionReader::TPEFLineNumSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFLineNumSectionReader::~TPEFLineNumSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFLineNumSectionReader::type() const {
    return Section::ST_LINENO;
}

/**
 * Reads section data from TPEF binary file.
 *
 * @param stream Stream to be read from.
 * @param section Section where the information is to be stored.
 * @exception UnreachableStream If reading of section fails.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some of read value were out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
void
TPEFLineNumSectionReader::readData(
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    LineNumSection* lineNumSection = dynamic_cast<LineNumSection*>(section);
    assert(lineNumSection != NULL);

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {
        // mark start of first element
        SectionOffset elementStart = header().bodyOffset;

        LineNumProcedure *procedure = NULL;

        while (elementStart + header().elementSize <=
               header().bodyOffset + header().bodyLength) {

            Word elementOffset = stream.readWord();
            HalfWord lineNum = stream.readHalfWord();

            // Let's create new procedure for our line numbers if needed.
            if (lineNum == 0) {
                procedure = new LineNumProcedure();

                SectionIndexKey indexKey(header().linkId, elementOffset);

                if (elementOffset != 0) {
                    procedure->setProcedureSymbol(
                        CREATE_SAFEPOINTER(indexKey));
                }

                section->addElement(procedure);

            } else if (procedure != NULL) {
                LineNumElement *elem = new LineNumElement();

                // where is section id off data section stored
                SectionOffsetKey instrKey(codeSectionId_, elementOffset);
                elem->setInstruction(CREATE_SAFEPOINTER(instrKey));
                elem->setLineNumber(lineNum);

                // add line to last created procedure
                procedure->addLine(elem);

            } else {
                bool sectionShouldNotHaveLineNumbersBeforeProcedure = false;
                assert(sectionShouldNotHaveLineNumbersBeforeProcedure);
            }

            elementStart += header().elementSize;
            stream.setReadPosition(elementStart);
        }
    }
}

/**
 * Reads info field of section header, which contains the identification
 * code of the code section to which the line number section applies.
 *
 * Read position of stream will be moved 4 bytes forward.
 *
 * @param stream Stream where from info word is read.
 * @param sect Section whose info field is interpreted.
 */
void
TPEFLineNumSectionReader::readInfo(BinaryStream& stream,
                                   Section* sect) const {

    // identification code of referenced section is in first 2 bytes of info
    // field
    codeSectionId_ = stream.readHalfWord();

    if (codeSectionId_ != 0) {
        SectionKey sKey(codeSectionId_);
        dynamic_cast<LineNumSection*>(sect)->setCodeSection(
            CREATE_SAFEPOINTER(sKey));
    } else {
        dynamic_cast<LineNumSection*>(sect)->setCodeSection(
            &SafePointer::null);
    }

    // skip rest 2 of 4 bytes
    stream.readHalfWord();
}

}
