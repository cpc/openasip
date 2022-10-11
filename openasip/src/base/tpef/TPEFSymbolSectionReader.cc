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
 * @file TPEFSymbolSectionReader.cc
 *
 * Definition of TPEFSymbolSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "TPEFSymbolSectionReader.hh"
#include "SafePointer.hh"
#include "Exception.hh"
#include "ReferenceKey.hh"
#include "SectionReader.hh"
#include "TPEFBaseType.hh"
#include "SymbolElement.hh"
#include "SymbolSection.hh"
#include "NoTypeSymElement.hh"
#include "CodeSymElement.hh"
#include "DataSymElement.hh"
#include "ProcedSymElement.hh"
#include "FileSymElement.hh"
#include "SectionSymElement.hh"
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

using ReferenceManager::SafePointer;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionKey;

typedef SymbolElement::SymbolType SymbolType;
typedef SymbolElement::SymbolBinding SymbolBinding;

const Byte TPEFSymbolSectionReader::SYMBOL_TYPE_MASK = 0x0F;

TPEFSymbolSectionReader TPEFSymbolSectionReader::proto_;

/**
 * Constructor.
 *
 * Registers itself to SectionReader.
 */
TPEFSymbolSectionReader::TPEFSymbolSectionReader() : TPEFSectionReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Destructor.
 */
TPEFSymbolSectionReader::~TPEFSymbolSectionReader() {
}

/**
 * Returns the type of section it is meant to read.
 *
 * @return The type of section it can read.
 */
Section::SectionType
TPEFSymbolSectionReader::type() const {
    return Section::ST_SYMTAB;
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
TPEFSymbolSectionReader::readData(
    BinaryStream& stream, Section* section) const {
    // base classes implementation must be called with these.
    TPEFSectionReader::readData(stream, section);

    SymbolSection* symbolSection =
        dynamic_cast<SymbolSection*>(section);
    assert(symbolSection != NULL);

    // create indexs' starting from 0 undefined element is checked
    SectionIndex index = 0;
    bool undefSymbolDefined = false;

    // check that link section is defined properly
    assert(header().linkId != 0);

    if (!section->noBits()) {
        // start of first element
        SectionOffset elementStart = header().bodyOffset;

        while (elementStart + header().elementSize <=
               header().bodyOffset + header().bodyLength) {

            SymbolElement *elem = NULL;

            Word nameOffset = stream.readWord();
       	    Word value = stream.readWord();
            Word size = stream.readWord();
            Byte info = stream.readByte();
            Byte otherField = stream.readByte();

            SectionId sectionToBelong = stream.readHalfWord();

            // lower half byte is type of symbol
            SymbolType typeOfSym =
                static_cast<SymbolType>(info & SYMBOL_TYPE_MASK);

            // upper half byte is binding
            SymbolBinding binding =
                static_cast<SymbolBinding>(info >> (BYTE_BITWIDTH / 2));

            // symbol of right type
            elem = createSymbol(typeOfSym, value, size, sectionToBelong);

            elem->setAbsolute(otherField & TPEFHeaders::STO_ABS);
            elem->setBinding(binding);

            SectionOffsetKey sOffKey(header().linkId, nameOffset);
            elem->setName(CREATE_SAFEPOINTER(sOffKey));

            // section which to symbol belongs
            SectionKey sKey(sectionToBelong);
            elem->setSection(CREATE_SAFEPOINTER(sKey));

            SectionIndexKey sectionIndexKey(header().sectionId, index);
            SafePointer::addObjectReference(sectionIndexKey, elem);


            // check undefined symbol
            if (index == 0) {
                assert(elem->type() == SymbolElement::STT_NOTYPE);
                assert(elem->binding() == SymbolElement::STB_LOCAL);
                assert(elem->absolute() == true);
                assert(nameOffset == 0);
                undefSymbolDefined = true;
            }

            section->addElement(elem);

            elementStart += header().elementSize;
            stream.setReadPosition(elementStart);
            index++;
        }
    }

    // create undefined symbol if not found from table
    // (e.g. if section had nobits flag set)
    if (!undefSymbolDefined) {
        NoTypeSymElement* elem = new NoTypeSymElement();
        elem->setBinding(SymbolElement::STB_LOCAL);
        elem->setAbsolute(true);
        SectionOffsetKey sOffKey(header().linkId, 0);
        elem->setName(CREATE_SAFEPOINTER(sOffKey));
        SectionIndexKey sectionIndexKey(header().sectionId, 0);
        SafePointer::addObjectReference(sectionIndexKey, elem);
        section->addElement(elem);
    }
}

/**
 * Creates symbol element.
 *
 * @param symType Type of symbol to create.
 * @param aValue Value of element.
 * @param aSize Size of element.
 * @param sectToBelong Identification code of the section that contains the
 *        element.
 * @return Newly created symbol.
 */
SymbolElement*
TPEFSymbolSectionReader::createSymbol(
    SymbolElement::SymbolType symType,
    Word aValue,
    Word aSize,
    SectionId sectToBelong) const {

    SymbolElement *elem = NULL;

    // NOTE: check symbols from latest spec and
    // add whole element reading stuff here.
    switch (symType) {
    case SymbolElement::STT_NOTYPE:
        elem = new NoTypeSymElement();
        break;

    case SymbolElement::STT_PROCEDURE:
        elem = new ProcedSymElement();
        /* fall through */

    case SymbolElement::STT_CODE:
        if (elem == NULL) {
            elem = new CodeSymElement();
        }

        if (sectToBelong != 0) {
            SectionOffsetKey sOffKey(sectToBelong, aValue);
            dynamic_cast<CodeSymElement*>
                (elem)->setReference(CREATE_SAFEPOINTER(sOffKey));
            dynamic_cast<CodeSymElement*>
                (elem)->setSize(aSize);
        }

        break;

    case SymbolElement::STT_DATA:
        elem = new DataSymElement();

        if (sectToBelong != 0) {
            SectionOffsetKey sOffKey(sectToBelong, aValue);
            dynamic_cast<DataSymElement*>
                (elem)->setReference(CREATE_SAFEPOINTER(sOffKey));

            dynamic_cast<DataSymElement*>
                (elem)->setSize(aSize);
        }

        break;

    case SymbolElement::STT_SECTION:
        elem = new SectionSymElement();
        dynamic_cast<SectionSymElement*>(elem)->setValue(aValue);
        dynamic_cast<SectionSymElement*>(elem)->setSize(aSize);
        break;

    case SymbolElement::STT_FILE:
        elem = new FileSymElement();
        dynamic_cast<FileSymElement*>(elem)->setValue(aValue);
        break;

    default:
        ;
    }

    return elem;
}

}
