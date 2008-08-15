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
 * @file TPEFSymbolSectionWriter.hh
 *
 * Declaration of TPEFSymbolSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SYMBOL_SECTION_WRITER_HH
#define TTA_TPEF_SYMBOL_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Writes TPEF symbol table to stream.
 */
class TPEFSymbolSectionWriter : public TPEFSectionWriter {
protected:
    TPEFSymbolSectionWriter();
    virtual ~TPEFSymbolSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFSymbolSectionWriter(const TPEFSymbolSectionWriter&);
    /// Assignment not allowed.
    TPEFSymbolSectionWriter operator=(const TPEFSymbolSectionWriter&);

    void writeValueAndSize(
        BinaryStream &stream,
        const SymbolElement* elem) const;

    /// An unique instance of class.
    static const TPEFSymbolSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
