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
 * @file TPEFSectionWriter.hh
 *
 * Declaration of TPEFSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SECTION_WRITER_HH
#define TTA_TPEF_SECTION_WRITER_HH

#include "SectionWriter.hh"

#include "BinaryWriter.hh"
#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Base class for all TPEF filetype writer classes.
 */
class TPEFSectionWriter : public SectionWriter {
protected:
    TPEFSectionWriter();
    virtual ~TPEFSectionWriter();

    virtual const BinaryWriter& parent() const;

    virtual void actualWriteHeader(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    /// Returns SectionTypet hat actual reader or writer instance can handle.
    virtual Section::SectionType type() const = 0;

    virtual void createKeys(const Section* sect) const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

    virtual SectionId getSectionId() const;

    virtual Word elementSize(const Section* section) const;

private:
    TPEFSectionWriter(const TPEFSectionWriter&);

    void writeBodyStartOffset(
        BinaryStream& stream,
        const Section* sect) const;
};
}

#endif
