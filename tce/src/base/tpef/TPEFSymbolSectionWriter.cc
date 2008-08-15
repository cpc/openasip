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
 * @file TPEFSymbolSectionWriter.cc
 *
 * Definition of TPEFSymbolSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <list>

#include "TPEFSymbolSectionWriter.hh"
#include "SafePointer.hh"
#include "ReferenceKey.hh"
#include "SectionElement.hh"
#include "SectionSizeReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIdReplacer.hh"
#include "NoTypeSymElement.hh"
#include "CodeSymElement.hh"
#include "ProcedSymElement.hh"
#include "DataSymElement.hh"
#include "FileSymElement.hh"
#include "SectionSymElement.hh"

namespace TPEF {

using std::list;
using ReferenceManager::SafePointer;
using ReferenceManager::SectionKey;
using ReferenceManager::FileOffsetKey;
using ReferenceManager::SectionIndexKey;

const TPEFSymbolSectionWriter TPEFSymbolSectionWriter::instance_;
const Word TPEFSymbolSectionWriter::elementSize_ = 16;

/**
 * Constructor.
 *
 * Registers itself to SectionWriter.
 */
TPEFSymbolSectionWriter::TPEFSymbolSectionWriter() : TPEFSectionWriter() {
    registerSectionWriter(this);
}

/**
 * Destructor.
 */
TPEFSymbolSectionWriter::~TPEFSymbolSectionWriter() {
}

/**
 * Returns the section type this writer can write.
 *
 * @return The section type writer can write.
 */
Section::SectionType
TPEFSymbolSectionWriter::type() const {
    return Section::ST_SYMTAB;
}

/**
 * Writes the data of the section in to stream.
 *
 * @param stream The stream to be written to.
 * @param sect The section to be written.
 */
void
TPEFSymbolSectionWriter::actualWriteData(
    BinaryStream& stream,
    const Section* sect) const {

    FileOffset startOffset = stream.writePosition();

    // file offset to data of section
    SafePointer::addObjectReference(
        FileOffsetKey(startOffset), sect->element(0));

    // resolve identification code of currently written section
    SectionKey sectionKey = SafePointer::sectionKeyFor(sect);
    SectionId sectionId = sectionKey.sectionId();

    // first element should match with undefined symbol
    SymbolElement *undef = dynamic_cast<SymbolElement*>(sect->element(0));
    assert(undef != NULL);
    assert(undef->type() == SymbolElement::STT_NOTYPE);
    assert(undef->absolute() == true);
    assert(undef->binding() == SymbolElement::STB_LOCAL);
    assert(undef->name()->offset() == 0);

    // symbols might have to be organized in a way local symbols are
    // before weak symbols and global syms will come after everything else
    for (Word i = 0; i < sect->elementCount(); i++) {

        SymbolElement* elem = dynamic_cast<SymbolElement*>(sect->element(i));
        assert(elem != NULL);

        // each element must be registered with its index
        SafePointer::addObjectReference(
            SectionIndexKey(sectionId, i), elem);

        // write name
        assert(elem->name() != NULL);
        SectionOffsetReplacer sOffReplacer(elem->name());
        sOffReplacer.resolve();

        writeValueAndSize(stream, elem);

        // write info
        stream.writeByte(
            (elem->binding() << (BYTE_BITWIDTH / 2)) |
            static_cast<Byte>(elem->type()));

        // other field,  right now only STO_ABS is used
        Byte otherField = 0;
        if (elem->absolute()) {
            otherField = otherField | TPEFHeaders::STO_ABS;
        }
        stream.writeByte(otherField);

        // section to which the symbol belongs
        // NOTE: Maybe nullSection should be used for these,
        //       so NULL would be error.

        if (elem->section() != NULL) {
            SectionIdReplacer sectIdReplacer(elem->section());
            sectIdReplacer.resolve();
        } else {
            stream.writeHalfWord(0);
        }
    }
    SectionSizeReplacer::setSize(sect, stream.writePosition() - startOffset);
}

/**
 * Writes value and size fields of symbol.
 *
 * @param stream Stream where to write.
 * @param elem Element to write.
 */
void
TPEFSymbolSectionWriter::writeValueAndSize(
    BinaryStream &stream,
    const SymbolElement *elem) const {

    switch (elem->type()) {
    case SymbolElement::STT_NOTYPE: {
	    // write value
	    // write size
	    stream.writeWord(0);
	    stream.writeWord(0);
	    break;
	}

	case SymbolElement::STT_PROCEDURE:
	case SymbolElement::STT_CODE: {
 	    const CodeSymElement* codeSym =
            dynamic_cast<const CodeSymElement*>(elem);
	    assert(codeSym != NULL);

	    // write value
	    if (codeSym->reference() != NULL) {
            SectionOffsetReplacer replacer(codeSym->reference());
            replacer.resolve();
	    } else {
            stream.writeWord(0);
	    }

	    // write size
	    stream.writeWord(codeSym->size());
	    break;
	}

	case SymbolElement::STT_DATA: {
	    const DataSymElement* dataSym =
            dynamic_cast<const DataSymElement*>(elem);
	    assert(dataSym != NULL);

	    // write value
	    if (dataSym->reference() != NULL) {
            SectionOffsetReplacer replacer(dataSym->reference());
            replacer.resolve();
	    } else {
            stream.writeWord(0);
	    }

	    // write size
	    stream.writeWord(dataSym->size());
	    break;
	}

	case SymbolElement::STT_SECTION: {
	    const SectionSymElement* sectSym =
            dynamic_cast<const SectionSymElement*>(elem);
	    assert(sectSym != NULL);

	    // write value
	    stream.writeWord(sectSym->value());
	    // write size
	    stream.writeWord(sectSym->size());
	    break;
	}

	case SymbolElement::STT_FILE: {
	    const FileSymElement* fileSym =
            dynamic_cast<const FileSymElement*>(elem);
	    assert(fileSym != NULL);

	    // write value
	    stream.writeWord(fileSym->value());
	    // write size
	    stream.writeWord(0);
	    break;
	}

	default: {
	    bool unknownSymbolType = false;
	    assert(unknownSymbolType);
	}
	}
}

/**
 * Returns the fixed size length of section elements.
 *
 * @return The fixed size length of section elements.
 */
Word
TPEFSymbolSectionWriter::elementSize(const Section* /*section*/) const {
    return elementSize_;
}

/**
 * Creates needed keys for section if noBits flag is set.
 *
 * @param sect Section which for keys will be created.
 */
void
TPEFSymbolSectionWriter::createKeys(const Section* sect) const {
    SectionSizeReplacer::setSize(sect, 0x0);
}

}
