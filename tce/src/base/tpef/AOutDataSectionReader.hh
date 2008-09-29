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
 * @file AOutDataSectionReader.hh
 *
 * Declaration of AOutDataSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_DATA_SECTION_READER_HH
#define TTA_AOUT_DATA_SECTION_READER_HH

#include "AOutSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "ReferenceKey.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads data section from a.out binary file.
 */
class AOutDataSectionReader : public AOutSectionReader {
public:
    virtual ~AOutDataSectionReader();

protected:
    AOutDataSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    AOutDataSectionReader(const AOutDataSectionReader&);

    /// Prototype instance of AOutDataSectionReader to be registered to
    /// SectionReader.
    static AOutDataSectionReader proto_;
};
}

#endif
