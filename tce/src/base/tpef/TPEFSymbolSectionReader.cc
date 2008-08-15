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
 * @file TPEFSymbolSectionReader.cc
 *
 * Definition of TPEFSymbolSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
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
    BinaryStream& stream,
    Section* section) const
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

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
