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
 * @file TPEFCodeSectionWriter.hh
 *
 * Declaration of TPEFCodeSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_CODE_SECTION_WRITER_HH
#define TTA_TPEF_CODE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Writes TPEF code section to file.
 */
class TPEFCodeSectionWriter : public TPEFSectionWriter {
protected:
    virtual ~TPEFCodeSectionWriter();
    TPEFCodeSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFCodeSectionWriter(const TPEFCodeSectionWriter&);
    /// Assignment not allowed.
    TPEFCodeSectionWriter& operator=(const TPEFCodeSectionWriter&);

    void writeAttributeField(
        BinaryStream& stream,
        SectionElement* elem,
        bool isEnd) const;

    void writeDataField(BinaryStream& stream, SectionElement* elem) const;

    void writeAnnotations(BinaryStream& stream, SectionElement* elem) const;

    /// Unique instance of class.
    static const TPEFCodeSectionWriter instance_;
    /// Maximum number of bytes that single immediate can contain.
    static const Byte IMMEDIATE_VALUE_MAX_BYTES;
};
}

#endif
